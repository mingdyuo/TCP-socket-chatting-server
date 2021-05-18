#include "LogicProcess.h"


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

