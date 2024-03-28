#include "block_epoll_net.h"
#include "block_epoll_net.h"


bool Block_Epoll_Net::InitNet(int port, void (*recv_callback)(int, char *, int))
{
    //creat a tcp socket
    if((m_listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("create socket error");
        return false;
    }

    //set REUSERADDR
    int flags = 1;
    if ((setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&flags, sizeof(flags)))== -1 ){
        perror("setsockopt error");
        return false;
    }
    //set NONBLOCK
    setNonBlockFd( m_listenfd );//监听套接字m_listenfd 采用 LT 非阻塞模式

    //set address
    struct sockaddr_in local_addr;
    bzero( &local_addr , sizeof(sockaddr_in) );
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;
    //bind addr
    if((bind(m_listenfd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr_in))) == -1 ) {
        perror("bind error");
        close(m_listenfd);
        return false;
    }
    //listen
    if (listen(m_listenfd, 128) == -1 ){
        perror("listen error");
        close(m_listenfd);
        return false;
    }

    //set callback
    m_recv_callback = recv_callback;
    //create epoll
    m_epoll_fd = epoll_create( MAX_EVENTS );
    m_listenEv = new myevent_s(this);
    m_listenEv->eventset(m_listenfd ,m_epoll_fd );
    //将监听套接字 添加到epoll中 , 模式LT 非阻塞
    m_listenEv->eventadd( EPOLLIN);
    // init threadpool
    InitThreadPool();
    return true;
}
/*
    区别：
    1、使用目的上：
        reuseaddr：这个套接字选项通知内核，如果端口忙，但TCP状态位于TIME_WAIT ，可以重用端口。
                   如果端口忙，而TCP状态位于其他状态，重用端口时依旧得到一个错误信息，指明"地址已经使用中"。
                   如果你的服务程序停止后想立即重启，而新套接字依旧使用同一端口，此时SO_REUSEADDR 选项非常有用。
        reuseport：SO_REUSEPORT支持多个进程或者线程绑定到同一端口，提高服务器程序的性能
    2、用法上：
        reuseaddr：绑定相同端口，但是IP地址需要不同
        reuseport：绑定相同端口，且IP地址需要相同
*/
bool Block_Epoll_Net::InitThreadPool()
{
    m_pool = NULL;
    m_threadpool = new thread_pool;
    //创建拥有10个线程的线程池 最大线程数200 环形队列最大值50000
    if((m_pool = (m_threadpool->Pool_create(200,10,50000))) == NULL) {
        perror("Create Thread_Pool Failed:");
        exit(-1);
    }
    return true;
}

void Block_Epoll_Net::EventLoop()
{
    printf("EventLoop:server running\n");
    int  i = 0;
    while (1) {
        int nfd = epoll_wait( m_epoll_fd, events, MAX_EVENTS+1, 1000);
        if (nfd < 0) {
            printf("epoll_wait error, exit\n");
            continue;
        }
        for (i = 0; i < nfd; i++) {
            struct myevent_s *ev = (struct myevent_s *)events[i].data.ptr;
            int fd = ev->fd;
            if ( (events[i].events & EPOLLIN) ) {
                if( fd == m_listenfd )
                    accept_event();
                else
                    recv_event(ev);
            }
            if ((events[i].events & EPOLLOUT) ) {
                epollout_event(ev);
            }
        }
    }
}

void Block_Epoll_Net::accept_event()
{
    struct sockaddr_in caddr;
    socklen_t len = sizeof(caddr);
    int clientfd ;
    if ((clientfd = accept(m_listenfd, (struct sockaddr *)&caddr, &len)) == -1) {
        if (errno != EAGAIN && errno != EINTR) {}
        printf("%s: accept, %s\n", __func__, strerror(errno));
        return;
    }

    //设置非阻塞
    setNonBlockFd( clientfd );
    //设置接收缓冲区大小
    setRecvBufSize( clientfd );
    //设置发送缓冲区大小
    setRecvBufSize( clientfd );
    //设置 无延迟
    setNoDelay( clientfd );

    myevent_s * clientEv = new myevent_s(this);
    clientEv->eventset( clientfd , m_epoll_fd );
    // 使用EPOLLONESHOT epoll监听不会重复检测 避免多线程并发 使得同一个套接字接收是排队的
    clientEv->eventadd(  EPOLLIN|EPOLLONESHOT/*|EPOLLET|*/ );

    m_mapSockfdToEvent.insert(clientfd , clientEv);

    printf("new connect [%s:%d][time:%ld] \n",
           inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port), time(NULL) );
}
void Block_Epoll_Net::recv_event( myevent_s *ev)
{
    //接收事件采用线程池 多线程处理, 由于使用EPOLLONESHOT , 避免同一套机字的线程并发问题
    m_threadpool->Producer_add( m_pool , recv_task , (void*) ev );
}
void Block_Epoll_Net::epollout_event( myevent_s *ev)
{
    // epoll LT模式 阻塞模式 发送阻塞 , 不用监听EPOLLOUT事件
    //接收事件采用线程池 多线程处理, 由于使用EPOLLONESHOT , 避免同一套机字的线程并发问题
    m_threadpool->Producer_add( m_pool , send_task , (void*)NULL );
    // 这次接收完 要重新注册事件  此时 EPOLL MODE -> EPOLLIN|EPOLLONESHOT 没有修改, 使用重复值
    ev->eventadd(EPOLLIN|EPOLLONESHOT);

}


void* Block_Epoll_Net::recv_task(void* arg)
{
    myevent_s * ev = (myevent_s*)arg;
    //利用全局指针 方便操作
    Block_Epoll_Net * pthis = ev->pNet;

    //接收和处理分离
    int nRelReadNum = 0;
    int nPackSize = 0;
    char *pSzBuf = NULL;
    do {
        if((nRelReadNum = read(ev->fd,&nPackSize,sizeof(nPackSize))) <= 0 ){
            break;
        }
        pSzBuf = new char[nPackSize];
        int nOffSet = 0;
        nRelReadNum = 0;
        //接收包的数据
        while(nPackSize)
        {
            nRelReadNum = recv(ev->fd,pSzBuf+nOffSet,nPackSize,0);
            if(nRelReadNum == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    continue;
                }
                perror("read");
                exit(-1);
            }
            else if(nRelReadNum == 0) {
                //断线
            }
            else {
                nOffSet += nRelReadNum;
                nPackSize -= nRelReadNum;
            }
        }
        //接收和处理分离 跑线程池里其他线程处理 , 避免处理影响接收
        DataBuffer * buffer = new DataBuffer(ev->pNet , ev->fd , pSzBuf , nOffSet );
        pthis->m_threadpool->Producer_add( pthis->m_pool , Buffer_Deal , (void*) buffer );

        // 这次接收完 要重新注册事件  此时 EPOLL MODE -> EPOLLIN|EPOLLONESHOT 没有修改, 使用重复值
        ev->eventadd(ev->events);

        return nullptr;

    }while(0);

    // 借助 do..while 在最后统一处理错误  避免使用goto语句
    ev->eventdel();
    close(ev->fd);
    //回收event结构
    pthis->m_mapSockfdToEvent.erase( ev->fd );
    delete ev;
    return nullptr;

}

void* Block_Epoll_Net::send_task(void*) {
    struct SendTask* st = nullptr;
    //从读写队列中取出数据
    while(SendQue.size() > 0) {
        SendQue.pop(st);
        send(st->m_sockfd, (const char*)st->m_buf, st->m_nlen, 0);
        fdSend[st->m_sockfd]--;
    }
    return nullptr;
}


void * Block_Epoll_Net::Buffer_Deal( void * arg )
{
    DataBuffer * buffer = (DataBuffer *)arg;
    if( !buffer ) return NULL;

    buffer->pNet->m_recv_callback(buffer->sockfd,buffer->buf,buffer->nlen);

    if(buffer->buf != NULL) {
        delete [] buffer->buf;
        buffer->buf = NULL;
    }
    delete buffer;
    return nullptr;
}



void Block_Epoll_Net::SendData(int fd, char *szbuf, int nlen)
{
    // 发送不可分割 避免多线程并发
    // 先包大小, 再数据包 , 一次放入缓冲区

    /*
     +--------------+------------------+---------------------+
     |<-- 4bytes -->|<-- 4bytes协议头-->|<--  协议其他内容   -->|
     +--------------+------------------+---------------------+
     |<- packsize ->|<------------ 数据包 struct ------------>|
     * */

    int nPackSize = nlen + 4;
    vector<char> vecbuf( nPackSize , 0);
    //vecbuf.resize( nPackSize );

    char* buf = &* vecbuf.begin();
    char* tmp = buf;
    *(int*)tmp = nlen;//按四个字节int写入
    tmp += sizeof(int );
    memcpy( tmp , szbuf , nlen );

    if(fdSend[fd] == 0) {//读写队列中没有这个fd对应的包
        int res = send( fd,(const char *)buf, nPackSize ,0);
        if(res > 0 && res < nPackSize) {
            char* ch = new char[4096];
            memcpy(ch, buf + res, nPackSize - res);
            struct SendTask st(fd, ch, nPackSize - res);
            SendQue.push(&st);
            fdSend[fd]++;
            // 这次接收完 要重新注册事件  此时 EPOLL MODE -> EPOLLIN|EPOLLONESHOT|EPOLLOUT
            struct myevent_s* ev = new myevent_s(this);
            m_mapSockfdToEvent.find(fd, ev);
            ev->eventadd(ev->events | EPOLLOUT);
        }
    }
    else {
        struct SendTask st(fd, buf, nPackSize);
        SendQue.push(&st);
        fdSend[fd]++;
        // 这次接收完 要重新注册事件  此时 EPOLL MODE -> EPOLLIN|EPOLLONESHOT|EPOLLOUT
        struct myevent_s* ev = new myevent_s(this);
        m_mapSockfdToEvent.find(fd, ev);
        ev->eventadd(ev->events | EPOLLOUT);
    }

}



void Block_Epoll_Net::setNonBlockFd(int fd)
{
    int flags = 0;
    flags = fcntl(fd, F_GETFL, 0);
    int ret = fcntl(fd, F_SETFL, flags|O_NONBLOCK);
    if( ret == -1)
        perror("setNonBlockFd fail:");
}

void Block_Epoll_Net::setRecvBufSize( int fd)
{ /*clientfd*/
    //接收缓冲区
    int nRecvBuf = 256*1024;//设置为 256 K
    setsockopt(fd,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));
}

void Block_Epoll_Net::setSendBufSize( int fd)
{
    //发送缓冲区
    int nSendBuf=128*1024;//设置为 128 K
    setsockopt(fd,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));
}

#include<netinet/tcp.h>
void Block_Epoll_Net::setNoDelay( int fd)
{
    //nodelay
    int value = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY ,(char *)&value, sizeof(int));
}
