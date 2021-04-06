
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
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
#include <math.h>

#define SERVER_COM_SPEED 15
#define LOCAL_COM_SPEED 1
#define SWPT_MULTI_SERVER 0
#define NSO 1
#define NO 2
#define SERVER_NUMBER 8
#define TASK_TIMEOUT 2

using namespace veins;



class SdnController : public cSimpleModule
{
    private:
        int assignTaskNum[10];
        int curQueueLength[SERVER_NUMBER];
        int maxQueueLength[SERVER_NUMBER];
        double avgQueueLength[SERVER_NUMBER];
        int countQueue[SERVER_NUMBER];


        double tempOffloadCost[SERVER_NUMBER];
        double checkOffloadCost[SERVER_NUMBER];
        double tempOffloadDelayCost[SERVER_NUMBER];
        double checkOffloadDelayCost[SERVER_NUMBER];
        double tempOffloadCompCost[SERVER_NUMBER];
        double checkOffloadCompCost[SERVER_NUMBER];
        double tempExtraCost;
        double tempBufferCost;
        int checkQueueLength[SERVER_NUMBER];

    protected:
    virtual void fillOffloadDecision(OffloadDecision* offloadDecisionMsg, Info taskInfo, int targetServerId);
    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void updateServerQueueTable(ServerUpdate* serverUpdateMsg);
    virtual int taskAssignmentFunc_SWPTMULTI(TaskInfo* taskInfoMsg);
    virtual int taskAssignmentFunc_NSO(TaskInfo* taskInfoMsg);
    virtual int taskAssignmentFunc_NO(TaskInfo* taskInfoMsg);
    virtual void addToBuffer(TaskInfo* taskInfoMsg, int targetServerId);
    virtual void deleteFromBuffer(Task task, int serverId);
    virtual double getBufferCost_SWPT(Task task, int serverId);
    virtual double getBufferCost_FCFS(Task task, int serverId);

    Queue serverQueueTable[10];

// For buffering
    Task bufferTable[SERVER_NUMBER][50];
    int bufferLengthArr[SERVER_NUMBER];

    double computationSpeedArray[10];
    double V2I_DataRate = 100;
    double Eth_DataRate = 100;
    double unitOperatingCost = 0.3;
    int offloadMode = 0;
};
Define_Module(SdnController);

void SdnController::initialize()
{
    offloadMode = par("offloadMode");
    for (int i=0;i<SERVER_NUMBER;i++)
    {
        computationSpeedArray[i] = SERVER_COM_SPEED;
        curQueueLength[i] = 0;
        avgQueueLength[i] = 0;
        maxQueueLength[i] = 0;
        countQueue[i] = 0;
        tempOffloadCost[i] = 0;
        checkOffloadCost[i] = 0;
        checkOffloadDelayCost[i] = 0;
        checkOffloadCompCost[i] = 0;
        checkOffloadDelayCost[i] = 0;
        checkOffloadCompCost[i] = 0;
/* result collection */
        assignTaskNum[i] = 0;
/* end result collection */
    }
    assignTaskNum[9] = 0;  
}

void SdnController::finish()
{
/*   Result collection */
    recordScalar("assignTaskNum_local",assignTaskNum[9]);
    recordScalar("assignTaskNum_0",assignTaskNum[0]);
    recordScalar("assignTaskNum_1",assignTaskNum[1]);
    recordScalar("assignTaskNum_2",assignTaskNum[2]);
    recordScalar("assignTaskNum_3",assignTaskNum[3]);
    recordScalar("assignTaskNum_4",assignTaskNum[4]);
    recordScalar("assignTaskNum_5",assignTaskNum[5]);
    recordScalar("assignTaskNum_6",assignTaskNum[6]);
    recordScalar("assignTaskNum_7",assignTaskNum[7]);


    recordScalar("maxExpectedQueueLength_0",maxQueueLength[0]);
    recordScalar("maxExpectedQueueLength_1",maxQueueLength[1]);
    recordScalar("maxExpectedQueueLength_2",maxQueueLength[2]);
    recordScalar("maxExpectedQueueLength_3",maxQueueLength[3]);
    recordScalar("maxExpectedQueueLength_4",maxQueueLength[4]);
    recordScalar("maxExpectedQueueLength_5",maxQueueLength[5]);
    recordScalar("maxExpectedQueueLength_6",maxQueueLength[6]);
    recordScalar("maxExpectedQueueLength_7",maxQueueLength[7]);



    recordScalar("avgExpectedQueueLength_0",avgQueueLength[0]);
    recordScalar("avgExpectedQueueLength_1",avgQueueLength[1]);
    recordScalar("avgExpectedQueueLength_2",avgQueueLength[2]);
    recordScalar("avgExpectedQueueLength_3",avgQueueLength[3]);
    recordScalar("avgExpectedQueueLength_4",avgQueueLength[4]);
    recordScalar("avgExpectedQueueLength_5",avgQueueLength[5]);
    recordScalar("avgExpectedQueueLength_6",avgQueueLength[6]);
    recordScalar("avgExpectedQueueLength_7",avgQueueLength[7]);



    recordScalar("lastExpectedQueueLength_0",curQueueLength[0]);
    recordScalar("lastExpectedQueueLength_1",curQueueLength[1]);
    recordScalar("lastExpectedQueueLength_2",curQueueLength[2]);
    recordScalar("lastExpectedQueueLength_3",curQueueLength[3]);
    recordScalar("lastExpectedQueueLength_4",curQueueLength[4]);
    recordScalar("lastExpectedQueueLength_5",curQueueLength[5]);
    recordScalar("lastExpectedQueueLength_6",curQueueLength[6]);
    recordScalar("lastExpectedQueueLength_7",curQueueLength[7]);

    recordScalar("checkOffloadCost_0",checkOffloadCost[0]);
    recordScalar("checkOffloadCost_1",checkOffloadCost[1]);
    recordScalar("checkOffloadCost_2",checkOffloadCost[2]);
    recordScalar("checkOffloadCost_3",checkOffloadCost[3]);
    recordScalar("checkOffloadCost_4",checkOffloadCost[4]);
    recordScalar("checkOffloadCost_5",checkOffloadCost[5]);
    recordScalar("checkOffloadCost_6",checkOffloadCost[6]);
    recordScalar("checkOffloadCost_7",checkOffloadCost[7]);

    recordScalar("checkOffloadDelayCost_0",checkOffloadDelayCost[0]);
    recordScalar("checkOffloadDelayCost_1",checkOffloadDelayCost[1]);
    recordScalar("checkOffloadDelayCost_2",checkOffloadDelayCost[2]);
    recordScalar("checkOffloadDelayCost_3",checkOffloadDelayCost[3]);
    recordScalar("checkOffloadDelayCost_4",checkOffloadDelayCost[4]);
    recordScalar("checkOffloadDelayCost_5",checkOffloadDelayCost[5]);
    recordScalar("checkOffloadDelayCost_6",checkOffloadDelayCost[6]);
    recordScalar("checkOffloadDelayCost_7",checkOffloadDelayCost[7]);

    recordScalar("checkOffloadCompCost_0",checkOffloadCompCost[0]);
    recordScalar("checkOffloadCompCost_1",checkOffloadCompCost[1]);
    recordScalar("checkOffloadCompCost_2",checkOffloadCompCost[2]);
    recordScalar("checkOffloadCompCost_3",checkOffloadCompCost[3]);
    recordScalar("checkOffloadCompCost_4",checkOffloadCompCost[4]);
    recordScalar("checkOffloadCompCost_5",checkOffloadCompCost[5]);
    recordScalar("checkOffloadCompCost_6",checkOffloadCompCost[6]);
    recordScalar("checkOffloadCompCost_7",checkOffloadCompCost[7]);

    recordScalar("checkQueueLength_0",checkQueueLength[0]);
    recordScalar("checkQueueLength_1",checkQueueLength[1]);
    recordScalar("checkQueueLength_2",checkQueueLength[2]);
    recordScalar("checkQueueLength_3",checkQueueLength[3]);
    recordScalar("checkQueueLength_4",checkQueueLength[4]);
    recordScalar("checkQueueLength_5",checkQueueLength[5]);
    recordScalar("checkQueueLength_6",checkQueueLength[6]);
    recordScalar("checkQueueLength_7",checkQueueLength[7]);

    recordScalar("tempExtraCost",tempExtraCost);
    recordScalar("tempBufferCost",tempBufferCost);
/* End Result collection */
}

void SdnController::handleMessage(cMessage *msg)
{
    if (TaskInfo* taskInfoMsg = dynamic_cast<TaskInfo*>(msg)){
        int targetServerId;
        if (offloadMode == SWPT_MULTI_SERVER)
        {
            targetServerId = taskAssignmentFunc_SWPTMULTI(taskInfoMsg);
        }
        else if (offloadMode == NSO)
        {
            targetServerId = taskAssignmentFunc_NSO(taskInfoMsg);
        }
        else        // NO
        {
            targetServerId = taskAssignmentFunc_NO(taskInfoMsg);
        }

// update buffer for newly assigned task    
        addToBuffer(taskInfoMsg,targetServerId);

        int forwardServerId = taskInfoMsg->getForwardServerId();         
        Info taskInfo;
        taskInfo.taskSendId = taskInfoMsg->getTaskSendId();
        taskInfo.taskOwnerId = taskInfoMsg->getTaskOwnerId();
        taskInfo.forwardServerId = forwardServerId;
        delete taskInfoMsg;

        OffloadDecision* offDecisionMsg = new OffloadDecision();
        fillOffloadDecision(offDecisionMsg,taskInfo,targetServerId);
        send(offDecisionMsg,"gate$o",forwardServerId);
        EV << "Assign a task to server " << targetServerId << "\n";
/* result collection*/       
        if (targetServerId == -1)
        {
            assignTaskNum[9]++;
        }
        else
        {
            assignTaskNum[targetServerId]++;
        }
 /* end result collection*/
    }

    else if (ServerUpdate* serverUpdateMsg = dynamic_cast<ServerUpdate*>(msg))
    {
//        int serverId = serverUpdateMsg->getServerId();
//        EV << "Received an update from server " << serverId <<"\n";
        updateServerQueueTable(serverUpdateMsg);
    }
}

void SdnController::fillOffloadDecision(OffloadDecision* offloadDecisionMsg, Info taskInfo, int targetServerId)
{
    offloadDecisionMsg->setTaskOwnerId(taskInfo.taskOwnerId);
    offloadDecisionMsg->setTaskSendId(taskInfo.taskSendId);
/* Test data forwarding*/
//    offloadDecisionMsg->setServerId(1);
    offloadDecisionMsg->setServerId(targetServerId);
}


void SdnController::updateServerQueueTable(ServerUpdate* serverUpdateMsg)
{
    int serverId = serverUpdateMsg->getServerId();
    if (serverUpdateMsg->getUpdateState()== TRUE)
    {
        Task task;
        task.taskOwnerId = serverUpdateMsg->getTaskOwnerId();
        task.taskSendId = serverUpdateMsg->getTaskSendId();
        task.computationLoad = serverUpdateMsg->getComputationLoad();
        task.unitDelayCost = serverUpdateMsg->getUnitDelayCost();
        task.WPT = (task.computationLoad)/(computationSpeedArray[serverId]*task.unitDelayCost);
        if ((offloadMode == SWPT_MULTI_SERVER) || (offloadMode == NO))
        {
            serverQueueTable[serverId].enqueue_SWPT(task);
        }
        else if (offloadMode == NSO)
        {
            serverQueueTable[serverId].enqueue(task);
        }
        
//        EV << "Enqueue a new task to server " << serverId <<"\n";

// update buffer for newly arriving task 
        deleteFromBuffer(task,serverId);
 //       EV << "Queue length 0 = " << serverQueueTable[0].getRear()+1 <<"\n";
 //       EV << "Queue length 1 = " << serverQueueTable[1].getRear()+1 <<"\n";
    }

    else if (serverUpdateMsg->getUpdateState()== FALSE)
    {
        serverQueueTable[serverId].dequeue();
//        EV << "Dequeue a task from server " << serverId <<"\n";
    }

// Queue length monitor
    curQueueLength[serverId] = serverQueueTable[serverId].getRear()+1;
    if (curQueueLength[serverId]>maxQueueLength[serverId])
    {
        maxQueueLength[serverId] = curQueueLength[serverId];
    }
    countQueue[serverId]++;
    avgQueueLength[serverId] = (avgQueueLength[serverId]*(countQueue[serverId]-1)+curQueueLength[serverId])/countQueue[serverId];
}

// Making the decision of local/offload
// Assign the task to the most suitable server
int SdnController::taskAssignmentFunc_SWPTMULTI(TaskInfo* taskInfo)
{
    Task task;
    task.computationLoad = taskInfo->getComputationLoad();
    task.unitDelayCost = taskInfo->getUnitDelayCost();
    task.inputSize = taskInfo->getInputSize();

    int targetServerID = -1;
    double localCost = (task.unitDelayCost)*((task.computationLoad)/LOCAL_COM_SPEED);
    double extraCost;   // the summation of the cost caused by Waiting time and Extra time
    double offloadCostMin = localCost;
    double offloadDelayCost, offloadCost = 0;
    double Eth_DataRate_Mod = Eth_DataRate;

    double bufferCost = 0;

    
    double offloadCompCost =  task.computationLoad*unitOperatingCost;
    for (int i=0;i<SERVER_NUMBER;i++)
    {
        Eth_DataRate_Mod = Eth_DataRate;
        task.WPT = (task.computationLoad)/(computationSpeedArray[i]*task.unitDelayCost);
//  double WPT = (newTask->getComputationLoad())/(computationSpeedArray[i]*newTask->getUnitDelayCost());
// Computing Cost for offloading  
        bufferCost = getBufferCost_SWPT(task,i);

        extraCost = serverQueueTable[i].getExtraCost_SWPT(task);
        
        if (extraCost == -1)
        {
            tempOffloadCost[i] = -1;
            continue;
        } 
        else
        {
            if (taskInfo->getForwardServerId() == i)
            {
/* Prioritize the adjacent server*/
                Eth_DataRate_Mod = 1000*1000;
            }
        

            offloadDelayCost = bufferCost + extraCost + task.unitDelayCost*((task.inputSize/V2I_DataRate)+(task.inputSize/Eth_DataRate_Mod)+(task.computationLoad/computationSpeedArray[i]));
            offloadCost = offloadDelayCost + offloadCompCost;

            
            tempOffloadDelayCost[i] = offloadDelayCost;
            tempOffloadCompCost[i] = offloadCompCost;
            tempOffloadCost[i] = offloadCost;
/* Test delay*/
            EV << " Task number " << taskInfo->getTaskOwnerId() << "_" << taskInfo->getTaskSendId() << "\n";
            EV << "Expected offload cost to sever " << i << "=" << "\n";
            EV << "offloadCost = " <<  offloadCost << "\n";
            EV << "offloadDelayCost = " <<  offloadDelayCost << "\n";
            EV << "bufferCost = " <<  bufferCost << "\n";
//            EV << "extraCost = " <<  *(extraCost) + *(extraCost+1) << "\n";
/* End test delay*/
            if (offloadCost <= offloadCostMin)
            {
                offloadCostMin = offloadCost;
                targetServerID = i;
            }
        }

    }

    if (targetServerID == -1)
    {
        for (int i=0;i<SERVER_NUMBER;i++)
        {
            checkOffloadCost[i] = tempOffloadCost[i];
            checkOffloadDelayCost[i] = tempOffloadDelayCost[i];
            checkOffloadCompCost[i] = tempOffloadCompCost[i];
            checkQueueLength[i] = curQueueLength[i];
            tempBufferCost = bufferCost;
            tempExtraCost = extraCost;
        }
        
    }

    return targetServerID;
}

int SdnController::taskAssignmentFunc_NSO(TaskInfo* taskInfo)
{
    Task task;
    task.computationLoad = taskInfo->getComputationLoad();
    task.unitDelayCost = taskInfo->getUnitDelayCost();
    task.inputSize = taskInfo->getInputSize();

    int targetServerID = -1;
    double localCost = (task.unitDelayCost)*((task.computationLoad)/LOCAL_COM_SPEED);
    double extraCost;   // the summation of the cost caused by Waiting time and Extra time
    double offloadCostMin = localCost;
    double offloadDelayCost, offloadCost = 0;
    double Eth_DataRate_Mod = Eth_DataRate;

    double bufferCost = 0;

    
    double offloadCompCost =  task.computationLoad*unitOperatingCost;
    for (int i=0;i<SERVER_NUMBER;i++)
    {
        Eth_DataRate_Mod = Eth_DataRate;
        task.WPT = (task.computationLoad)/(computationSpeedArray[i]*task.unitDelayCost);
//  double WPT = (newTask->getComputationLoad())/(computationSpeedArray[i]*newTask->getUnitDelayCost());
// Computing Cost for offloading  
        extraCost = serverQueueTable[i].getExtraCost_FCFS(task);
        
        bufferCost = getBufferCost_FCFS(task,i);

        if (extraCost == -1)
        {
            continue;
        } 
        else
        {
            if (taskInfo->getForwardServerId() == i)
            {
/* Prioritize the adjacent server*/
                Eth_DataRate_Mod = 1000*1000;
            }

            offloadDelayCost = bufferCost + extraCost + task.unitDelayCost*((task.inputSize/V2I_DataRate)+(task.inputSize/Eth_DataRate_Mod)+(task.computationLoad/computationSpeedArray[i]));
            offloadCost = offloadDelayCost + offloadCompCost;
            
            if (offloadCost < offloadCostMin)
            {
                offloadCostMin = offloadCost;
                targetServerID = i;
            }
        }

    }

    
    return targetServerID;
}


int SdnController::taskAssignmentFunc_NO(TaskInfo* taskInfo)
{
    Task task;
    task.computationLoad = taskInfo->getComputationLoad();
    task.unitDelayCost = taskInfo->getUnitDelayCost();
    task.inputSize = taskInfo->getInputSize();

    int targetServerID = -1;
    double localCost = (task.unitDelayCost)*((task.computationLoad)/LOCAL_COM_SPEED);
    double extraCost;   // the summation of the cost caused by Waiting time and Extra time
    double offloadCostMin = localCost;
    double offloadDelayCost, offloadCost = 0;
    double Eth_DataRate_Mod = Eth_DataRate;

    double bufferCost = 0;

    
    double offloadCompCost =  task.computationLoad*unitOperatingCost;
    int i = taskInfo->getForwardServerId();

    task.WPT = (task.computationLoad)/(computationSpeedArray[i]*task.unitDelayCost);
//  double WPT = (newTask->getComputationLoad())/(computationSpeedArray[i]*newTask->getUnitDelayCost());
// Computing Cost for offloading  
    extraCost = serverQueueTable[i].getExtraCost_SWPT(task);

    bufferCost = getBufferCost_SWPT(task,i);

    if (extraCost == -1)
    {
        return -1;
    } 
    else
    {
        offloadDelayCost = bufferCost + extraCost + task.unitDelayCost*((task.inputSize/V2I_DataRate)+(task.computationLoad/computationSpeedArray[i]));
        offloadCost = offloadDelayCost + offloadCompCost;
            
        if (offloadCost < localCost)
        {
            targetServerID = i;
        }
    }
    return targetServerID;
}



void SdnController::addToBuffer(TaskInfo* taskInfoMsg, int targetServerId)
{
    if (targetServerId == -1)
    {
        return;
    }
    
    Task task;
    task.taskOwnerId = taskInfoMsg->getTaskOwnerId();
    task.taskSendId = taskInfoMsg->getTaskSendId();
    task.computationLoad = taskInfoMsg->getComputationLoad();
    task.unitDelayCost = taskInfoMsg->getUnitDelayCost();
    task.WPT = (task.computationLoad)/(computationSpeedArray[targetServerId]*task.unitDelayCost);
    task.expectSvArrTime = simTime().dbl() + taskInfoMsg->getInputSize()/V2I_DataRate +  taskInfoMsg->getInputSize()/Eth_DataRate;

    int thisBufferLength = bufferLengthArr[targetServerId];
    bufferTable[targetServerId][thisBufferLength] = task;
    bufferLengthArr[targetServerId]++;
}

void SdnController::deleteFromBuffer(Task task, int serverId)
{
    
    int thisBufferLength = bufferLengthArr[serverId];


    for(int i=0; i<thisBufferLength; i++)
    {
        if ((bufferTable[serverId][i].taskOwnerId == task.taskOwnerId) && (bufferTable[serverId][i].taskSendId == task.taskSendId))
        {
            for (int j = i; j < thisBufferLength-1; j++)
            {
                bufferTable[serverId][j] = bufferTable[serverId][j+1];
            }
            bufferLengthArr[serverId]--;
            break;
        }
    }
}


double SdnController::getBufferCost_SWPT(Task task, int serverId)
{
    double bufferCost = 0;
    int thisBufferLength = bufferLengthArr[serverId];
    
    for (int i=0;i<thisBufferLength; i++)
    {
        if (bufferTable[serverId][i].WPT <= task.WPT)
        {
            bufferCost =  bufferCost + task.unitDelayCost*(bufferTable[serverId][i].computationLoad/SERVER_COM_SPEED);
        }
        else
        {
            bufferCost =  bufferCost + (task.computationLoad/SERVER_COM_SPEED)*bufferTable[serverId][i].unitDelayCost;
        }
    }
    return bufferCost;
}

double SdnController::getBufferCost_FCFS(Task task, int serverId)
{
    double curTime;
    double bufferCost = 0;
    int thisBufferLength = bufferLengthArr[serverId];
    
    for (int i=0;i<thisBufferLength; i++)
    {
        bufferCost =  bufferCost + task.unitDelayCost*(bufferTable[serverId][i].computationLoad/SERVER_COM_SPEED);
    }

// By the way , clear the long living task, just test
    for(int i=0; i<thisBufferLength; i++)
    {
        curTime = simTime().dbl();
        if ((bufferTable[serverId][i].expectSvArrTime+TASK_TIMEOUT) < curTime)
        {
            Task newTask = bufferTable[serverId][i];
            deleteFromBuffer(newTask,serverId);
        }
    }


    return bufferCost;
}








