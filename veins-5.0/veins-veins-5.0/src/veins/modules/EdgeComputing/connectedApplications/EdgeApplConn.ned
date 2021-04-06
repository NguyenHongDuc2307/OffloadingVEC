

package org.car2x.veins.modules.EdgeComputing.connectedApplications;
import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;

simple EdgeApplConn extends DemoBaseApplLayer
{
    parameters:
    	@class(veins::EdgeApplConn);
    	double x;
    	double y;
    	double z;
// There are 2 scheduling modes:  SWPT-based (true) and FCFS-based (false)
		bool schedSWPT = default(true);
    	@display("i=block/app2");
    //	double broadcastInterval @unit("s")=default(1s);
	//	bool TCPMATLAB=default(false);
    gates:
        inout ControlEthgPort;
		inout DataEthgPort;        
}
