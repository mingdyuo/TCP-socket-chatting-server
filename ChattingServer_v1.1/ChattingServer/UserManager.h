#pragma once
#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <map>
#include <set>

#ifdef _MULTI_THREAD_LOGIC_PROCESS
#include <mutex>
#endif

#include "ChatUser.h"
#include "SendServer.h"

class RoomManager;

class UserManager
{

public:
    UserManager()
        : userId_(1), rMgr_(nullptr), sendServer_(nullptr)
    { }

    UserManager(UserManager const&) = delete;
    UserManager(UserManager const&&) = delete;

    ~UserManager()
    {
        if (userList_.empty())
            return;

        for (auto& user : userList_)
        {
            if (user.second == nullptr)
                continue;

            user.second->Release();
            delete user.second;
        }
    }

    void SetMgr(RoomManager* rMgr, SendServer* sServer)
    {
        rMgr_ = rMgr;
        sendServer_ = sServer;
    }

    Session* GetSession(int id) const;
    User* GetUser(uint32_t id) const;

    bool CreateUser(HANDLE iocpHandle, SOCKET socket);

    void RemoveUser(User* user);
    void RemoveUser(int uid);

    void SendAcceptPacket(Session* session);

    

protected:
    std::map<uint32_t, User*>   userList_;

    
    uint32_t                    userId_;

#ifdef _MULTI_THREAD_LOGIC_PROCESS
    std::mutex                  mutex_;
#endif

    SendServer* sendServer_;
    RoomManager* rMgr_;

};


#endif