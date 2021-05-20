#include "UserManager.h"
#include "../NetworkLib/Packet.h"

/* * * * * * * * * * * *
*   User Create & Remove
* * * * * * * * * * * * */

bool UserManager::CreateUser(HANDLE iocpHandle, SOCKET socket)
{
    User* newUser = new User(userId_);

    if (false == newUser->Initialize(iocpHandle, socket))
    {
        delete newUser;
        return false;
    }

    // CONST_LOCK_GUARD(mutex_);

    userList_.insert({ userId_++, newUser });
    ++userCount_;

    userInLobby_.insert(newUser);

    return true;
}

void UserManager::RemoveUser(User* user)
{
    user->Release();

    uint32_t uid = user->GetId();

    if (false == user->IsInRoom())
    {
        // EXIT
    }

    printf("[CLOSE] User (%d, %s) ÇØÁ¦\n", user->GetId(), user->GetNickname().c_str());

    delete user;

    // mutex
    userList_.erase(uid);
}

void UserManager::RemoveUser(int index)
{
    // CONST_LOCK_GUARD(mutex_);

    auto it = userList_.find(index);
    if (it == userList_.end())
        return;

    this->RemoveUser(it->second);
}

/* * * * * * * * * * * *
*         Getter
* * * * * * * * * * * * */

Session* UserManager::GetSession(int id) const
{
    //CONST_LOCK_GUARD(mutex_)

    auto user = userList_.find(id);
    if (user == userList_.end())
        return nullptr;

    return user->second->GetSession();

}

User* UserManager::GetUser(uint32_t id) const
{
    //CONST_LOCK_GUARD(mutex_)

    auto user = userList_.find(id);
    if (user == userList_.end())
        return nullptr;
    return user->second;
}

/* * * * * * * * * * * *
*     Packet Process
*   -> Accept & Lobby
* * * * * * * * * * * * */

void UserManager::SendAcceptPacket(Session* session)
{
    std::shared_ptr<PK_S_SERVER_ENTER_OK> packet = std::make_shared<PK_S_SERVER_ENTER_OK>(session->GetPid());
    SendPackage package(session, packet);
    sendServer_->PushPackage(package);
}

void UserManager::SendLobbyInfo(Session* session) 
                                                            //< Current Connected User infos & Users in lobby
{

    std::shared_ptr<PK_S_LOBBY_INFO> packet = std::make_shared<PK_S_LOBBY_INFO>();
    //std::shared_ptr<PK_S_LOBBY_INFO> packet = std::make_shared<PK_S_LOBBY_INFO>();
    //for (auto& user : userList_)
    //{
    //    if (user.second->IsInLobby())
    //        packet->lobbyInfo.push_back(
    //            UserPacketInfo(user.second->GetId(), user.second->GetPosition()));
    //}

    //SendPackage package(session, packet);
    //networkServer_->PushPackage(package);

}

void UserManager::LobbyCast(Session* session) 
                                                            //< In-out Info of other users
{
   /* std::shared_ptr<PK_S_PLAYER_POSITION> packet = std::make_shared<PK_S_PLAYER_POSITION>(user->GetId(), user->GetPosition());
    SendPackage package(nullptr, packet);

    for (auto& user : userInLobby_)
    {
        package.session_ = user->GetSession();
        networkServer_->PushPackage(package);
    }*/
    
}


/* * * * * * * * * * * * * * *
*     Packet Process
*   -> Chatting Room Packet
* * * * * * * * * * * * * * * */


void UserManager::SendMulticast(Session* session)
{

}

void UserManager::SendBroadcast(Session* session)
{

}

void UserManager::SendUnicast(Session* session)
{

}