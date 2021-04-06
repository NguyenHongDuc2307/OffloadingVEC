

#pragma once

#include "veins/veins.h"

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/EdgeComputing/Queue/Task.h"
#include "veins/modules/EdgeComputing/Queue/serverQueue.h"
#include "veins/modules/EdgeComputing/EdgeMessage/TaskResponse_m.h"
#include "veins/modules/EdgeComputing/EdgeMessage/OffloadDecision_m.h"
#include "veins/modules/EdgeComputing/EdgeMessage/TaskInfo_m.h"
#include "veins/modules/EdgeComputing/EdgeMessage/ServerUpdate_m.h"
#include <vector>
#include <map>
#include <string>

using namespace omnetpp;

namespace veins {


class VEINS_API EdgeApplConn : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;
    void finish() override;

protected:
/*Gate ids*/
//  int InterConnPort;
    int ControlEthgPort;
    int DataEthgPort;
/*@{*/
    void onBSM(DemoSafetyMessage* bsm) override;
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;

    void handleSelfMsg(cMessage* msg) override;
    void handleMessage(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;
    

    virtual void fillTaskResponse(TaskResponse* response, Task task);
    virtual void fillOffloadDecision(OffloadDecision* offloadDecisionMsg, Info taskInfo);

    virtual void refreshDisplay() const override;

    virtual void updateEnqueueToSdn(Task task);
    virtual void updateDequeueToSdn();
    virtual void sendCheckChannel(cMessage *msg, const char *gateName);

    cMessage* taskScheduled;

   // cOutVector testVector[10];
   // cOutVector SocketErrorIndex;

   // std::vector<std::pair<int,std::pair<double,int>>> temSpeedData;
   // int RId[8]{0,0,0,0,0,0,0,0};
   // double RSp[8]{0,0,0,0,0,0,0,0};
   // double RPo[8]{0,0,0,0,0,0,0,0};
  //  bool TCPMATLAB;

    Queue taskQueue;
    double computationSpeed = 15;
    double dataRate = 100;
    double unitOperatingCost = 0.3;
    double maxDist;
    Coord curPos;
    bool schedSWPT;
    
    int revTaskNum = 0;
    int compTaskNum = 0;
    int curQueueLength = 0;
    int maxQueueLength = 0;
    int lastQueueLength = 0;
    
    int revInfoNum = 0;
//  int curWaitLength = 0;
    double totalOffloadCost = 0;
    double totalComputingCost = 0;
    int RSU_ID = 0; 

// Calculate the average queue lenght to assess offloading behavior
    double avgQueueLength = 0;
    double count = 0;
  //  SOCKET sockclient;
  //  struct sockaddr_in addr;
  //  bool ServerReady=false;
};

} // namespace veins
