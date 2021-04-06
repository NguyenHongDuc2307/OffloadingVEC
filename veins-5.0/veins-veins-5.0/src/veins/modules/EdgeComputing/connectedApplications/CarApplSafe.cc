

#include "veins/modules/EdgeComputing/connectedApplications/CarApplSafe.h"
#include "veins/modules/EdgeComputing/Information/CarInfo.h"
#include "veins/modules/EdgeComputing/EdgeMessage/CarInfoWSM_m.h"
#include "veins/modules/EdgeComputing/EdgeMessage/CarControl_m.h"
#include "veins/modules/EdgeComputing/EdgeMessage/TaskRequest_m.h"
#include "veins/modules/EdgeComputing/EdgeMessage/TaskResponse_m.h"
#include "veins/modules/EdgeComputing/EdgeMessage/TaskInfo_m.h"
#include "veins/modules/EdgeComputing/EdgeMessage/OffloadDecision_m.h"
#include <omnetpp.h>
#include <cmath>
#include <math.h>



#include <string>

using namespace veins;

Define_Module(veins::CarApplSafe);


#include <Winsock2.h>
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")

#define LOCAL_COM_SPEED 1
#define V2I_DATARATE 100
#define TASK_INTERVAL 20

void CarApplSafe::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {

    //    UDPMATLAB=par("UDPMATLAB");
    //    senseState=new cMessage("sense state");
    //    senseStateInterval=par("senseStateInterval");
        taskInfoInterval = TASK_INTERVAL;
    //    WATCH(totalOffloadCost);
    //    WATCH(localTaskNum);
        CAR_ID = getParentModule()->getIndex();
    //    scheduleAt(simTime() + senseStateInterval, senseState);
    //    for(int i=0;i<10;i++){
    //        testVector[i].setName(("test vector"+std::to_string(i)).c_str());
    //    }
    //    vehicleSpeeds.setName("vehicle Speeds");
    //    lanePositions.setName("lane Positions");

        taskGen = new cMessage("task generation");
        scheduleAt(simTime() + taskInfoInterval*uniform(0,1), taskGen);
    }
    else if (stage == 1) {

    }
    compTaskNum = 0;
    sentTaskNum = 0;
}

void CarApplSafe::finish()
{
    DemoBaseApplLayer::finish();
//    recordScalar("compTaskNum", compTaskNum);
//    recordScalar("sentTaskNum", sentTaskNum);
//    cancelAndDelete(senseState);
    recordScalar("localTaskNum",localTaskNum);
    recordScalar("totalLocalCost",totalLocalCost);
    cancelAndDelete(taskGen);

}

void CarApplSafe::onBSM(DemoSafetyMessage* bsm)
{

}

void CarApplSafe::onWSM(BaseFrame1609_4* wsm)
{
    int offloadServer = -1;
    double delay = 0;
    if(OffloadDecision* offDecision =dynamic_cast<OffloadDecision*>(wsm)){
        if(offDecision->getTaskOwnerId()==CAR_ID && offDecision->getTaskSendId()==curTaskInfo.getTaskSendId())
        {
            EV << "Car " << CAR_ID << " receive one offload decision for task number " << offDecision->getTaskSendId() << endl;
            offloadServer = offDecision->getServerId();
            if (offloadServer == -1)
            {
                EV << "One task is processed locally";
                localTaskNum++;
                totalLocalCost = totalLocalCost + (curTaskInfo.getUnitDelayCost())*((curTaskInfo.getComputationLoad())/LOCAL_COM_SPEED);
            }
            else if (offloadServer >= 0)
            {
                TaskRequest* request = new TaskRequest();
                fillTaskRequest(request, offloadServer);
                EV << "Car " << CAR_ID << " send one request for task number " << request->getTaskSendId() << endl;
                populateWSM(request);
                populateRequest(request,curTaskInfo.getInputSize());
//                sendDown(request);
                delay = request->getInputSize()/V2I_DATARATE;
                sendDelayedDown(request,delay);        
                
            }
        }
    }

    else if(TaskResponse* taskResponse =dynamic_cast<TaskResponse*>(wsm)){
        simtime_t taskCompTime = simTime();
        if(taskResponse->getTaskOwnerId()==CAR_ID)
        {
            EV << "Car " << CAR_ID << " receive one response for task number " << taskResponse->getTaskSendId() << endl;
            if(taskResponse->getDeadline() >= taskCompTime)
            {
                compTaskNum++;
            }
        }
    }
}

void CarApplSafe::onWSA(DemoServiceAdvertisment* wsa)
{

}

void CarApplSafe::handleSelfMsg(cMessage* msg)
{   
    
   
    if (msg == taskGen)
    {
        TaskInfo* taskInfo = new TaskInfo();
        fillTaskInfo(taskInfo);
        curTaskInfo = *taskInfo;
        EV << "Car " << CAR_ID << " send one TaskInfo for task number " << taskInfo->getTaskSendId() << endl;
        populateWSM(taskInfo);
        sendDown(taskInfo);
/*
        TaskRequest* request = new TaskRequest();
        fillTaskRequest(request);
        populateWSM(request);
        sendDown(request);
*/
        sentTaskNum++;
      //  EV << "Current Position is " << mobility->getPositionAt(simTime()) << endl;
        scheduleAt(simTime() + taskInfoInterval, taskGen);    
    }

}

void CarApplSafe::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);

}

void CarApplSafe::populateRequest(TaskRequest* wsm, int dataLength)
{    
    double requestDataLengthBits = dataLength; 
    wsm->addBitLength(requestDataLengthBits);
}

/*
void CarApplSafe::populateWSM(BaseFrame1609_4* wsm, LAddress::L2Type rcvId, int serial)
{

}
*/
void CarApplSafe::fillTaskRequest(TaskRequest* request, int serverId)
{
//    simtime_t curTaskDeadline = simTime() + taskTimeConstraint;
    simtime_t curTaskGenTime = simTime();
//    double curTaskComputationLoad = intuniform(1, 5);
    request->setDesId(serverId);
    request->setTaskOwnerId(CAR_ID);
    request->setTaskSendId(curTaskInfo.getTaskSendId());
//  request->setDataByteLength(taskDataLength);
    request->setDeadline(curTaskInfo.getDeadline());
    request->setGenerateTime(curTaskGenTime);
    request->setComputationLoad(curTaskInfo.getComputationLoad());
    request->setCurPosition(mobility->getPositionAt(simTime()));
    request->setUnitDelayCost(curTaskInfo.getUnitDelayCost());
    request->setInputSize(curTaskInfo.getInputSize());
}

void CarApplSafe::fillTaskInfo(TaskInfo* taskInfo)
{
    curTaskId++;
    simtime_t curTaskDeadline = simTime() + taskTimeConstraint;
    double curTaskComputationLoad = intuniform(3, 7);
//    double inputSize = intuniform(1, 3);
    double inputSize = 2;
    double unitDelayCost = intuniform(1,3);

    taskInfo->setTaskOwnerId(CAR_ID);
    taskInfo->setTaskSendId(curTaskId);
//  request->setDataByteLength(taskDataLength);
    taskInfo->setDeadline(curTaskDeadline);
    taskInfo->setComputationLoad(curTaskComputationLoad);
    taskInfo->setInputSize(inputSize);
    taskInfo->setUnitDelayCost(unitDelayCost);
}

void CarApplSafe::handleLowerMsg(cMessage* msg){
//    EV << "Car receive one message";
    DemoBaseApplLayer::handleLowerMsg(msg);
    
}
