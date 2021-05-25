#ifndef SEND_SERVER_H
#define SEND_SERVER_H

#ifdef _MULTI_THREAD_SEND_SERVER
#include <mutex>
#endif

#include <queue>
#include <iostream>
#include "../NetworkLib/Package.h"

class SendServer
{
public:
	SendServer() :
		bRun_(true) {}
	~SendServer() {}

	void Run()
	{
		while (bRun_)
		{
			SendPackage package = PopPackage();

			if (package.Empty())
			{
				Sleep(1);
				continue;
			}

			package.session_->SendPacket(package.packet_.get());
		}
	}

	void Close()
	{
		bRun_ = false;
	}

	SendPackage PopPackage()
	{
#ifdef _MULTI_THREAD_SEND_SERVER
		CONST_LOCK_GUARD(mutex_);
#endif
		if (packages_.empty())
			return SendPackage();

		SendPackage package = packages_.front();
		packages_.pop();

		return package;
	}

	void PushPackage(const SendPackage& package_)
	{
#ifdef _MULTI_THREAD_SEND_SERVER
		CONST_LOCK_GUARD(mutex_);
#endif
		packages_.push(package_);
	}

private:

	std::queue<SendPackage>		packages_;
	bool						bRun_;

private:

#ifdef _MULTI_THREAD_SEND_SERVER
	std::mutex				mutex_;
#endif 

};

#endif //< SEND_SERVER_H