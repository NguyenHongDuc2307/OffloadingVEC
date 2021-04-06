
#pragma once

#include "veins/veins.h"

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/EdgeComputing/EdgeMessage/TaskRequest_m.h"
#include "veins/modules/EdgeComputing/EdgeMessage/TaskInfo_m.h"
#include "veins/modules/messages/BaseFrame1609_4_m.h"

using namespace omnetpp;

namespace veins {


class VEINS_API CarApplSafe : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;
    void finish() override;

protected:
    void onBSM(DemoSafetyMessage* bsm) override;
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;

    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;
    void handleLowerMsg(cMessage* msg) override;

    virtual void populateRequest(TaskRequest* wsm, int dataLength);
//    void populateWSM(BaseFrame1609_4* wsm, LAddress::L2Type rcvId = LAddress::L2BROADCAST(), int serial = 0) override;

    virtual void fillTaskRequest(TaskRequest* request, int serverId);
    virtual void fillTaskInfo(TaskInfo* TaskInfo);

//    cOutVector testVector[10];

/********STATES***************************************************/
//    cOutVector vehicleSpeeds;
//    cOutVector lanePositions;


    double currentSpeed;
    double currentLanePosition;
    int curTaskId = 0;
    int CAR_ID = 0;
    simtime_t taskTimeConstraint = 10;
    double totalLocalCost = 0;
    int localTaskNum = 0;

    simtime_t senseStateInterval;
    simtime_t taskInfoInterval;

//    cMessage* senseState;
    cMessage* taskGen;

//    SOCKET sockclient;
//    struct sockaddr_in addr;


    bool passCrossing=false;
    bool receiveControl=false;
//    bool UDPMATLAB;
//    double controlSpeed;
    int compTaskNum;
    int sentTaskNum = 0;
    TaskInfo curTaskInfo;




};

} // namespace veins
