
#include "veins/modules/EdgeComputing/connectedApplications/EdgeApplSafe.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "veins/modules/EdgeComputing/Information/CarInfo.h"
#include "veins/modules/EdgeComputing/EdgeMessage/CarInfoWSM_m.h"
#include "veins/modules/EdgeComputing/EdgeMessage/CarControl_m.h"
#include "veins/modules/EdgeComputing/EdgeMessage/TaskRequest_m.h"
#include "veins/modules/EdgeComputing/EdgeMessage/TaskResponse_m.h"
#include "veins/modules/EdgeComputing/Queue/Task.h"
#include "veins/modules/EdgeComputing/Queue/serverQueue.h"
#include <omnetpp.h>

#include <math.h>

#include <fstream>
#include <iostream>

#include <string>

using namespace veins;

Define_Module(veins::EdgeApplSafe);


#include<winsock.h>
#pragma comment(lib,"ws2_32.lib")

void EdgeApplSafe::initialize(int stage)
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
    //    curPos = mobility->getPositionAt(simTime());


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

void EdgeApplSafe::finish()
{

    DemoBaseApplLayer::finish();
    recordScalar("revTaskNum",revTaskNum);
    recordScalar("compTaskNum",compTaskNum);
    cancelAndDelete(taskScheduled);
 /*
    CHAR SOver[15]="SimulationOVER";
    if(TCPMATLAB){
        ::send(sockclient,SOver,15,0);//End Matlab Server
        closesocket(sockclient);
    }
*/


}

void EdgeApplSafe::onBSM(DemoSafetyMessage* bsm)
{

}

void EdgeApplSafe::onWSM(BaseFrame1609_4* wsm)
{
    if(TaskRequest* request = dynamic_cast<TaskRequest*>(wsm)){
            EV << "Edge server receive one message";
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



     //       EV << "A new request received from = " << task.senderPos << endl;
     //       EV << " Queue Rear before enqueue is " << taskQueue.getRear() << endl;
            if (schedSWPT)
            {
                taskQueue.enqueue_SWPT(task);
            }
            else
            {
                taskQueue.enqueue(task);
            }       
            
            curQueueLength = taskQueue.getRear()+1;
     //       EV << " Queue Rear after enqueue is " << taskQueue.getRear() << endl;
            if(taskQueue.getRear() == 0){
                EV << "A new task is queued up first " << taskQueue.getFront().taskId;
                scheduleAt(simTime() + task.computationLoad/computationSpeed, taskScheduled);
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

void EdgeApplSafe::onWSA(DemoServiceAdvertisment* wsa)
{

}

//bool cmp(std::pair<int,std::pair<double,int>> a, std::pair<int,std::pair<double,int>> b) {
//    return a.second.first > b.second.first;
//}



void EdgeApplSafe::handleSelfMsg(cMessage* msg)
{


    if(msg==taskScheduled){
        compTaskNum++;
        Task task = taskQueue.dequeue();
        curQueueLength = taskQueue.getRear()+1;
        TaskResponse* response = new TaskResponse();
        fillTaskResponse(response,task);
        populateWSM(response);
        sendDown(response);
        
//        EV << " Queue Rear 2 is " << taskQueue.getRear();
        if(taskQueue.getRear() != -1)
        {
            EV << "A new task is queued up second " << taskQueue.getFront().taskId;
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

void EdgeApplSafe::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);

}

void EdgeApplSafe::fillTaskResponse(TaskResponse* response, Task task)
{
    response->setTaskOwnerId(task.taskOwnerId);
    response->setTaskSendId(task.taskSendId);
    response->setServerId(myId);
    response->setDeadline(task.deadline);
    response->setComputationLoad(task.computationLoad);
    response->setTimeToServer(task.timeToServer);
    response->setTimeFromServer(simTime());
}


void EdgeApplSafe::refreshDisplay() const
{
    std::string buf;
    buf = " Received: "+std::to_string(revTaskNum)+"\n Completed: "+std::to_string(compTaskNum)+"\n Queue Length: "+std::to_string(curQueueLength);
    getParentModule()->getDisplayString().setTagArg("t", 0, buf.c_str());
}
