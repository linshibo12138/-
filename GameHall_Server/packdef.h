#ifndef _PACKDEF_H
#define _PACKDEF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include "err_str.h"
#include <malloc.h>
#include <string>
#include <iostream>
#include <map>
#include <list>
#include <unordered_set>
#include <chrono>
#include <iomanip>
#include <sys/types.h>
#include <hiredis/hiredis.h>
#include "./libco/co_closure.h"
#include "time.h"
using namespace std;
//边界值
#define _DEF_SIZE       (45)
#define _DEF_BUFFERSIZE (1000)
#define _DEF_PORT       (8000)
#define _DEF_SERVERIP "0.0.0.0"  //TODO

/*-------------数据库信息-----------------*/
#define _DEF_DB_NAME    "GameHall"
#define _DEF_DB_IP      "localhost"
#define _DEF_DB_USER    "root"
#define _DEF_DB_PWD     "541022"
#define _DEF_SQLPOOL_INITSIZE           (10)
#define _DEF_SQLPOOL_MAXSIZE            (1024)
#define _DEF_SQLPOOL_MAXIDLETIME        (60)
#define _DEF_SQLPOOL_CONNECTIONTIMEOUT  (100)
/*--------------------------------------*/
#define BOOL bool
#define TRUE true
#define FALSE false
typedef int file_fd;
typedef int sock_fd;
typedef int PackType;

#define HEART_CHECK_TIMES     (6) // 保活30秒，每5秒发一次心跳包，阈值为6

#define  MAX_PATH            (260)
#define  MAX_SIZE            (40)
#define  MAX_CONTENT_LEN     (4096)

#define  FILE_BLOCK_LEN      (512)

/////////////////////网络//////////////////////////////////////
#define  DEF_BUF_SIZE	     (4096)
#define _DEF_LISTEN          (128)
#define _DEF_EPOLLSIZE       (4096)
#define _DEF_IPSIZE          (16)
#define _DEF_COUNT           (10)
#define _DEF_TIMEOUT         (10)
#define _DEF_SQLLEN          (400)

struct FileInfo {
    string fileName;
    string fileKey;
    int fileLen;
    string fileStoreAddress;
    int uploaderID;
    int downloadedNum;
};

struct FileUploadTask {
    string taskID;
    file_fd filefd;
    int fileLen;
    int offset;
    int uploaderID;
    int blockNum;
};
struct FileDownloadTask {
    string taskID;
    file_fd filefd;
    int fileLen;
    off_t offset;
    int downloaderID;
    int blockNum;
};
struct PlayerInfo {
    int playerID;
    char playerName[MAX_SIZE];
    int zoneID;
    int successNum;
    int failNum;
    int rank;
    long long lastOfflineTime;
};
struct RoomInfo {
    int roomID;
    int hostID;
    int guestID;
    unordered_set<int> m_WatcherID;
    int userInRoom;
    bool isPlaying;
};

struct MsgInfo {
    char content[MAX_CONTENT_LEN];
    int senderID;
    int receiverID;
    bool isZoneGroup;
    int zoneID;
    long long sendTime;
};

struct GameInfo {
    int roomID;
    int senderID;
    int x;
    int y;
    int chessColor;
};

#define DEF_PROTOCOL_BASE          (10000)
#define DEF_PROTOCOL_COUNT         (100)

///注册登陆页面
//注册请求
#define  DEF_REGISTER_RQ            (DEF_PROTOCOL_BASE + 0)
//注册回复
#define  DEF_REGISTER_RS            (DEF_PROTOCOL_BASE + 1)
//登录请求
#define  DEF_LOGIN_RQ               (DEF_PROTOCOL_BASE + 2)
//登录回复
#define  DEF_LOGIN_RS               (DEF_PROTOCOL_BASE + 3)

///大厅页面
//上线请求
#define  DEF_ONLINE_RQ              (DEF_PROTOCOL_BASE + 4)
//加入专区
#define  DEF_JOIN_ZONE              (DEF_PROTOCOL_BASE + 5)
//文件上传请求
#define  DEF_FILE_UPLOAD_RQ         (DEF_PROTOCOL_BASE + 6)
//文件上传回复
#define  DEF_FILE_UPLOAD_RS         (DEF_PROTOCOL_BASE + 7)
//文件块上传请求
#define  DEF_FILEBLOCK_UPLOAD_RQ    (DEF_PROTOCOL_BASE + 8)
//文件块上传回复
#define  DEF_FILEBLOCK_UPLOAD_RS    (DEF_PROTOCOL_BASE + 9)
//文件下载请求
#define  DEF_FILE_DOWNLOAD_RQ       (DEF_PROTOCOL_BASE + 10)
//文件下载回复
#define  DEF_FILE_DOWNLOAD_RS       (DEF_PROTOCOL_BASE + 11)
//文件块下载请求
#define  DEF_FILEBLOCK_DOWNLOAD_RQ  (DEF_PROTOCOL_BASE + 12)
//文件块下载回复
#define  DEF_FILEBLOCK_DOWNLOAD_RS  (DEF_PROTOCOL_BASE + 13)
//下线请求
#define  DEF_OFFLINE_RQ             (DEF_PROTOCOL_BASE + 14)

///专区页面
//快速匹配请求
#define  DEF_QUICKPLAY_RQ           (DEF_PROTOCOL_BASE + 15)
//快速匹配回复
#define  DEF_QUICKPLAY_RS           (DEF_PROTOCOL_BASE + 16)
//专区内群聊请求
#define  DEF_CHATINZONE_RQ          (DEF_PROTOCOL_BASE + 17)
//添加好友请求
#define  DEF_ADDFRIEND_RQ           (DEF_PROTOCOL_BASE + 18)
//添加好友回复
#define  DEF_ADDFRIEND_RS           (DEF_PROTOCOL_BASE + 19)
//好友聊天请求
#define  DEF_CHATWITHFRIEND_RQ      (DEF_PROTOCOL_BASE + 20)
//选择房间请求
#define  DEF_JOIN_ROOM_RQ           (DEF_PROTOCOL_BASE + 21)
//选择房间回复
#define  DEF_JOIN_ROOM_RS           (DEF_PROTOCOL_BASE + 22)
//退出专区
#define  DEF_LEAVE_ZONE             (DEF_PROTOCOL_BASE + 23)

///房间页面
//开局请求
#define  DEF_STARTGAME_RQ           (DEF_PROTOCOL_BASE + 24)
//开局回复
#define  DEF_STARTGAME_RS           (DEF_PROTOCOL_BASE + 25)
//观战请求
#define  DEF_WATCHGAME_RQ           (DEF_PROTOCOL_BASE + 26)
//观战回复
#define  DEF_WATCHGAME_RS           (DEF_PROTOCOL_BASE + 27)
//同房间玩家文字交流请求
#define  DEF_CHATBYCHAR_RQ          (DEF_PROTOCOL_BASE + 28)
//落子信息交互请求
#define  DEF_TRANSPORTGAMEINFO_RQ   (DEF_PROTOCOL_BASE + 29)
//一局游戏结束通知
#define  DEF_ONEGAMEEND_RQ          (DEF_PROTOCOL_BASE + 30)
//离开房间
#define  DEF_LEAVE_ROOM_RQ          (DEF_PROTOCOL_BASE + 31)

//心跳检测
#define DEF_HEART_JUMP_RQ           (DEF_PROTOCOL_BASE + 32)
#define DEF_HEART_JUMP_RS           (DEF_PROTOCOL_BASE + 33)

//返回的结果
//注册请求的结果
#define  tel_is_exist           (0)
#define  register_success       (1)
#define  register_fail          (2)
//登录请求的结果
#define  user_not_exist         (0)
#define  password_error         (1)
#define  login_success          (2)
#define  user_online            (3)
#define  login_fail             (4)
//上传文件的结果
#define  file_is_exist          (0)
#define  start_upload           (1)
#define  upload_error           (2)
#define  block_success          (3)
#define  upload_finish          (4)
//下载文件的结果
#define  file_not_exist         (0)
#define  start_download         (1)
#define  download_error         (2)
#define  block_success          (3)
#define  download_finish        (4)
//添加好友的结果
#define  no_this_user           (0)
#define  user_refuse            (1)
#define  user_offline           (2)
#define  add_success            (3)
//加入房间的结果
#define no_this_room            (0)
#define join_success            (1)
//游戏开局请求的结果
#define  start_success          (0)
#define  start_fail             (1)

enum StatusOfFiveInLine {Host, Player, Watcher};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//心跳检测包
typedef struct STRU_HEART_JUMP {
    STRU_HEART_JUMP() : m_nType(DEF_HEART_JUMP_RQ) {}
    PackType m_nType;               //包类型
}STRU_HEART_JUMP;

//登录请求
typedef struct STRU_LOGIN_RQ
{
    STRU_LOGIN_RQ():m_nType(DEF_LOGIN_RQ)
    {
        memset(m_tel,0,MAX_SIZE);
        memset(m_password,0,MAX_SIZE);
    }

    PackType m_nType;               //包类型
    char     m_tel[MAX_SIZE];      //用户手机号
    char     m_password[MAX_SIZE];  //密码
} STRU_LOGIN_RQ;

//登录回复
typedef struct STRU_LOGIN_RS {
    STRU_LOGIN_RS():m_nType(DEF_LOGIN_RS), m_userID(0), m_lResult(0) {
        memset(m_name, 0, MAX_SIZE);
    }
    PackType m_nType;       //包类型
    int  m_userID;          //用户id
    int  m_lResult;        //注册结果
    char m_name[MAX_SIZE];  //用户名
} STRU_LOGIN_RS;

//注册请求
typedef struct STRU_REGISTER_RQ {
    STRU_REGISTER_RQ():m_nType(DEF_REGISTER_RQ) {
        memset(m_name, 0, MAX_SIZE);
        memset(m_tel,0,MAX_SIZE);
        memset(m_password,0,MAX_SIZE);
    }
    PackType m_nType;               //包类型
    char     m_name[MAX_SIZE];      //用户名
    char     m_tel[MAX_SIZE] ;      //手机号
    char     m_password[MAX_SIZE];  //密码

} STRU_REGISTER_RQ;

//注册回复
typedef struct STRU_REGISTER_RS {
    STRU_REGISTER_RS():m_nType(DEF_REGISTER_RS), m_lResult(0) {}
    PackType m_nType;   //包类型
    int  m_lResult;     //注册结果
} STRU_REGISTER_RS;


//上线请求
typedef struct STRU_ONLINE_RQ {
    STRU_ONLINE_RQ() : m_nType(DEF_ONLINE_RQ), m_userid(0), time(NULL){}
    PackType m_nType;   //包类型
    int  m_userid;      //用户id
    time_t time;        //上线时间戳 //TimestampUtil
}STRU_ONLINE_RQ;

//加入专区请求
typedef struct STRU_JOINZONE_RQ {
    STRU_JOINZONE_RQ() : m_nType(DEF_JOIN_ZONE), m_userID(0), m_zoneID(0){}
    PackType m_nType;   //包类型
    int m_userID;
    int m_zoneID;
}STRU_JOINZONE_RQ;

//文件上传请求
typedef struct STRU_FILE_UPLOAD_RQ {
    STRU_FILE_UPLOAD_RQ() :m_nType(DEF_FILE_UPLOAD_RQ), m_uploaderID(0) {
        memset(m_fileName, 0, MAX_SIZE);
    }
    PackType m_nType;   //包类型
    char m_fileName[MAX_SIZE];//文件名
    char m_uploaderID;//上传者id
}STRU_FILE_UPLOAD_RQ;

//文件上传回复
typedef struct STRU_FILE_UPLOAD_RS {
    STRU_FILE_UPLOAD_RS() : m_nType(DEF_FILE_UPLOAD_RS), m_result(0),m_UploadTaskID(""){}
    PackType m_nType;   //包类型
    int m_result;
    string m_UploadTaskID;

}STRU_FILE_UPLOAD_RS;

//文件块上传请求
typedef struct STRU_FILEBLOCK_UPLOAD_RQ {
    STRU_FILEBLOCK_UPLOAD_RQ() :m_nType(DEF_FILEBLOCK_UPLOAD_RQ), m_UploadTaskID("") {
        memset(m_fileBuf, 0, DEF_BUF_SIZE);
    }
    PackType m_nType;   //包类型
    string m_UploadTaskID;
    char m_fileBuf[DEF_BUF_SIZE];//文件块
}STRU_FILEBLOCK_UPLOAD_RQ;

//文件块上传回复
typedef struct STRU_FILEBLOCK_UPLOAD_RS {
    STRU_FILEBLOCK_UPLOAD_RS() :m_nType(DEF_FILEBLOCK_UPLOAD_RS), m_UploadTaskID(""), m_result(0), m_allOffset(0) {}
    PackType m_nType;   //包类型
    string m_UploadTaskID;
    int m_result;
    int m_allOffset;
}STRU_FILEBLOCK_UPLOAD_RS;

//文件下载请求
typedef struct STRU_FILE_DOWNLOAD_RQ {
    STRU_FILE_DOWNLOAD_RQ() :m_nType(DEF_FILE_DOWNLOAD_RQ),fileKey(""), m_downloaderID(0) {}
    PackType m_nType;   //包类型
    string fileKey;
    char m_downloaderID;//上传者id
}STRU_FILE_DOWNLOAD_RQ;

//文件下载回复
typedef struct STRU_FILE_DOWNLOAD_RS {
    STRU_FILE_DOWNLOAD_RS() : m_nType(DEF_FILE_DOWNLOAD_RS), m_result(0), m_downloadTaskID(""){}
    PackType m_nType;   //包类型
    int m_result;
    string m_downloadTaskID;
    struct FileInfo info;
    char m_fileBuf[DEF_BUF_SIZE];//文件块
}STRU_FILE_DOWNLOAD_RS;

//文件块下载请求
typedef struct STRU_FILEBLOCK_DOWNLOAD_RQ {
    STRU_FILEBLOCK_DOWNLOAD_RQ() :m_nType(DEF_FILEBLOCK_DOWNLOAD_RQ), m_downloadTaskID(""), m_result(0), m_allOffset(0) {

    }
    PackType m_nType;   //包类型
    string m_downloadTaskID;
    int m_result;
    int m_allOffset;
}STRU_FILEBLOCK_DOWNLOAD_RQ;

//文件块下载回复
typedef struct STRU_FILEBLOCK_DOWNLOAD_RS {
    STRU_FILEBLOCK_DOWNLOAD_RS() :m_nType(DEF_FILEBLOCK_DOWNLOAD_RS), m_downloadTaskID("") {
         memset(m_fileBuf, 0, DEF_BUF_SIZE);
    }
    PackType m_nType;   //包类型
    string m_downloadTaskID;
    char m_fileBuf[DEF_BUF_SIZE];//文件块
}STRU_FILEBLOCK_DOWNLOAD_RS;
//下线请求
typedef struct STRU_OFFLINE_RQ {
    STRU_OFFLINE_RQ() : m_nType(DEF_OFFLINE_RQ), m_userID(0), lastOfflineTime(0){}
    PackType m_nType;   //包类型
    int m_userID;
    long long lastOfflineTime;
}STRU_OFFLINE_RQ;

//快速匹配请求
typedef struct STRU_QUICKPLAY_RQ {
    STRU_QUICKPLAY_RQ() : m_nType(DEF_QUICKPLAY_RQ), m_userID(0) {}
    PackType m_nType;   //包类型
    int m_userID;
}STRU_QUICKPLAY_RQ;

//快速匹配回复
typedef struct STRU_QUICKPLAY_RS {
    STRU_QUICKPLAY_RS() : m_nType(DEF_QUICKPLAY_RS), info(nullptr) {}
    PackType m_nType;   //包类型
    RoomInfo* info;
}STRU_QUICKPLAY_RS;

//专区内群聊请求
typedef struct STRU_CHATINZONE_RQ {
    STRU_CHATINZONE_RQ() : m_nType(DEF_CHATINZONE_RQ), m_speakerID(0), m_zoneID(0) {
        memset(m_infoBuf, 0, DEF_BUF_SIZE);
    }
    PackType m_nType;   //包类型
    int m_speakerID;
    int m_zoneID;
    char m_infoBuf[DEF_BUF_SIZE];
}STRU_CHATINZONE_RQ;

//添加好友请求
typedef struct STRU_ADDFRIEND_RQ {
    STRU_ADDFRIEND_RQ(): m_nType(DEF_ADDFRIEND_RQ), m_addSenderID(0), m_addReceiverID(0) {}
    PackType m_nType;   //包类型
    int m_addSenderID;
    int m_addReceiverID;
}STRU_ADDFRIEND_RQ;

//添加好友回复
typedef struct STRU_ADDFRIEND_RS {
    STRU_ADDFRIEND_RS(): m_nType(DEF_ADDFRIEND_RQ), m_result(0), m_addSenderID(0), m_addReceiverID(0) {}
    PackType m_nType;   //包类型
    int m_result;
    int m_addSenderID;
    int m_addReceiverID;
}STRU_ADDFRIEND_RS;

//好友聊天请求
typedef struct STRU_CHATWITHFRIEND_RQ {
    STRU_CHATWITHFRIEND_RQ() : m_nType(DEF_CHATWITHFRIEND_RQ), m_sendID(0), m_receiverID(0) {
        memset(m_infoBuf, 0, DEF_BUF_SIZE);
    }
    PackType m_nType;   //包类型
    int m_sendID;
    int m_receiverID;
    char m_infoBuf[DEF_BUF_SIZE];
}STRU_CHATWITHFRIEND_RQ;

//选择房间请求
typedef struct STRU_JOINROOM_RQ {
    STRU_JOINROOM_RQ() : m_nType(DEF_JOIN_ROOM_RQ), m_UserID(0), m_RoomID(0) {}
    PackType m_nType;   //包类型
    int m_UserID;
    int m_RoomID;   
}STRU_JOINROOM_RQ;
//选择房间回复
typedef struct STRU_JOINROOM_RS {
    STRU_JOINROOM_RS() : m_nType(DEF_JOIN_ROOM_RS), m_UserID(0), m_RoomID(0), status(0) {}
    PackType m_nType;   //包类型
    int m_result;
    int m_UserID;
    int m_RoomID;
    int status;
}STRU_JOINROOM_RS;
//退出专区
typedef struct STRU_LEAVEZONE_RQ {
    STRU_LEAVEZONE_RQ() : m_nType(DEF_LEAVE_ROOM_RQ), m_userID(0), m_zoneID(0) {}
    PackType m_nType;   //包类型
    int m_userID; //用户ID
    int m_zoneID;
}STRU_LEAVEZONE_RQ;

//开局请求
typedef struct STRU_STARTGAME_RQ {
    STRU_STARTGAME_RQ() : m_nType(DEF_WATCHGAME_RQ), m_roomID(0) {}
    PackType m_nType;   //包类型
    int m_roomID;
}STRU_STARTGAME_RQ;
enum PLAYINGSTATE {playing = 0, waiting = 1};
//开局回复
typedef struct STRU_STARTGAME_RS {
    STRU_STARTGAME_RS() : m_nType(DEF_WATCHGAME_RQ), m_result(0), m_roomID(0) {}
    PackType m_nType;   //包类型
    int m_roomID;
    int m_result;
}STRU_STARTGAME_RS;
//观战请求
typedef struct STRU_WATCHGAME_RQ {
    STRU_WATCHGAME_RQ() : m_nType(DEF_WATCHGAME_RQ), m_userID(0), m_roomID(0) {}
    PackType m_nType;   //包类型
    int m_userID;
    int m_roomID;
}STRU_WATCHGAME_RQ;

//观战回复---暂缓，结合redis来写
typedef struct STRU_WATCHGAME_RS {
    STRU_WATCHGAME_RS() : m_nType(DEF_WATCHGAME_RS) {}
    PackType m_nType;   //包类型

}STRU_WATCHGAME_RS;
 //处理玩家文字聊天
typedef struct STRU_CHATBYCHAR_RQ {
    STRU_CHATBYCHAR_RQ() : m_nType(DEF_CHATBYCHAR_RQ), m_roomID(0), m_senderID(0) {
        memset(m_infoBuf, 0, DEF_BUF_SIZE);
    }
    PackType m_nType;   //包类型
    int m_roomID;
    int m_senderID;
    char m_infoBuf[DEF_BUF_SIZE];
}STRU_CHATBYCHAR_RQ;

//落子信息交互请求
typedef struct STRU_TRANSPORTGAMEINFO_RQ {
    STRU_TRANSPORTGAMEINFO_RQ() : m_nType(DEF_TRANSPORTGAMEINFO_RQ) {}
    PackType m_nType;   //包类型
    GameInfo info;//黑\白子落在(x,y)点
}STRU_TRANSPORTGAMEINFO_RQ;

//一局游戏结束通知
typedef struct STRU_ONEGAMEEND_RQ {
    STRU_ONEGAMEEND_RQ() : m_nType(DEF_ONEGAMEEND_RQ), roomID(0), winnerID(0), failerID(0){}
    PackType m_nType;   //包类型
    int roomID;
    int winnerID;
    int failerID;
}STRU_ONEGAMEEND_RQ;

//离开房间请求
typedef struct STRU_LEAVEROOM_RQ
{
    STRU_LEAVEROOM_RQ() : m_nType(DEF_LEAVE_ROOM_RQ), m_userId(0), m_roomId(0) {}
    PackType   m_nType;   //包类型
    int    m_userId; //用户ID
    int    m_roomId;
}STRU_LEAVEROOM_RQ;




///音频数据帧
/// 成员描述
/// int type;
/// int userId;
/// int roomId;
/// int min;
/// int sec;
/// int msec;
/// QByteArray audioFrame;  --> char frame[]; 柔性数组
///


///视频数据帧
/// 成员描述
/// int type;
/// int userId;
/// int roomId;
/// int min;
/// int sec;
/// int msec;
/// QByteArray videoFrame;
///



#endif



