#ifndef SRC_VEINS_MODULES_EDGECOMPUTING_QUEUE_TASK_H_
#define SRC_VEINS_MODULES_EDGECOMPUTING_QUEUE_TASK_H_
#include <omnetpp.h>
#include "veins/base/utils/Coord.h"

class Task {
public:
    std::string taskId;
	double computationLoad;
    double unitDelayCost;
    double inputSize;
    int desId;
    int taskOwnerId;
    int taskSendId;
    double WPT;
    int forwardServerId;
    double expectSvArrTime;
    ::veins::Coord senderPos;
    ::omnetpp::simtime_t deadline;
    ::omnetpp::simtime_t generateTime;
//    double startComputationTime;
    ::omnetpp::simtime_t timeToServer;

    Task(void) {}
	
};

class Info {
public:
    int taskOwnerId;
    int taskSendId;
    int forwardServerId;

    Info(void) {}
};

#endif /* SRC_VEINS_MODULES_EDGECOMPUTING_QUEUE_TASK_H_ */
