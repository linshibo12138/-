#ifndef MYSQLCONNECTIONPOOL_H
#define MYSQLCONNECTIONPOOL_H
#include<string>
#include<queue>
#include<mutex>
#include<atomic>
#include<thread>
#include<memory>
#include<functional>
#include<condition_variable>
#include"Mysql.h"



class MysqlConnectionPool
{
public:

    //获取连接池对象实例，静态成员方法，不依赖于对象调用
    static MysqlConnectionPool* getConnectionPool();
    //从线程池中获取线程
    CMysql* getConnection();
    //关闭单条连接
    void closeConnection(CMysql* m_sql);
    //销毁连接池
    void destroyPool();
private:
    MysqlConnectionPool(); // 单例，构造函数私有化
    //加载配置文件
    bool loadConfigFile();

    //用来线程来产生连接
    void produceConnectionTask();
    //用来扫描连接的进程函数，防止很线程池中线程数量大于initSize而不归还资源
    void scannerConnectionTask();


    string _ip; //mysql的ip地址
    unsigned short _port; //mysql的端口号
    string _username; //mysql的连接用户名
    string _password; //mysql连接用户的密码
    string _dbname;
    int _initSize; //初始化连接池的数量
    int _maxSize; //最大化连接池的数量
    int _maxIdleTime; //连接池最大空闲时间
    int _connectionTimeout;//连接池最大获取时间
    queue<CMysql*> _connectionQue; //连接池存储数据库连接队列，必须是多线程安全的
    mutex _queueMutex;//维护连接池队列线程安全的互斥锁
    atomic_int _connectionCnt;//记录所创建的connection数量

    condition_variable cv;//设置条件变量，用于连接生产线程和连接消费线程的实现

};

#endif // MYSQLCONNECTIONPOOL_H
