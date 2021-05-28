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


class UserManager
{

public:
    UserManager(SendServer* sServer)
        : userId_(1), sendServer_(sServer)
    { }

    UserManager() = delete;
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

    Session* GetSession(int id) const;
    User* GetUser(uint32_t id) const;

    bool CreateUser(HANDLE iocpHandle, SOCKET socket);

    inline void ExitLobby(User* user)
    {
        userInLobby_.erase(user);
    }

    inline void EnterLobby(User* user)
    {
        userInLobby_.insert(user);
    }

    void RemoveUser(User* user);
    void RemoveUser(int uid);

    void SendAcceptPacket(Session* session);

    void SendLobbyInfo(Session* session);

    void SendMulticast(Session* session);
    void SendBroadcast(Session* session);
    void SendUnicast(Session* session);

    void LobbyCast(Session* session, PacketType type);
    

protected:
    std::map<uint32_t, User*>   userList_;

    std::vector<std::set<User*>> chatRoom_;
    std::set<User*>             userInLobby_;
    uint32_t                    userId_;

#ifdef _MULTI_THREAD_LOGIC_PROCESS
    std::mutex                  mutex_;
#endif

    SendServer* sendServer_;

};


#endif