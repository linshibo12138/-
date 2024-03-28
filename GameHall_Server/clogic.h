#ifndef CLOGIC_H
#define CLOGIC_H

#include"TCPKernel.h"

class CLogic
{
public:
    CLogic( TcpKernel* pkernel )
    {
        m_pKernel = pkernel;
        m_sqlPool = pkernel->m_sqlPool;
        m_tcp = pkernel->m_tcp;
        m_redis = pkernel->m_redis;
    }
    
public:
    //设置协议映射
    void setNetPackMap();
    //心跳检测
    void KeepClientAlive();
    void GetClientAlive(sock_fd clientfd, char*szbuf, int nlen);
    /************** 发送数据*********************/
    void SendData( sock_fd clientfd, char*szbuf, int nlen )
    {
        m_pKernel->SendData( clientfd ,szbuf , nlen );
    }
    /************** 网络处理 *********************/
    ///注册登陆页面
    //处理注册请求
    void RegisterRq(sock_fd clientfd, char*szbuf, int nlen);

    //处理登录请求
    void LoginRq(sock_fd clientfd, char*szbuf, int nlen);


    ///大厅页面
    //处理上线请求
    void OnlineRq(sock_fd clientfd, char*szbuf, int nlen);
    //处理选择专区请求
    void ChooseZone(sock_fd clientfd, char*szbuf, int nlen);
    //处理文件上传请求
    void FileUpdownRq(sock_fd clientfd, char*szbuf, int nlen);
    //处理文件下载请求
    void FileDownloadRq(sock_fd clientfd, char*szbuf, int nlen);
    //处理下线请求
    void Offline(sock_fd clientfd, char*szbuf, int nlen);

    ///专区页面
    //处理快速匹配请求
    void QuickPlayRq(sock_fd clientfd, char*szbuf, int nlen);
    //处理专区内群聊请求
    void ChatInZoneRq(sock_fd clientfd, char*szbuf, int nlen);
    //处理添加好友请求
    void AddFriendRq(sock_fd clientfd, char*szbuf, int nlen);
    //处理好友聊天请求
    void ChatWithFriendRq(sock_fd clientfd, char*szbuf, int nlen);
    //处理选择房间请求
    void ChooseRoomRq(sock_fd clientfd, char*szbuf, int nlen);
    //处理退出专区请求
    void QuitFromZone(sock_fd clientfd, char*szbuf, int nlen);


    ///五子棋游戏页面
    //处理游戏开局请求
    void StartGameRq(sock_fd clientfd, char*szbuf, int nlen);
    //处理观战请求
    void WatchGameRq(sock_fd clientfd, char*szbuf, int nlen);
    //处理悔棋
    void RetractChess(sock_fd clientfd, char*szbuf, int nlen);
    //处理玩家视频/语音聊天(音频同步）
    void ChatByVideoRq(sock_fd clientfd, char*szbuf, int nlen);
    void ChatByAudioRq(sock_fd clientfd, char*szbuf, int nlen);
    //处理玩家文字聊天
    void ChatByCharRq(sock_fd clientfd, char*szbuf, int nlen);
    //处理双方落子信息交互
    void TransportGameInfoRq(sock_fd clientfd, char*szbuf, int nlen);
    //处理断线重连
    void ConnectAgain(sock_fd clientfd, char*szbuf, int nlen);
    //处理一局游戏结束通知（根据结果更改排名）
    void OneGameEndRq(sock_fd clientfd, char*szbuf, int nlen);
    //处理退出房间请求
    void QuitFromRoom(sock_fd clientfd, char*szbuf, int nlen);
    /*******************************************/


    void GetTotalRank();
private:
    TcpKernel* m_pKernel;
//    CMysql * m_sql;
    Block_Epoll_Net * m_tcp;
    redisContext * m_redis;
    queue<int>quick[3];
    MysqlConnectionPool* m_sqlPool;
    int CommandCountInRoom[1024];
    RoomInfo room[1024];
    PlayerInfo player[1024];
    MyMap<int, sock_fd> m_mapPlayeridToClientfd;//已上线平台的玩家id与玩家客户端和服务端之间建立连接的映射
    MyMap<int, struct PlayerInfo*>m_mapPlayeridToPlayerInfo;//该平台已上线玩家id与玩家信息结构体指针的映射(信息存储在mysql中，用户上线后将相关信息读出）
    MyMap<string, struct FileInfo*> m_mapFilekeyToFileinfo;//玩家上传的文件Key与文件信息结构体指针的映射（文件key:游戏文件名_上传游戏玩家id)
    MyMap<string, struct FileUploadTask*> m_mapUploadTaskidToTaskInfo;//玩家上传游戏的任务id与上传任务信息结构体指针的映射
    MyMap<string, struct FileDownloadTask*> m_mapDownloadTaskidToTaskInfo;//玩家下载游戏的任务id与上传任务信息结构体指针的映射
    MyMap<int, struct RoomInfo*>m_mapRoomidToRoominfo;//房间id与房间内信息的映射
    MyMap<int, unordered_set<int>>m_mapZoneidToPlayerinzone;//专区id与这个专区内玩家id集合的映射
    MyMap<int, long long> m_mapPlayeridToLatestOfflineTime;//玩家id与最近一次下线时间戳的映射
    MyMap<int, int> m_mapClientfdToChecktimes;//每条连接对应的心跳检测剩余次数

    //MyMap<int , int>m_mapPlayeridTo
    /*
    redis可用于
    服务端
    1.段位排行榜，用zipset
    2.缓存每个有玩家的房间的房间以及其中的玩家id key-value
    3.缓存每个房间每局游戏的落子指令，用于断线重连/观战/复盘
    客户端
    缓存专区以及好友聊天记录（有限制条数）
    缓存本端下棋指令

    mysql表
    1.玩家注册表（玩家id，玩家昵称，玩家手机号，玩家密码）
    2.玩家各专区信息表（序号，玩家id，专区id，该专区下胜局数、负局数、排行榜段位排名）
    3.玩家好友关系表（好友关系id，玩家Aid,玩家Bid）
    4.专区内群聊已读信息表（信息id，专区id，发送者id，发送时间戳，信息内容）
    5.单聊已读信息表（信息id，发送者id，接收者id，发送时间戳，信息内容）
    6.玩家未读信息表（信息id，接收者id，发送者id，是否是专区群聊，专区id，信息内容，时间戳）
    7.玩家上传文件表

    可以定义一个函数开辟一个线程专门定时检测房间数是否足够或空闲过多，如果不足够要及时补充房间

    */
};

#endif // CLOGIC_H
