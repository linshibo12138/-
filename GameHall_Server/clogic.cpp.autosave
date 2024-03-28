#include "clogic.h"

void CLogic::setNetPackMap()
{
    NetPackMap(DEF_REGISTER_RQ)             = &CLogic::RegisterRq;
    NetPackMap(DEF_LOGIN_RQ)                = &CLogic::LoginRq;
    //NetPackMap(DEF_ONLINE_RQ)               = &CLogic::OnlineRq;
    NetPackMap(DEF_JOIN_ZONE)               = &CLogic::ChooseZone;
    NetPackMap(DEF_FILE_UPLOAD_RQ)          = &CLogic::FileUpdownRq;
    NetPackMap(DEF_FILEBLOCK_UPLOAD_RQ )    = &CLogic::FileUpdownRq;
    NetPackMap(DEF_FILE_DOWNLOAD_RQ)        = &CLogic::FileDownloadRq;
    NetPackMap(DEF_FILEBLOCK_DOWNLOAD_RQ)   = &CLogic::FileDownloadRq;
    NetPackMap(DEF_OFFLINE_RQ)              = &CLogic::Offline;
    NetPackMap(DEF_QUICKPLAY_RQ)            = &CLogic::QuickPlayRq;
    NetPackMap(DEF_CHATINZONE_RQ)           = &CLogic::ChatInZoneRq;
    NetPackMap(DEF_ADDFRIEND_RQ)            = &CLogic::AddFriendRq;
    NetPackMap(DEF_ADDFRIEND_RS)            = &CLogic::AddFriendRq;
    NetPackMap(DEF_CHATWITHFRIEND_RQ)       = &CLogic::ChatWithFriendRq;
    NetPackMap(DEF_JOIN_ROOM_RQ)            = &CLogic::ChooseRoomRq;
    NetPackMap(DEF_LEAVE_ZONE)              = &CLogic::QuitFromZone;
    NetPackMap(DEF_STARTGAME_RQ)            = &CLogic::StartGameRq;
    NetPackMap(DEF_WATCHGAME_RQ)            = &CLogic::WatchGameRq;
    NetPackMap(DEF_TRANSPORTGAMEINFO_RQ)    = &CLogic::TransportGameInfoRq;
    NetPackMap(DEF_ONEGAMEEND_RQ)           = &CLogic::OneGameEndRq;
    NetPackMap(DEF_LEAVE_ROOM_RQ)           = &CLogic::QuitFromRoom;
    NetPackMap(DEF_CHATBYCHAR_RQ)           = &CLogic::ChatWithFriendRq;
}



void CLogic::KeepClientAlive()
{
    while(1) {
        sleep(150);//隔五分钟发一次心跳探测
        int count = m_mapPlayeridToClientfd.getCount();
        if(count > 0) {
            STRU_HEART_JUMP rq;
            for(int i = 0; i < count; ++i) {
                int sock;
                int checkTimes;
                m_mapPlayeridToClientfd.find(i, sock);
                m_mapClientfdToChecktimes.find(sock, checkTimes);
                if(checkTimes > 0) {
                    SendData(sock, (char*)&rq, sizeof(rq));
                    checkTimes--;
                    m_mapClientfdToChecktimes.update(sock, checkTimes);
                }
                else {
                    m_mapPlayeridToClientfd.erase(i);
                    m_mapClientfdToChecktimes.erase(sock);
                    close(sock);
                }
            }
        }
    }

}

void CLogic::GetClientAlive(sock_fd clientfd, char *szbuf, int nlen)
{
    STRU_HEART_JUMP* rs = (STRU_HEART_JUMP*) szbuf;
    if(rs->m_nType == DEF_HEART_JUMP_RQ) {
        rs->m_nType = DEF_HEART_JUMP_RS;
        int checktimes;
        m_mapClientfdToChecktimes.find(clientfd, checktimes);
        checktimes = HEART_CHECK_TIMES;
        m_mapClientfdToChecktimes.update(clientfd, checktimes);        SendData(clientfd, (char*)rs, sizeof(*rs));
    }
    else {
        int checktimes;
        m_mapClientfdToChecktimes.find(clientfd, checktimes);
        checktimes = HEART_CHECK_TIMES;
        m_mapClientfdToChecktimes.update(clientfd, checktimes);
    }
}


//注册
void CLogic::RegisterRq(sock_fd clientfd,char* szbuf,int nlen)
{
    printf("clientfd:%d RegisterRq\n", clientfd);
    STRU_REGISTER_RQ* rq = (STRU_REGISTER_RQ*)szbuf;
    char tel[MAX_SIZE], name[MAX_SIZE], password[MAX_SIZE];
    memset(tel, 0, MAX_SIZE);
    memset(name, 0, MAX_SIZE);
    memset(password, 0, MAX_SIZE);
    strcpy(tel, rq->m_tel);
    strcpy(name, rq->m_name);
    strcpy(password, rq->m_password);
    char sqlbuf[DEF_BUF_SIZE];
    memset(sqlbuf, 0, MAX_SIZE);
    STRU_REGISTER_RS rs;
    //检验
    //检验手机号是否注册过帐号

    //检验通过后将注册信息添加到数据库中
    memset(sqlbuf, 0, MAX_SIZE);
    sprintf(sqlbuf, "insert into playerList(name, tel, password) values(%s, %s, %s);", tel, name, password);
    CMysql* m_sql = m_sqlPool->getConnection();
    if((m_sql->UpdataMysql(sqlbuf)) != TRUE) {
        printf("update mysql fail");
        rs.m_lResult = register_fail;
    }
    else {
        rs.m_lResult = register_success;
    }
    SendData(clientfd, (char*)&rs, sizeof(rs));

}

//登录
void CLogic::LoginRq(sock_fd clientfd ,char* szbuf,int nlen)
{
    printf("clientfd:%d LoginRq\n", clientfd);
    STRU_LOGIN_RQ* rq = (STRU_LOGIN_RQ*)szbuf;
    char tel[MAX_SIZE], password[MAX_SIZE];
    memset(tel, 0, MAX_SIZE);
    memset(password, 0, MAX_SIZE);
    strcpy(tel, rq->m_tel);
    strcpy(password, rq->m_password);
    char sqlbuf[DEF_BUF_SIZE];
    memset(sqlbuf, 0, DEF_BUF_SIZE);
    list<string> sqlList;
    STRU_LOGIN_RS  rs;
    //查询该tel是否注册过
    sprintf(sqlbuf, "select num, name, password from playerList where tel = %s", tel);
    CMysql* m_sql = m_sqlPool->getConnection();
    if((m_sql->SelectMysql(sqlbuf, 3, sqlList)) == FALSE) {
        printf("Login Select mysql fail");
        rs.m_lResult = login_fail;
    }
    else {
        if(sqlList.size() <= 0) {
            rs.m_lResult = user_not_exist;
        }
        else {
            int playerID = stoi((sqlList.front()).c_str());
            sqlList.pop_front();
            char playerName[MAX_SIZE];
            memset(playerName, 0, MAX_SIZE);
            strcpy(playerName, (sqlList.front()).c_str());
            sqlList.pop_front();
            char playerPassword[MAX_SIZE];
            memset(playerPassword, 0, MAX_SIZE);
            if((strcmp(playerPassword, (sqlList.front()).c_str())) != 0) {
                rs.m_lResult = password_error;
                sqlList.pop_front();
            }
            else {
                if(m_mapPlayeridToClientfd.IsExist(playerID)) {
                    rs.m_lResult = user_online;
                }
                else {
                    m_mapPlayeridToClientfd.insert(playerID, clientfd);
                    rs.m_lResult = login_success;
                    rs.m_userID = playerID;
                    strcpy(rs.m_name, playerName);
                    //将下线期间未读消息按发送者id发送过去
                    sqlList.clear();
                    MsgInfo info;
                    //查询
                    sprintf(sqlbuf, "select messageID, senderID, isZoneGroup, zoneID, msg, ctime from unreadMessage where receiverID = %d", playerID);
                    if((m_sql->SelectMysql(sqlbuf, 6, sqlList)) == FALSE) {
                        printf("unreadMsg Select mysql fail");
                    }
                    else {
                        if(sqlList.size() <= 0) {
                            printf("该用户无未读信息");
                        }
                        else {
                            //将未读消息信息读取出来

                            //发送给该用户

                            //将该消息写入对应的已读信息表
                        }
                    }
                }
            }
        }
    }
    SendData(clientfd, (char*)&rs, sizeof(rs));
}


#include "openssl/md5.h"

int getMd5File(const char* filePath, char* result, int byteSize)
{
    FILE *file = fopen(filePath, "rb");
    if (file)
    {
        MD5_CTX c;
        MD5_Init(&c);
        
        char buffer[1024*100];
        while (!feof(file))
        {
            size_t numberOfObjects = fread(buffer, sizeof(char), 1024*100, file);
            MD5_Update(&c, buffer, numberOfObjects);
        }
        fclose(file);

        unsigned char digest[16] = { 0 };
        char hexBuffer[3];
        MD5_Final(digest, &c);
        result[0] = '\0';
        for (size_t i = 0; i != 16; ++i)
        {
            if (digest[i] < 16)     sprintf(hexBuffer, "0%x", digest[i]);
            else                    sprintf(hexBuffer, "%x", digest[i]);
            strcat(result, hexBuffer);
        }
        result[32] = '\0';

        if(byteSize == 16)
        {
            char temp[17];
            snprintf(temp, 17, "%s", result + 8);
            temp[16] = '\0';
            sprintf(result, "%s", temp);
        }
    }
    else
    {
        return 1;
    }
    return 0;
}

void CLogic::ChooseZone(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d ChooseZone\n", clientfd);
    STRU_JOINZONE_RQ* rq = (STRU_JOINZONE_RQ*)szbuf;
    struct PlayerInfo* info = nullptr;
    m_mapPlayeridToPlayerInfo.find(rq->m_userID, info);
    info->zoneID = rq->m_zoneID;
}

void CLogic::FileUpdownRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d FileUpdownRq\n", clientfd);
    STRU_FILE_UPLOAD_RQ* rq = (STRU_FILE_UPLOAD_RQ*)szbuf;

}
#include <sys/sendfile.h>
void CLogic::FileDownloadRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d FileDownloadRq\n", clientfd);
    PackType type = *szbuf;
    if(type == DEF_FILE_DOWNLOAD_RQ) {//文件下载请求
        STRU_FILE_DOWNLOAD_RQ* rq = (STRU_FILE_DOWNLOAD_RQ*)szbuf;
        struct FileInfo* info = nullptr;
        STRU_FILE_DOWNLOAD_RS rs;
        if(m_mapFilekeyToFileinfo.find(rq->fileKey, info) ==false) {
            rs.m_result = file_not_exist;
            SendData(clientfd, (char*)&rs, sizeof(rs));
        }
        else {
            rs.m_result = start_download;
            rs.info = *info;
            struct FileDownloadTask task;
            task.filefd = open(info->fileStoreAddress.c_str(), O_RDWR);
            task.offset = 0;
            task.fileLen = info->fileLen;
            task.downloaderID = rq->m_downloaderID;
            task.taskID = rq->fileKey + '_' + to_string(rq->m_downloaderID);
            task.blockNum = info->fileLen / FILE_BLOCK_LEN;
            m_mapDownloadTaskidToTaskInfo.insert(task.taskID, &task);
            rs.m_downloadTaskID = task.taskID;
            lseek(task.filefd, 0, SEEK_SET);
            read(task.filefd, rs.m_fileBuf, DEF_BUF_SIZE);
            SendData(clientfd, (char*)&rs, sizeof(rs));

        }
    }
    else {//文件块下载请求
        STRU_FILEBLOCK_DOWNLOAD_RQ* rq = (STRU_FILEBLOCK_DOWNLOAD_RQ*)szbuf;
        FileDownloadTask* task;
        task->offset = rq->m_allOffset;
        m_mapDownloadTaskidToTaskInfo.find(rq->m_downloadTaskID, task);

        sendfile(clientfd,task->filefd, &task->offset, DEF_BUF_SIZE);
    }
}

void CLogic::Offline(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d Offline\n", clientfd);
    STRU_OFFLINE_RQ* rq = (STRU_OFFLINE_RQ*)szbuf;
    
    m_mapPlayeridToClientfd.erase(rq->m_userID);
    
    m_mapPlayeridToLatestOfflineTime.insert(rq->m_userID, rq->lastOfflineTime);

}

void CLogic::QuickPlayRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d QuickPlayRq\n", clientfd);
}

void CLogic::ChatInZoneRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d ChatInZoneRq\n", clientfd);
}

void CLogic::AddFriendRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d AddFriendRq\n", clientfd);
}

void CLogic::ChatWithFriendRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d ChatWithFriendRq\n", clientfd);
}

void CLogic::ChooseRoomRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d ChooseRoomRq\n", clientfd);
    STRU_JOINROOM_RQ* rq = (STRU_JOINROOM_RQ*) szbuf;
    struct RoomInfo* info = nullptr;
    STRU_JOINROOM_RS rs;
    rs.m_RoomID = rq->m_RoomID;
    rs.m_UserID = rq->m_UserID;
    if((m_mapRoomidToRoominfo.find(rq->m_RoomID, info)) == FALSE) {
        rs.m_result = no_this_room;
    }
    else {
        rs.m_result = join_success;
        if(info->userInRoom == 0) {
            rs.status = StatusOfFiveInLine::Host;
            info->hostID = rq->m_UserID;
            info->userInRoom++;
        }
        else if(info->userInRoom == 1) {
            rs.status = StatusOfFiveInLine::Player;
            info->guestID = rq->m_UserID;
            info->userInRoom++;
        }
        else if(info->userInRoom >= 2) {
            rs.status = StatusOfFiveInLine::Watcher;
            info->m_WatcherID.insert(rq->m_UserID);
            info->userInRoom++;
        }
    }
    SendData(clientfd, (char*)&rs, sizeof(rs));
}

void CLogic::QuitFromZone(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d QuitFromZone\n", clientfd);
    STRU_LEAVEZONE_RQ* rq = (STRU_LEAVEZONE_RQ*)szbuf;
    struct PlayerInfo* info = nullptr;
    unordered_set<int> roominfo;
    m_mapPlayeridToPlayerInfo.find(rq->m_userID, info);
    m_mapZoneidToPlayerinzone.find(info->zoneID, roominfo);
    roominfo.erase(rq->m_userID);
    info->zoneID = 0;
}

void CLogic::StartGameRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d StartGameRq\n", clientfd);
    STRU_STARTGAME_RQ* rq = (STRU_STARTGAME_RQ*)szbuf;
    int roomID = rq->m_roomID;
    struct RoomInfo* info = nullptr;
    m_mapRoomidToRoominfo.find(roomID, info);
    STRU_STARTGAME_RS rs;
    rs.m_roomID = roomID;
    if(info->userInRoom < 2 || info->isPlaying == PLAYINGSTATE::playing) {
        rs.m_result = start_fail;
    }
    else {
        rs.m_result = start_success;
    }
    SendData(clientfd, (char*)&rs, sizeof(rs));
}

void CLogic::WatchGameRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d WatchGameRq\n", clientfd);
}

void CLogic::RetractChess(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d ChatByVideoRq\n", clientfd);
}

void CLogic::ChatByVideoRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d ChatByVideoRq\n", clientfd);
}

void CLogic::ChatByAudioRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d ChatByAudioRq\n", clientfd);
}

void CLogic::ChatByCharRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d ChatByCharRq\n", clientfd);
}

void CLogic::TransportGameInfoRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d TransportGameInfoRq\n", clientfd);
    STRU_TRANSPORTGAMEINFO_RQ* rq = (STRU_TRANSPORTGAMEINFO_RQ*)szbuf;
    char redisBuf[1024];
    sprintf(redisBuf, "lpush %d %d_%d_%d", rq->info.roomID, rq->info.chessColor, rq->info.x, rq->info.y);
    redisReply *r = (redisReply*)redisCommand(m_redis,redisBuf);
    if(r->type == REDIS_REPLY_STATUS) {
        printf("redis push %s\n", r->str);
        CommandCountInRoom[rq->info.roomID]++;
    }
    RoomInfo* info;
    int sendID;
    m_mapRoomidToRoominfo.find(rq->info.roomID, info);
    if(rq->info.senderID == info->hostID) {
        sendID = info->guestID;
    }
    else {
        sendID = info->hostID;
    }
    SendData(sendID, szbuf, nlen);
}

void CLogic::ConnectAgain(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d ConnectAgain\n", clientfd);
}

void CLogic::OneGameEndRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d OneGameEndRq\n", clientfd);
    STRU_ONEGAMEEND_RQ*rq = (STRU_ONEGAMEEND_RQ*)szbuf;
    int count = CommandCountInRoom[rq->roomID];
    char redisBuf[1024];
    //清空本局指令redis缓存
    sprintf(redisBuf, "ltrim %d %d %d;", rq->roomID, count + 1, count +2);
    redisReply *r = (redisReply*)redisCommand(m_redis,redisBuf);
    if(r->type == REDIS_REPLY_STATUS) {
        printf("redis ltrim %s\n", r->str);
        CommandCountInRoom[rq->roomID] = 0;
    }
    //更新score排名
    player[rq->winnerID].successNum++;
    player[rq->failerID].failNum--;
    int score = 100 * player[rq->winnerID].successNum / player[rq->winnerID].successNum + player[rq->winnerID].failNum;
    sprintf(redisBuf, "zincrby %d %d %d;", player[rq->winnerID].zoneID, score, rq->winnerID);
    r = (redisReply*)redisCommand(m_redis,redisBuf);
    if(r->type == REDIS_REPLY_STATUS) {
        printf("redis zincrby %s\n", r->str);
    }
    score = 100 * player[rq->failerID].successNum / player[rq->failerID].successNum + player[rq->failerID].failNum;
    sprintf(redisBuf, "zadd %d %d %d;", player[rq->failerID].zoneID, score, rq->failerID);
    r = (redisReply*)redisCommand(m_redis,redisBuf);
    if(r->type == REDIS_REPLY_STATUS) {
        printf("redis zadd %s\n", r->str);
    }
    room[rq->roomID].isPlaying = 0;
}

void CLogic::GetTotalRank() {
    deque<char*> dqRank;
    char redisBuf[1024];
    sprintf(redisBuf, "zrange 0 -1;");
    redisReply *r = (redisReply*)redisCommand(m_redis,redisBuf);
    for(int i = 0; i < r->elements; ++i) {
        dqRank.push_front(r->element[i]->str);//之后再正序输出
    }
}

void CLogic::QuitFromRoom(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d QuitFromRoom\n", clientfd);
}
