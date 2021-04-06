
#include "veins/modules/EdgeComputing/connectedApplications/EdgeApplConn.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "veins/modules/EdgeComputing/Information/CarInfo.h"
#include "veins/modules/EdgeComputing/EdgeMessage/CarInfoWSM_m.h"
#include "veins/modules/EdgeComputing/EdgeMessage/CarControl_m.h"
#include "veins/modules/EdgeComputing/EdgeMessage/TaskRequest_m.h"
#include "veins/modules/EdgeComputing/EdgeMessage/TaskResponse_m.h"
#include "veins/modules/EdgeComputing/EdgeMessage/TaskInfo_m.h"
#include "veins/modules/EdgeComputing/EdgeMessage/OffloadDecision_m.h"
#include "veins/modules/EdgeComputing/EdgeMessage/ServerUpdate_m.h"
#include "veins/modules/EdgeComputing/Queue/Task.h"
#include "veins/modules/EdgeComputing/Queue/serverQueue.h"
#include <omnetpp.h>

#include <math.h>

#include <fstream>
#include <iostream>

#include <string>

using namespace veins;



Define_Module(veins::EdgeApplConn);


#include<winsock.h>
#pragma comment(lib,"ws2_32.lib")

void EdgeApplConn::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
//        TCPMATLAB=par("TCPMATLAB");
        taskScheduled = new cMessage("new task is done");
        maxDist = 0;
      
        curPos.x = par("x");
        curPos.y = par("y");
        curPos.z = par("z");
        schedSWPT = par("schedSWPT");
        WATCH(revTaskNum);
        WATCH(compTaskNum);
        WATCH(curQueueLength);
        WATCH(totalOffloadCost);
        ControlEthgPort = findGate("ControlEthgPort$i");
        DataEthgPort = findGate("DataEthgPort$i");
        RSU_ID = getParentModule()->getIndex();
    //    curPos = mobility->getPositionAt(simTime());
    //    EV << "InterConnPort = "<< InterConnPort;

//        for(int i=0;i<10;i++){
//            testVector[i].setName(("test vector"+std::to_string(i)).c_str());
//        }
//        SocketErrorIndex.setName("Socket Error");


  //      broadcast=new cMessage("broadcast");
  //      scheduleAt(simTime() + 0.01, broadcast);




    }
    else if (stage == 1) {

    }
}

void EdgeApplConn::finish()
{

    lastQueueLength = curQueueLength;
    DemoBaseApplLayer::finish();
    recordScalar("revTaskNum",revTaskNum);
    recordScalar("compTaskNum",compTaskNum);
    recordScalar("maxQueueLength",maxQueueLength);
    recordScalar("totalOffloadCost",totalOffloadCost);
    recordScalar("totalComputingCost",totalComputingCost);
    recordScalar("revInfoNum",revInfoNum);
    recordScalar("avgQueueLength",avgQueueLength);
    recordScalar("lastQueueLength",lastQueueLength);
    
    cancelAndDelete(taskScheduled);
 /*
    CHAR SOver[15]="SimulationOVER";
    if(TCPMATLAB){
        ::send(sockclient,SOver,15,0);//End Matlab Server
        closesocket(sockclient);
    }
*/


}

void EdgeApplConn::onBSM(DemoSafetyMessage* bsm)
{

}

void EdgeApplConn::handleMessage(cMessage* msg)
{
    if (msg->isSelfMessage()) {
        handleSelfMsg(msg);
    }
    else if (msg->getArrivalGateId() == upperLayerIn) {
        recordPacket(PassedMessage::INCOMING, PassedMessage::UPPER_DATA, msg);
        handleUpperMsg(msg);
    }
    else if (msg->getArrivalGateId() == upperControlIn) {
        recordPacket(PassedMessage::INCOMING, PassedMessage::UPPER_CONTROL, msg);
        handleUpperControl(msg);
    }
    else if (msg->getArrivalGateId() == lowerControlIn) {
        recordPacket(PassedMessage::INCOMING, PassedMessage::LOWER_CONTROL, msg);
        handleLowerControl(msg);
    }
    else if (msg->getArrivalGateId() == lowerLayerIn) {
        recordPacket(PassedMessage::INCOMING, PassedMessage::LOWER_DATA, msg);
        handleLowerMsg(msg);
    }
//  Receiving a message from InterConneted port
    else if ((msg->getArrivalGateId() == ControlEthgPort) || (msg->getArrivalGateId() == DataEthgPort)){
//        EV << "A new task is received from the other RSU";
        handleLowerMsg(msg);
    }

    else if (msg->getArrivalGateId() == -1) {
        /* Classes extending this class may not use all the gates, f.e.
         * BaseApplLayer has no upper gates. In this case all upper gate-
         * handles are initialized to -1. When getArrivalGateId() equals -1,
         * it would be wrong to forward the message to one of these gates,
         * as they actually don't exist, so raise an error instead.
         */
        throw cRuntimeError("No self message and no gateID?? Check configuration.");
    }

    else {
        /* msg->getArrivalGateId() should be valid, but it isn't recognized
         * here. This could signal the case that this class is extended
         * with extra gates, but handleMessage() isn't overridden to
         * check for the new gate(s).
         */
        throw cRuntimeError("Unknown gateID?? Check configuration or override handleMessage().");
    }
}

void EdgeApplConn::onWSM(BaseFrame1609_4* wsm)
{
/*
    if (TaskInfo* taskInfoMsg = dynamic_cast<TaskInfo*>(wsm)){
        EV << "Edge server " << myId << " receive one TaskInfo " << taskInfoMsg->getTaskSendId() << endl;
        Info taskInfo;
        taskInfo.taskSendId = taskInfoMsg->getTaskSendId();
        taskInfo.taskOwnerId = taskInfoMsg->getTaskOwnerId();

        OffloadDecision* offDecisionMsg = new OffloadDecision();

        fillOffloadDecision(offDecisionMsg,taskInfo);
        populateWSM(offDecisionMsg);
        sendDown(offDecisionMsg);
    }
*/
    if (TaskInfo* taskInfoMsg = dynamic_cast<TaskInfo*>(wsm)){
        
// Send report message to another Server
        revInfoNum++;
        TaskInfo* newTaskInfoMsg = new TaskInfo();
        *newTaskInfoMsg = *taskInfoMsg;
        newTaskInfoMsg->setForwardServerId(RSU_ID);
        EV << "Edge server " << RSU_ID << " receive one TaskInfo " << taskInfoMsg->getTaskSendId() << endl;
        sendCheckChannel(newTaskInfoMsg, "ControlEthgPort$o");  
//        EV << "Edge server " << RSU_ID << " receive one TaskInfo " << taskInfoMsg->getTaskSendId() << endl;
    }

    else if (OffloadDecision* offloadDecisionMsg = dynamic_cast<OffloadDecision*>(wsm)){
    //    offloadDecisionMsg->setServerId(RSU_ID);
        OffloadDecision* newOffDecisionMsg = new OffloadDecision();
        *newOffDecisionMsg = *offloadDecisionMsg;
        populateWSM(newOffDecisionMsg);
        sendDown(newOffDecisionMsg);
    }

    else if(TaskRequest* request = dynamic_cast<TaskRequest*>(wsm)){
        if (request->getDesId() == RSU_ID)
        {
//            EV << "Edge server " << myId << " receive one Request " << request->getTaskSendId() << endl;
// Send report message to another Server
//            cMessage *msg = new cMessage("reportMsg");
//            send(msg, "InterConnPort$o");
// Finish sending
            revTaskNum++;
            double Dist;
           // curPos = mobility->getPositionAt(simTime());
            Task task;
   //         sprintf(task.taskId,"%d_%d",request->getTaskOwnerId(),request->getTaskSendId());
            task.taskId = std::to_string(request->getTaskOwnerId())+"_"+ std::to_string(request->getTaskSendId());
            task.computationLoad = request->getComputationLoad();
            task.taskOwnerId = request->getTaskOwnerId();
            task.taskSendId = request->getTaskSendId();
            task.deadline = request->getDeadline();
            task.generateTime = request->getGenerateTime();
            task.senderPos = request->getCurPosition();
            task.timeToServer = simTime();
            task.unitDelayCost = request->getUnitDelayCost();
            task.inputSize = request->getInputSize();
            task.desId = request->getDesId();
            task.WPT = (task.computationLoad)/(computationSpeed*task.unitDelayCost);
            
      //      EV << "Edge server position is " << mobility->getPositionAt(simTime()) << endl;
            Dist = curPos.distance(task.senderPos);
            if (Dist > maxDist){
                maxDist = Dist;
            }
//            EV << "A new request received from = " << task.senderPos << endl;
     //       EV << " Queue Rear before enqueue is " << taskQueue.getRear() << endl;
            
     //       taskQueue.enqueue(task);
     //       taskQueue.enqueue_SWPT(task);
            if (schedSWPT)
            {
                taskQueue.enqueue_SWPT(task);
            }
            else
            {
                taskQueue.enqueue(task);
            }
            updateEnqueueToSdn(task);
            curQueueLength = taskQueue.getRear()+1;

// Result collection
            count++;
            avgQueueLength = (avgQueueLength*(count-1)+curQueueLength)/count;
// End result collection

            if (curQueueLength>maxQueueLength)
            {
                maxQueueLength = curQueueLength;
            }
     //       EV << " Queue Rear after enqueue is " << taskQueue.getRear() << endl;
            if(taskQueue.getRear() == 0){
                EV << "A new task is queued up first " << taskQueue.getFront().taskId;
                scheduleAt(simTime() + task.computationLoad/computationSpeed, taskScheduled);
            }
        } 

        else 
        {
            TaskRequest* newRequest = new TaskRequest();
            *newRequest = *request;
            newRequest->setForwardServerId(RSU_ID);
//            EV << "Edge server " << RSU_ID << " receive one TaskInfo " << taskInfoMsg->getTaskSendId() << endl;
            send(newRequest, "DataEthgPort$o");  
        }
    }
    
    
    /*
    CarInfoWSM* cwsm=dynamic_cast<CarInfoWSM*>(wsm);
    CarInfo carInfo=cwsm->getCarInfo();
    double speed=carInfo.getVehicleSpeed();

    double lanePosition=carInfo.getLanePosition();


    std::string roadId=carInfo.getRoadId();
    int roadIndex=carInfo.getRoadIndex();

    testVector[0].record(roadIndex);
    int Vid=carInfo.getCarId();
//    temSpeedData.push_back(std::pair<int,std::pair<double,int>>(Vid,std::pair<double,int>(speed,roadIndex)));
//    testVector[1].record(temSpeedData.size());
    if(RPo[roadIndex]<lanePosition){
        RId[roadIndex]=Vid;
        RSp[roadIndex]=speed;
    }
*/
}

void EdgeApplConn::onWSA(DemoServiceAdvertisment* wsa)
{

}

//bool cmp(std::pair<int,std::pair<double,int>> a, std::pair<int,std::pair<double,int>> b) {
//    return a.second.first > b.second.first;
//}



void EdgeApplConn::handleSelfMsg(cMessage* msg)
{


    if(msg==taskScheduled){
        compTaskNum++;
        Task task = taskQueue.dequeue();
        updateDequeueToSdn();
        curQueueLength = taskQueue.getRear()+1;

// Result collection
        count++;
        avgQueueLength = (avgQueueLength*(count-1)+curQueueLength)/count;
// End result collection
/*
        TaskResponse* response = new TaskResponse();
        fillTaskResponse(response,task);
        populateWSM(response);
        sendDown(response);
*/
/*  Start result collection   */
//        totalOffloadCost = totalOffloadCost + ((task.inputSize/dataRate)+simTime().dbl()-task.generateTime.dbl())*task.unitDelayCost+task.computationLoad*unitOperatingCost;
        totalOffloadCost = totalOffloadCost + (simTime().dbl()-task.generateTime.dbl())*task.unitDelayCost+task.computationLoad*unitOperatingCost;

/* Test delay*/
        double offloadcost = (simTime().dbl()-task.generateTime.dbl())*task.unitDelayCost+task.computationLoad*unitOperatingCost;
        EV << " Task number " << task.taskOwnerId << "_" << task.taskSendId << "\n";
        EV << "true offload cost = " << offloadcost << "\n";
/* End Test delay*/

        totalComputingCost = totalComputingCost + task.computationLoad*unitOperatingCost;
/*  End result collection   */
//        totalOffloadCost = totalOffloadCost + (task.inputSize/6)*task.unitDelayCost;
//        EV << " Queue Rear 2 is " << taskQueue.getRear();
        if(taskQueue.getRear() != -1)
        {
//            EV << "A new task is queued up second " << taskQueue.getFront().taskId;
            scheduleAt(simTime() + taskQueue.getFront().computationLoad/computationSpeed, taskScheduled);
        }

        /*
        if(TCPMATLAB){
            if(!ServerReady){
                       sockclient=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
                       if(INVALID_SOCKET == sockclient)
                       {
                           SocketErrorIndex.record(1);
                       }


                       addr.sin_family = AF_INET;
                       addr.sin_port = htons(6969);
                       addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
                       int iRetVal = connect(sockclient,(struct sockaddr*)&addr,sizeof(addr));
                       if(SOCKET_ERROR == iRetVal)
                       {
                           SocketErrorIndex.record(1);
                           closesocket(sockclient);
                       }
                       ServerReady=true;
                    }



            //        sort(temSpeedData.begin(),temSpeedData.end(),cmp);
                    std::string sendData="";
                    int sizeSend=0;
                    for(int ni=0;ni<8;ni++){
                            sendData.append(std::to_string(RSp[ni]));
                            sizeSend+=(std::to_string(RSp[ni]).length());

            //            else{
            //                sendData.append("0");
            //                sizeSend+=1;
            //            }

                        if(ni!=7){
                        sendData.append(",");
                        sizeSend+=1;
                        }
                    }

                    ::send(sockclient,sendData.c_str(),sizeSend,0);

                    CHAR szRecv[100] = {0};
                    recv(sockclient,szRecv,100,0);

//                    std::ofstream OsWrite("ot.txt",std::ofstream::app);
//                    OsWrite<<szRecv;
//                    OsWrite<<std::endl;
//                    OsWrite.close();


                    std::stringstream ss;
                    ss<<szRecv;
                    double s[8];
                    ss>>s[0]>>s[1]>>s[2]>>s[3]>>s[4]>>s[5]>>s[6]>>s[7];
                    CarControl* cctrl=new CarControl();
                    populateWSM(cctrl);
                    for(int ni=0;ni<8;ni++){
                        if(RId[ni]!=0){
                            cctrl->setVehicleId(ni,RId[ni]);
                            cctrl->setSpeed(ni,s[ni]);
                        }else{
                            cctrl->setVehicleId(ni,0);
                            cctrl->setSpeed(ni,0);
                        }
                    }

                    sendDelayedDown(cctrl,1);
                    for (int i=0;i<8;i++){
                        RId[i]=0;
                        RSp[i]=0;
                        RPo[i]=0;
                    }

        }*/





    }


}

void EdgeApplConn::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);

}

void EdgeApplConn::fillTaskResponse(TaskResponse* response, Task task)
{
    response->setTaskOwnerId(task.taskOwnerId);
    response->setTaskSendId(task.taskSendId);
    response->setServerId(RSU_ID);
    response->setDeadline(task.deadline);
    response->setComputationLoad(task.computationLoad);
    response->setTimeToServer(task.timeToServer);
    response->setTimeFromServer(simTime());
}

void EdgeApplConn::fillOffloadDecision(OffloadDecision* offloadDecisionMsg, Info taskInfo)
{
    offloadDecisionMsg->setTaskOwnerId(taskInfo.taskOwnerId);
    offloadDecisionMsg->setTaskSendId(taskInfo.taskSendId);
    offloadDecisionMsg->setServerId(RSU_ID);
}


void EdgeApplConn::refreshDisplay() const
{
    std::string buf;
//    buf = " Received: "+std::to_string(revTaskNum)+"\n Completed: "+std::to_string(compTaskNum)+"\n Queue Length: "+std::to_string(curQueueLength)+"\n OffloadCost"+std::to_string(totalOffloadCost);
//    buf = " Received: "+std::to_string(revTaskNum)+"\n Completed: "+std::to_string(compTaskNum);
    getParentModule()->getDisplayString().setTagArg("t", 0, buf.c_str());
}



void EdgeApplConn::updateEnqueueToSdn(Task task)
{
    ServerUpdate* newUpdate = new ServerUpdate();
    newUpdate->setServerId(RSU_ID);
//  UpdateState = TRUE means action "enqueue"
//  UpdateState = FALSE means action "dequeue"
    newUpdate->setUpdateState(TRUE);
    newUpdate->setTaskOwnerId(task.taskOwnerId);
    newUpdate->setTaskSendId(task.taskSendId);
    newUpdate->setComputationLoad(task.computationLoad);
    newUpdate->setUnitDelayCost(task.unitDelayCost);

    sendCheckChannel(newUpdate, "ControlEthgPort$o");
//    EV << "Sent an update to SDN \n" ;
}

void EdgeApplConn::updateDequeueToSdn()
{
    ServerUpdate* newUpdate = new ServerUpdate();
    newUpdate->setServerId(RSU_ID);
    newUpdate->setUpdateState(FALSE);
    sendCheckChannel(newUpdate, "ControlEthgPort$o");
//    EV << "Sent an update to SDN \n" ;
}


void EdgeApplConn:: sendCheckChannel(cMessage *msg, const char *gateName)
{
    double delay = 0;
    cChannel *txChannel = gate(gateName)->getTransmissionChannel();
    simtime_t txFinishTime = txChannel->getTransmissionFinishTime();
    if (txFinishTime <= simTime())
    {
    // channel free; send out packet immediately
        send(msg,gateName);
    }
    else
    {
        delay = txFinishTime.dbl() - simTime().dbl() + 0.0001;
        sendDelayed(msg, delay, gateName);
    }
}
