//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
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

#include "veins/modules/application/traci/VehicleFlow.h"



using namespace veins;

Define_Module(veins::VehicleFlow);

void VehicleFlow::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
        if (stage == 0) {
            // Initializing members and pointers of your application goes here
            EV << "Initializing " << par("appName").stringValue() << std::endl;
            totalSpeed = 0.0;
            speedUpdateCount = 0;
            sendBeacon = new cMessage("send Beacon");
            test = new cMessage("test");
            lock = 1;
        }
        else if (stage == 1) {
            // Initializing members that require initialized other modules goes here
            if (sendBeacon->isScheduled()) {
               cancelEvent(sendBeacon);
            }
            scheduleAt(simTime()+1,sendBeacon);
            //sendBSM(0);
         }
}

void VehicleFlow::finish()
{
    DemoBaseApplLayer::finish();
    // statistics recording goes here


}

void VehicleFlow::onBSM(DemoSafetyMessage* bsm)
{
    // Your application has received a beacon message from another car or RSU
    // code for handling the message goes here
}

void VehicleFlow::onWSM(BaseFrame1609_4* wsm)
{
    // Your application has received a data message from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void VehicleFlow::onWSA(DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void VehicleFlow::handleSelfMsg(cMessage* msg)
{
    //DemoBaseApplLayer::handleSelfMsg(msg);
    // this method is for self messages (mostly timers)
    // it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
 /**
   if (msg == test && traciVehicle->getLaneId() != "gneE12_0") {
      sendBSM(1);
      scheduleAt(simTime()+ 1 + uniform(0.01, 0.2),test);
      return;
   }
   else if(msg == test && traciVehicle->getLaneId() == "gneE12_0"){
       sendBSM(2);
   }
   else if(msg == sendBeacon){
       if(traciVehicle->getLaneId() == "-gneE15_0"){
           sendBSM(0);
           scheduleAt(simTime() + uniform(0.01, 0.2),test);
       }
       else{
           scheduleAt(simTime() + uniform(0.01, 0.2),sendBeacon);
       }
   }
   **/
    if(msg == sendBeacon && traciVehicle->getVType() == "vtype0"){
      sendBSM(1);
      scheduleAt(simTime() + 1 + uniform(0.01, 0.2),sendBeacon);
      EV<< "LALALA" <<std::endl;
    }
    else if(msg == sendBeacon && traciVehicle->getVType() == "vtype1"){
      sendBSM(1);
      scheduleAt(simTime() + 0.1 + uniform(0.005, 0.01),sendBeacon);
      EV<< "HAHAHA" <<std::endl;
    }
}

void VehicleFlow::handlePositionUpdate(cObject* obj)
{
   DemoBaseApplLayer::handlePositionUpdate(obj);
   // the vehicle has moved. Code that reacts to new positions goes here.
   // member variables such as currentPosition and currentSpeed are updated in the parent class
   double currentSpeed = traciVehicle->getSpeed();
   if(currentSpeed>=0){
     totalSpeed += currentSpeed;  // 累加车速
     speedUpdateCount++;  // 更新车速更新的次数
     EV << "Current simulation time: " << simTime() << ", vehicle speed: " << currentSpeed << " m/s" << std::endl;
     EV << "Lane ID = " << traciVehicle->getLaneId() << std::endl;
  }


}

void VehicleFlow::sendBSM(int type){

    BSMMessage* Beacon = new BSMMessage();
    Beacon->setVehicleID(this->getParentModule()->getIndex());
    Beacon->setSpeed(traciVehicle->getSpeed());
    Beacon->setType(type);

    mobility = TraCIMobilityAccess().get(getParentModule());
    Coord position = mobility->getPositionAt(simTime());
    std::string laneID = traciVehicle->getLaneId();

    Beacon->setPosX(position.x);
    Beacon->setPosY(position.y);
    Beacon->setLaneID(laneID);

    //新建WSM，这是应用层和MAC层通信的消息
    BaseFrame1609_4* WSM = new BaseFrame1609_4();

    //把rsuBeacon封装在WSM中
    WSM->encapsulate(Beacon);
    //设置WSM的基本信息
    populateWSM(WSM);
    sendDown(WSM);
    sendNumber++;
    //send(WSM,lowerLayerOut);
    EV << "Send Success" << std::endl;
    EV << "SendNumber= " << sendNumber << std::endl;
}





