#pragma once

#ifndef LOGIC_PROCESS_H
#define LOGIC_PROCESS_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * Edit here if multi threads are needed for logic process
 
#define _MULTI_THREAD_LOGIC_PROCESS
#define LOGIC_THREAD_COUNT 3

#define _MULTI_THREAD_SEND_SERVER
#define SEND_SERVER_THREAD_COUNT 3

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <map>
#include <thread>
#include <queue>


#ifdef _MULTI_THREAD_LOGIC_PROCESS
#include <mutex>
#endif // _MULTI_THREAD_LOGIC_PROCESS


#if (defined _MULTI_THREAD_LOGIC_PROCESS || defined _MULTI_THREAD_SEND_SERVER)
#define CONST_LOCK_GUARD(m) const std::lock_guard<std::mutex>(m)
#endif

#include "Package.h"
class SendServer;
class UserManager;
class RoomManager;


class LogicProcess
{
	typedef void (LogicProcess::* PROCESS_FUNCTION)(const RecvPackage& package);
	typedef std::map<packet_header_size, PROCESS_FUNCTION> PROCESS_FUNC_MAP;

public:

	LogicProcess() :
		uMgr_(nullptr), sendServer_(nullptr), rMgr_(nullptr), bProcessRun(true)
	{
		processFunc_[E_PK_C_SERVER_ENTER] = &LogicProcess::C_SERVER_ENTER;
		processFunc_[E_PK_C_SERVER_EXIT] = &LogicProcess::C_SERVER_EXIT;
		processFunc_[E_PK_C_LOBBY_ROOM_INFO] = &LogicProcess::C_LOBBY_ROOM_INFO;

		processFunc_[E_PK_C_ROOM_CREATE] = &LogicProcess::C_ROOM_CREATE;
		processFunc_[E_PK_C_ROOM_ENTER] = &LogicProcess::C_ROOM_ENTER;
		processFunc_[E_PK_C_ROOM_EXIT] = &LogicProcess::C_ROOM_EXIT;


		processFunc_[E_PK_C_UNICAST] = &LogicProcess::C_UNICAST;
		processFunc_[E_PK_C_MULTICAST] = &LogicProcess::C_MULTICAST;
		processFunc_[E_PK_C_BROADCAST] = &LogicProcess::C_BROADCAST;

	}

	~LogicProcess();

	void SetMgr(RoomManager* rMgr, UserManager* uMgr, SendServer* sServer);

	void	Run(); //< Process Thread 생성
	void	Close();
	
	void		PushPackage(const RecvPackage& package);
	RecvPackage PopPackage();

	bool CreateUser(HANDLE IOCPHandle, SOCKET socket);

	void RemoveUser(uint32_t pId);



protected:
	void RunProcess()
	{
		while (bProcessRun)
		{
			RecvPackage package = this->PopPackage();

			if (package.Empty()) //< 나중에 queue에 비동기를 걸어놓고 비어있는 경우 재우자
			{
				Sleep(1);
				continue;
			}
			this->Process(package);
		}
	}

	void Process(const RecvPackage& package)
	{
		PROCESS_FUNCTION fp = processFunc_[(packet_header_size)package.packet_->type()];
		if (fp != nullptr) (this->*fp)(package);
		delete package.packet_; // 체크
	}

	void C_SERVER_ENTER(const RecvPackage& package);
	void C_SERVER_EXIT(const RecvPackage& package);

	void C_LOBBY_ROOM_INFO(const RecvPackage& package);

	void C_ROOM_CREATE(const RecvPackage& package);
	void C_ROOM_ENTER(const RecvPackage& package);
	void C_ROOM_EXIT(const RecvPackage& package);

	void C_UNICAST(const RecvPackage& package);
	void C_MULTICAST(const RecvPackage& package);
	void C_BROADCAST(const RecvPackage& package);

private:

	PROCESS_FUNC_MAP			processFunc_;
	std::queue<RecvPackage>		jobQueue_;
	bool						bProcessRun;


private:
	SendServer*					sendServer_;
	UserManager*				uMgr_;
	RoomManager*				rMgr_;

private:

#ifdef _MULTI_THREAD_SEND_SERVER
	const static int			kNetworkThreadCnt = SEND_SERVER_THREAD_COUNT;
#else
	const static int			kNetworkThreadCnt = 1;
#endif // _MULTI_THREAD_SEND_SERVER

private:
#ifdef _MULTI_THREAD_LOGIC_PROCESS
	const static int			kProcessThreadCnt = LOGIC_THREAD_COUNT;
	std::mutex					mutex_;
#else
	const static int			kProcessThreadCnt = 1;
#endif // _MULTI_THREAD_LOGIC_PROCESS

	std::vector<std::thread>	sendThreads_;
	std::vector<std::thread>	processThreads_;



};

#endif // !LOGIC_PROCESS_H



