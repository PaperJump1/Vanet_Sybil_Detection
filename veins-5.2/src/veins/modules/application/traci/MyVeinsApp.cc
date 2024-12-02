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

#include "veins/modules/application/traci/MyVeinsApp.h"


using namespace veins;

Define_Module(veins::MyVeinsApp);

void MyVeinsApp::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
        if (stage == 0) {
            // Initializing members and pointers of your application goes here
            EV << "Initializing " << par("appName").stringValue() << std::endl;
            totalSpeed = 0.0;
            speedUpdateCount = 0;
            //vehicle_number=vehicle_number+1;
            sendBeacon = new cMessage("send Beacon");
        }
        else if (stage == 1) {
            // Initializing members that require initialized other modules goes here
            if (sendBeacon->isScheduled()) {
               cancelEvent(sendBeacon);
            }
              scheduleAt(simTime()+1,sendBeacon);
         }

}

void MyVeinsApp::finish()
{
    DemoBaseApplLayer::finish();
    // statistics recording goes here
    // 计算平均车速
   EV << "ok" << std::endl;
   double averageSpeed = speedUpdateCount > 0 ? totalSpeed / speedUpdateCount : 0;
   if(averageSpeed >13){
      EV << "Average vehicle speed: " << averageSpeed << " m/s" << std::endl;
    }
}

void MyVeinsApp::onBSM(DemoSafetyMessage* bsm)
{
    // Your application has received a beacon message from another car or RSU
    // code for handling the message goes here
}

void MyVeinsApp::onWSM(BaseFrame1609_4* wsm)
{
    // Your application has received a data message from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void MyVeinsApp::onWSA(DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void MyVeinsApp::handleSelfMsg(cMessage* msg)
{
    //DemoBaseApplLayer::handleSelfMsg(msg);
    // this method is for self messages (mostly timers)
    // it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
    EV<<"send" << std::endl;
   if (msg == sendBeacon) {
        BSMMessage* rsuBeacon = new BSMMessage();
        rsuBeacon->setVehicleID(this->getParentModule()->getIndex());
        rsuBeacon->setSpeed(traciVehicle->getSpeed());
        mobility = TraCIMobilityAccess().get(getParentModule());
        Coord position = mobility->getPositionAt(simTime());
        rsuBeacon->setPosX(position.x);
        rsuBeacon->setPosY(position.y);
        //新建WSM，这是应用层和MAC层通信的消息
        BaseFrame1609_4* WSM = new BaseFrame1609_4();
        //把rsuBeacon封装在WSM中
        WSM->encapsulate(rsuBeacon);
        //设置WSM的基本信息
        populateWSM(WSM);
        sendDown(WSM);

        //send(WSM,lowerLayerOut);
        EV << "send success" <<endl;
        if (simTime() < 2000) {
           scheduleAt(simTime()+4,sendBeacon);
         }
      return;
   }
}

void MyVeinsApp::handlePositionUpdate(cObject* obj)
{
   DemoBaseApplLayer::handlePositionUpdate(obj);
   // the vehicle has moved. Code that reacts to new positions goes here.
   // member variables such as currentPosition and currentSpeed are updated in the parent class
   EV << "change1" << std::endl;
   double currentSpeed = traciVehicle->getSpeed();
   if(currentSpeed>=0){
   totalSpeed += currentSpeed;  // 累加车速
   speedUpdateCount++;  // 更新车速更新的次数
     EV << "Current simulation time: " << simTime() << ", vehicle speed: " << currentSpeed << " m/s" << std::endl;
  }
}
