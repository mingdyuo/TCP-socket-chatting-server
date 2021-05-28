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

    printf("[CLOSE] User (%d, %s) 해제\n", user->GetId(), user->GetNickname().c_str());

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
    std::shared_ptr<PK_S_SERVER_ENTER_OK> packet = std::make_shared<PK_S_SERVER_ENTER_OK>(session->GetId());
    SendPackage package(session, packet);
    sendServer_->PushPackage(package);
}

void UserManager::SendLobbyInfo(Session* session) 
                                                            //< Current Connected User infos in lobby & ChatRooms
{
    std::shared_ptr<PK_S_LOBBY_ROOM_INFO> roomPacket = std::make_shared<PK_S_LOBBY_ROOM_INFO>();
    // TODO : 채팅방 정보 보내기


    std::shared_ptr<PK_S_LOBBY_USER_INFO> userPacket = std::make_shared<PK_S_LOBBY_USER_INFO>(userList_.size());
    for (auto& user : userList_)
    {
        userPacket->ids.push_back(user.second->GetId());
        userPacket->names.push_back(user.second->GetNickname());
    }

    SendPackage package(session, userPacket);
    sendServer_->PushPackage(package);


}

void UserManager::LobbyCast(Session* session, PacketType type) 
                                                            //< In-out Info of other users
{
    User* user = this->GetUser(session->GetId());
    if (type == E_PK_S_LOBBY_ENTER)
    {
        std::shared_ptr<PK_S_LOBBY_ENTER> packet = std::make_shared<PK_S_LOBBY_ENTER>(user->GetId(), user->GetNickname());
        SendPackage package(nullptr, packet);
        for (auto& user : userList_)
        {
            package.session_ = user.second->GetSession();
            sendServer_->PushPackage(package);
        }
    }
    else if (type == E_PK_S_LOBBY_EXIT)
    {

    }
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