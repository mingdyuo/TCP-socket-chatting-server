#include "UserManager.h"
#include "RoomManager.h"
#include "Room.h"
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

    rMgr_->PushIntoLobby(newUser);
    // userInLobby_.insert(newUser);

    return true;
}

void UserManager::RemoveUser(User* user)
{
    uint32_t uid = user->GetId();

    if (user->IsInLobby())
    {
        rMgr_->RemoveFromLobby(user);
    }
    else
    {
        user->GetRoom()->ExitRoom(user, true);
    }

    user->Release();
    printf("[CLOSE] User (%lu, %s) ÇØÁ¦\n", uid, user->GetNickname().c_str());

    userList_.erase(uid);
    
    if (user != nullptr)
    {
        delete user;
        user = nullptr;
    }

    // mutex
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

User* UserManager::FindByNickname(const std::string& name)
{
    for(auto& user : userList_)
    { 
        if (name == user.second->GetNickname())
            return user.second;
    }
    return nullptr;
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
