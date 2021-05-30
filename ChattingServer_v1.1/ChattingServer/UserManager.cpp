#include "UserManager.h"
#include "RoomManager.h"
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

    rMgr_->InputNewUser(newUser);
    // userInLobby_.insert(newUser);

    return true;
}

void UserManager::RemoveUser(User* user)
{
    user->Release();

    uint32_t uid = user->GetId();

    if (user->IsInLobby())
    {
        //< 현재 있는 채팅방에 따라서 암튼 삭제  && 캐스트
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
