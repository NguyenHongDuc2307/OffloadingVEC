#ifndef SRC_VEINS_MODULES_EDGECOMPUTING_QUEUE_SERVERQUEUE_H_
#define SRC_VEINS_MODULES_EDGECOMPUTING_QUEUE_SERVERQUEUE_H_
#include "veins/modules/EdgeComputing/Queue/Task.h"
#include <omnetpp.h>
#define SERVER_COM_SPEED 15

const int QUEUE_SIZE = 20;

class Queue{
    Task a[QUEUE_SIZE];
	int rear;
	int front;
	public:
		Queue() {
			rear = front = -1;
		}

		int getRear(){
			return rear;
		}

//  Assume that the "task" is inserted to the queue by SWPT principle
//  Calculate the summation of the cost caused by Waiting time and Extra time
		double getExtraCost_SWPT(Task task)
		{
			double r0,r1 = 0;

			if (front == -1)
			{
				r0=r1=0;
				return r0;
			}

			if (rear == QUEUE_SIZE - 1)
			{
				// This server is full
				r0=r1=-1;
				return r0;
			}
			else
			{
				if (rear == -1)
				{
					r0=r1=0;
					return 0;
				}
				else if (rear == 0)
				{
					r0 = task.unitDelayCost*(a[0].computationLoad/SERVER_COM_SPEED);
					r1 = 0;
					return r0+r1;
				}
				else
				{
					for (int i = 1; i<=rear; i++)
					{
						if(a[i].WPT <= task.WPT)
						{
							r0=r0 + task.unitDelayCost*(a[i].computationLoad/SERVER_COM_SPEED);
						}
						else
						{
							r1= r1 + (task.computationLoad/SERVER_COM_SPEED)*a[i].unitDelayCost;
						}
					}
					r0 = r0 + task.unitDelayCost*(a[0].computationLoad/SERVER_COM_SPEED);
				}
			}
			return r0+r1;
		}

//  Assume that the "task" is inserted to the queue by FCFS principle
		double getExtraCost_FCFS(Task task)
		{
			double r0,r1 = 0;

			if (front == -1)
			{
				r0=r1=0;
				return r0;
			}

			if (rear == QUEUE_SIZE - 1)
			{
				// This server is full
				r0=r1=-1;
				return r0;
			}
			else
			{
				if (rear == -1)
				{
				// Empty server
					r0=r1=0;
					return r0;
				}
				else if (rear == 0)
				{
					r0 = task.unitDelayCost*(a[0].computationLoad/SERVER_COM_SPEED);
					r1 = 0;
					return r0+r1;
				}
				else if (rear >= 0)
				{
					for (int i = 0; i<=rear; i++)
					{
						r0=r0 + task.unitDelayCost*(a[i].computationLoad/SERVER_COM_SPEED);					
					}
				}
			}
			return r0+r1;
		}

		void enqueue_SWPT(Task task){
            if (front == -1) {
	    	front++;
	        }
	        if (rear == QUEUE_SIZE - 1)
	        {

	        }
	        else
	        {
				Task temp;
				rear++;
		        a[rear] = task;
				if (rear >= 2)
				{
					for (int i=rear-1; i>0;i--)
					{
						if (a[i].WPT > task.WPT)
						{
							temp = a[i];
							a[i] = task;
							a[i+1] = a[i];
						}
					}
				}
	        }
        }
		
		void enqueue(Task task){
            if (front == -1) {
	    	front++;
	        }
	        if (rear == QUEUE_SIZE - 1)
	        {

	        }
	        else
	        {
				Task temp;
				rear++;
		        a[rear] = task;
	        }
        }

        Task dequeue(){
            int i;
	        for (i = 0; i < rear - 1; i++){
		        a[i] = a[i + 1];
	        }
	        rear--;
	        return a[0];
        }

        Task getFront(){
            return a[0];
        }

};

#endif  /*SRC_VEINS_MODULES_EDGECOMPUTING_QUEUE_SERVERQUEUE_H_*/
