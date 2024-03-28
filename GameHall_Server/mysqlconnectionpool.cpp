#include "mysqlconnectionpool.h"

MysqlConnectionPool *MysqlConnectionPool::getConnectionPool()
{
    //静态局部变量由编译器自动lock和unlock
    static MysqlConnectionPool pool;
    return &pool;
}

CMysql* MysqlConnectionPool::getConnection()
{
    unique_lock<mutex> lock(_queueMutex);
    while (_connectionQue.empty()) {
        if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeout)))
        {
            if (_connectionQue.empty()) {
                return nullptr;
            }
        }
    }
    CMysql* sp = _connectionQue.front();
    _connectionQue.pop();
    cv.notify_all();//消费完连接之后，通知生产者线程检查一下，如果生产队列为空后，就通知线程赶紧生产
    return sp;

}

MysqlConnectionPool::MysqlConnectionPool()
{
    _initSize = _DEF_SQLPOOL_INITSIZE;
    _maxSize = _DEF_SQLPOOL_MAXSIZE;
    _maxIdleTime = _DEF_SQLPOOL_MAXIDLETIME;
    _connectionTimeout = _DEF_SQLPOOL_CONNECTIONTIMEOUT;
    for (int i = 0; i < _initSize; i++) {
        CMysql* p = new CMysql();
        p->ConnectMysql( _DEF_DB_IP , _DEF_DB_USER, _DEF_DB_PWD, _DEF_DB_NAME );
        p->refreshAliveTime();
        _connectionQue.push(p);
        _connectionCnt++;
    }

    //启动一个新的线程，作为一个连接的生产者
    thread produce(std::bind(& MysqlConnectionPool::produceConnectionTask, this));
    //分离线程（分离线程），主线程结束后该线程自动结束
    produce.detach();
    //启动一个定时线程，扫描超过maxIdleTime时间的空闲线程
    thread scanner(std::bind(&MysqlConnectionPool::scannerConnectionTask, this));
    scanner.detach();

}


void MysqlConnectionPool::produceConnectionTask()
{
    while (true) {
        //所有的线程在创建时都被_queueMutex锁住了，共用一把锁，函数作用域结束后默认解锁
        //unique_lock无默认参数时会自动加锁
        unique_lock<mutex> lock(_queueMutex);
        while (!_connectionQue.empty()) {
            //condition_variable cv 必须和unique_lock一起使用
            cv.wait(lock);//队列不为空，此处生产者线程进入等待状态
        }
        if (_connectionCnt < _maxSize) {
            CMysql* p = new CMysql();
            p->ConnectMysql( _DEF_DB_IP , _DEF_DB_USER, _DEF_DB_PWD, _DEF_DB_NAME );
            p->refreshAliveTime();
            _connectionQue.push(p);
            _connectionCnt++;
        }
        cv.notify_all();//通知消费者线程可以进行连接了
    }
}

void MysqlConnectionPool::scannerConnectionTask()
{
    while (true)
    {
        //通过sleep模拟定时效果
        this_thread::sleep_for(chrono::seconds(_maxIdleTime));

        //扫描整个队列，释放多余的连接
        //队列要用互斥锁，防止多线程访问
        unique_lock<mutex> lock(_queueMutex);
        while (_connectionCnt > _initSize)
        {
            CMysql* p = _connectionQue.front();
            //如果队列头都没有超时的话，那么后面的connection肯定不会超时
            //每次回收返回队列都是插入在队尾的
            if (p->getAliveTime() >= _maxIdleTime)
            {
                _connectionQue.pop();
                _connectionCnt--;
                delete p;
            }
            else {
                break;
            }
        }
    }
}

void MysqlConnectionPool::closeConnection(CMysql *m_sql)
{
    //这里是在服务器应用线程中调用的，所以一定要考虑线程安全
    unique_lock<mutex> lock(_queueMutex);
    _connectionQue.push(m_sql);
    m_sql->refreshAliveTime();
}

void MysqlConnectionPool::destroyPool()
{
    while(!_connectionQue.empty()) {
        CMysql* p = _connectionQue.front();
        _connectionQue.pop();
        p->DisConnect();
    }
}
