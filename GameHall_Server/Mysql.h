#ifndef _MYSQL_H
#define _MYSQL_H
#include "packdef.h"
#include <mysql/mysql.h>
#include <list>
#include <string>
#include <ctime>
using namespace  std;


class CMysql{
public:
    int ConnectMysql(const char *server, const char *user, const char *password, const char *database);
    int SelectMysql(char* szSql,int nColumn,list<string>& lst);
    int UpdataMysql(char *szsql);
    void DisConnect();
    //刷新一下连接的起始的空闲时间
    void refreshAliveTime();
    //返回连接的存活时间
    clock_t getAliveTime();
private:
    MYSQL *conn;//表示和Mysql Server的一条连接
    clock_t alivetime;//记录进入空闲状态后的起始时间
//    MYSQL_RES *results;
//    MYSQL_ROW record;
    
};
#endif
