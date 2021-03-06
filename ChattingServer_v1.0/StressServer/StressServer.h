#pragma once
#include "ClientInfo.h"
#include "IOCPServer.h"
#include "CsvParser.h"

#include <process.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <memory>


class StressServer : public IOCPServer
{
public:
    StressServer() : mClientCount(0), mEndline(1) {
        if (!parseCsv(this->mNicknames, "nickname_data.csv")) 
        {
            printf("[에러] 랜덤 닉네임 불러오기 실패\n");
        }
        if (!parseCsv(this->mContents, "random_content.csv")) 
        {
            printf("[에러] 랜덤 전송 내용 불러오기 실패\n");
        }
    }

    virtual ~StressServer(void)
    {
        for (int i = 0;i < mClients.size();i++) 
        {
            if (mClients[i] && mClients[i]->IsConnected())
            {
                mClients[i]->Close();
            }
        }
        WSACleanup();
    }

    virtual unsigned ConnecterThread();     //< Create clients
    virtual unsigned WorkerThread();        //< GQCS 
    virtual unsigned SenderThread();        //< Send random packet

    bool CreateClient(const std::vector<std::string>& names);


private:
    const int SERVER_PORT               = 9898;
    const int MAX_CLIENT                = 10000;
    const int SLEEP_INTERVAL_CREATE     = 5;
    const int SLEEP_INTERVAL_SEND       = 25;

    std::vector<std::unique_ptr<stClientInfo>> mClients;

    std::vector<std::string>    mNicknames;
    std::vector<std::string>    mContents;

    int                         mClientCount;
    long long int               mEndline;


};


