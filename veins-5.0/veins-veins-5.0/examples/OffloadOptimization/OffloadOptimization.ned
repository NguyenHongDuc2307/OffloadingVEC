

//
// Copyright (C) 2017 Christoph Sommer <sommer@ccs-labs.org>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

import org.car2x.veins.modules.EdgeComputing.EdgeServerConn;
import org.car2x.veins.modules.EdgeComputing.SdnController;
import org.car2x.veins.modules.EdgeComputing.Router;
import org.car2x.veins.nodes.Scenario;

network OffloadOptimization extends Scenario
{
    submodules:
        RSU[8]: EdgeServerConn {
            @display("p=150,140;i=veins/sign/yellowdiamond;is=vs");
        }

        SdnController: SdnController {
            @display("p=6482,500;i=veins/sign/multicast");
        }
        
        Router: Router {
            @display("p=2482,500;i=veins/sign/router");
        }
        
        R0: Router {
            @display("p=4482,3500;i=veins/sign/router");
        }
        
        R1: Router {
            @display("p=8482,2000;i=veins/sign/router");
        }

    connections:
 //       RSU[0].ethg[0] <--> {  datarate = 1000Mbps; delay = 1ms; } <--> SdnController.gate++;
 //       RSU[1].ethg[0] <--> {  datarate = 1000Mbps; delay = 1ms; } <--> SdnController.gate++;
 //       RSU[2].ethg[0] <--> {  datarate = 1000Mbps; delay = 1ms; } <--> SdnController.gate++;
 //       RSU[3].ethg[0] <--> {  datarate = 1000Mbps; delay = 1ms; } <--> SdnController.gate++;
 //       RSU[4].ethg[0] <--> {  datarate = 1000Mbps; delay = 1ms; } <--> SdnController.gate++;
 //       RSU[5].ethg[0] <--> {  datarate = 1000Mbps; delay = 1ms; } <--> SdnController.gate++;
 //       RSU[6].ethg[0] <--> {  datarate = 1000Mbps; delay = 1ms; } <--> SdnController.gate++;
 //       RSU[7].ethg[0] <--> {  datarate = 1000Mbps; delay = 1ms; } <--> SdnController.gate++;
   		
        
        RSU[0].ethg[1] <--> {  datarate = 100Mbps; delay = 1ms; } <--> R0.gate++;
        RSU[1].ethg[1] <--> {  datarate = 100Mbps; delay = 1ms; } <--> R0.gate++;
        RSU[2].ethg[1] <--> {  datarate = 100Mbps; delay = 1ms; } <--> R0.gate++;
        RSU[3].ethg[1] <--> {  datarate = 100Mbps; delay = 1ms; } <--> R0.gate++;
        RSU[4].ethg[1] <--> {  datarate = 100Mbps; delay = 1ms; } <--> R0.gate++;
        RSU[5].ethg[1] <--> {  datarate = 100Mbps; delay = 1ms; } <--> R1.gate++;
        RSU[6].ethg[1] <--> {  datarate = 100Mbps; delay = 1ms; } <--> R1.gate++;
        RSU[7].ethg[1] <--> {  datarate = 100Mbps; delay = 1ms; } <--> R1.gate++;
        
        R0.gate++ <--> {  datarate = 100Mbps; delay = 1ms; } <--> Router.gate++;
        R1.gate++ <--> {  datarate = 100Mbps; delay = 1ms; } <--> Router.gate++;
        R0.gate++ <--> {  datarate = 1000Mbps; delay = 1ms; } <--> SdnController.gate++;
        R1.gate++ <--> {  datarate = 1000Mbps; delay = 1ms; } <--> SdnController.gate++;
        
}
