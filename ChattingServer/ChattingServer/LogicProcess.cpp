#include "LogicProcess.h"
#include "SendServer.h"
#include "UserManager.h"

LogicProcess::~LogicProcess()
{
#ifdef _MULTI_THREAD_LOGIC_PROCESS
	CONST_LOCK_GUARD(mutex_);
#endif
	while (!jobQueue_.empty())
	{
		RecvPackage package = jobQueue_.front();
		jobQueue_.pop();
		if (package.packet_)
		{
			delete package.packet_;
		}
	}
}

void LogicProcess::SetMgr(UserManager* uMgr, SendServer* sServer)
{
	uMgr_ = uMgr;
	sendServer_ = sServer;
}

/* * * * * * * * * * * * * * * * * * * * *
*					Run & Close
* * * * * * * * * * * * * * * * * * * * */

void LogicProcess::Run()
{
	for (int i = 0;i < kProcessThreadCnt;i++)
	{
		processThreads_.emplace_back(std::thread(&LogicProcess::RunProcess, this));
	}

	for (int i = 0;i < kNetworkThreadCnt;i++)
	{
		sendThreads_.emplace_back(std::thread(&SendServer::Run, sendServer_));
	}

}

void LogicProcess::Close()
{
	sendServer_->Close();
	for (int i = 0;i < kNetworkThreadCnt;i++)
	{
		sendThreads_[i].join();
	}

	bProcessRun = false;
	for (int i = 0;i < kProcessThreadCnt;i++)
	{
		processThreads_[i].join();
	}
}

/* * * * * * * * * * * * * * * * * * * * *
*				Queue Manage
* * * * * * * * * * * * * * * * * * * * */

void LogicProcess::PushPackage(const RecvPackage& package)
{

#ifdef _MULTI_THREAD_LOGIC_PROCESS
	CONST_LOCK_GUARD(mutex_);
#endif

	jobQueue_.push(package);
}

RecvPackage LogicProcess::PopPackage()
{
#ifdef _MULTI_THREAD_LOGIC_PROCESS
	CONST_LOCK_GUARD(mutex_);
#endif

	if (jobQueue_.empty())
		return RecvPackage();

	RecvPackage package = jobQueue_.front();
	jobQueue_.pop();

	return package;
}

/* * * * * * * * * * * * * * * * * * * * *
*				User Manage
* * * * * * * * * * * * * * * * * * * * */

bool LogicProcess::CreateUser(HANDLE IOCPHandle, SOCKET socket)
{
	return uMgr_->CreateUser(IOCPHandle, socket);
}


void LogicProcess::RemoveUser(uint32_t pId)
{
	uMgr_->RemoveUser(pId);
}


/* * * * * * * * * * * * * * * * * * * * *
*				Logic Functions
* * * * * * * * * * * * * * * * * * * * */


void LogicProcess::C_SERVER_ENTER(const RecvPackage& package)
{
	PK_C_SERVER_ENTER* packet = static_cast<PK_C_SERVER_ENTER*>(package.packet_);
	User* user = uMgr_->GetUser(package.session_->GetId());
	user->SetNickname(packet->nickname);

	uMgr_->SendAcceptPacket(package.session_);
	uMgr_->SendLobbyInfo(package.session_);
	uMgr_->LobbyCast(package.session_, packet->type());
	
}