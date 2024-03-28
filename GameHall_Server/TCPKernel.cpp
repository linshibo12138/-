#include<TCPKernel.h>
#include "packdef.h"
#include<stdio.h>
#include<sys/time.h>
#include"clogic.h"

using namespace std;



//设置网络协议映射
void TcpKernel::setNetPackMap()
{
    //清空映射
    bzero( m_NetPackMap , sizeof(m_NetPackMap) );

    //协议映射赋值
    m_logic->setNetPackMap();

}


TcpKernel::TcpKernel()
{

}

TcpKernel::~TcpKernel()
{
    if( m_logic ) delete m_logic;
    redisFree(m_redis);
}

TcpKernel *TcpKernel::GetInstance()
{
    static TcpKernel kernel;
    return &kernel;
}

int TcpKernel::Open( int port)
{
    initRand();
    //连接数据库
    m_sqlPool = MysqlConnectionPool::getConnectionPool();
    //    if((m_sql = m_sqlPool->getConnection())) {
    //        printf("Mysql Connect Success...\n");
    //    }
    if((m_redis = redisConnect("127.0.0.1",6379))->err)
    {
        redisFree(m_redis);
        printf("Conncet Redis Failed...\n");
        return FALSE;
    }
    else
    {
        printf("Redis Connect Success...\n");
    }


    //初始网络
    m_tcp = new Block_Epoll_Net;
    bool res = m_tcp->InitNet( port , &TcpKernel::DealData ) ;
    if( !res )
        err_str( "net init fail:" ,-1);

    m_logic = new CLogic(this);
    setNetPackMap();
    thread thread(std::bind(&CLogic::KeepClientAlive, this->m_logic));
    //分离线程（分离线程），主线程结束后该线程自动结束
    thread.detach();

    return TRUE;
}

void TcpKernel::Close()
{
    m_sqlPool->destroyPool();

}

//随机数初始化
void TcpKernel::initRand()
{
    struct timeval time;
    gettimeofday( &time , NULL);
    srand( time.tv_sec + time.tv_usec );
}

void TcpKernel::DealData(sock_fd clientfd,char *szbuf,int nlen)
{
    PackType type = *(PackType*)szbuf;
    if( (type >= DEF_PROTOCOL_BASE) && ( type < DEF_PROTOCOL_BASE + DEF_PROTOCOL_COUNT) )
    {
        PFUN pf = NetPackMap( type );
        if( pf )
        {
            (TcpKernel::GetInstance()->m_logic->*pf)( clientfd , szbuf , nlen);
        }
    }

    return;
}

void TcpKernel::EventLoop()
{
    printf("event loop\n");
    m_tcp->EventLoop();
}

void TcpKernel::SendData(sock_fd clientfd, char *szbuf, int nlen)
{
    m_tcp->SendData(clientfd , szbuf ,nlen );
}



