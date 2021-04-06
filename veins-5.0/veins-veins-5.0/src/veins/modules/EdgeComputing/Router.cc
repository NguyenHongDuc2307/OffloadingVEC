
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
#include "veins/modules/EdgeComputing/EdgeMessage/TaskRequest_m.h"
#include <vector>
#include <map>
#include <string>

using namespace veins;



class Router : public cSimpleModule
{
    private:
/*   Test */
        int from0,from1,from2,from3,from4,from5,from6,from7=0;
        int to0,to1,to2,to3,to4,to5,to6,to7=0;
/* End Test */
    protected:
        virtual void initialize() override;
        virtual void finish() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void refreshDisplay() const override;
};
Define_Module(Router);

void Router::initialize()
{
/*   Test */
/* End Test */
}

void Router::finish()
{
/*   Test */
    recordScalar("from0",from0);
    recordScalar("from1",from1);
    recordScalar("from2",from2);
    recordScalar("from3",from3);
    recordScalar("from4",from4);
    recordScalar("from5",from5);
    recordScalar("from6",from6);
    recordScalar("from7",from7);

    recordScalar("to0",to0);
    recordScalar("to1",to1);
    recordScalar("to2",to2);
    recordScalar("to3",to3);
    recordScalar("to4",to4);
    recordScalar("to5",to5);
    recordScalar("to6",to6);
    recordScalar("to7",to7);
  

/* End Test */
}

void Router::handleMessage(cMessage *msg)
{
    if (TaskRequest* request = dynamic_cast<TaskRequest*>(msg))
    {
        int desId = request->getDesId();
/*   Test */
        if (desId == 0)
        {
            to0++;
        }
        else if (desId == 1)
        {
            to1++;
        }
        else if (desId == 2)
        {
            to2++;
        }
        else if (desId == 3)
        {
            to3++;
        }
        else if (desId == 4)
        {
            to4++;
        }
        else if (desId == 5)
        {
            to5++;
        }
        else if (desId == 6)
        {
            to6++;
        }
        else if (desId == 7)
        {
            to7++;
        }
/* End Test */
        Info taskInfo;
        double delay = 0;
        TaskRequest* newRequest = new TaskRequest();
        *newRequest = *request;
        delete request;

        cChannel *txChannel = gate("gate$o",desId)->getTransmissionChannel();
        simtime_t txFinishTime = txChannel->getTransmissionFinishTime();
        if (txFinishTime <= simTime())
        {
    // channel free; send out packet immediately
            send(newRequest,"gate$o",desId);
        }
        else
        {
            delay = txFinishTime.dbl() - simTime().dbl() + 0.0001;
            sendDelayed(newRequest, delay, "gate$o", desId);
        }
        
        
    }
}


void Router::refreshDisplay() const
{
//    char buf[40];
//    sprintf(buf, "from0to1: %ld   from1to0: %ld", from0to1,from1to0);
//    getDisplayString().setTagArg("t", 0, buf);
}



