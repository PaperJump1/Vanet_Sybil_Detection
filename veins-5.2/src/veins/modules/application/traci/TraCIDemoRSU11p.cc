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

#include "veins/modules/application/traci/TraCIDemoRSU11p.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"


using namespace veins;

Define_Module(veins::TraCIDemoRSU11p);

void TraCIDemoRSU11p::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        EV << "Initializing RSU" << std::endl;
    }
    else if (stage == 1) {

    }
}



void TraCIDemoRSU11p::onWSA(DemoServiceAdvertisment* wsa)
{
    // if this RSU receives a WSA for service 42, it will tune to the chan
    EV << "DemoServiceAdvertisment receive" << std::endl;
    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    }
}

void TraCIDemoRSU11p::onWSM(BaseFrame1609_4* frame)
{
    EV<<"onWSA BaseFrame1609_4 receive" <<std::endl;
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);
    // this rsu repeats the received traffic update in 2 seconds plus some random delay
    sendDelayedDown(wsm->dup(), 2 + uniform(0.01, 0.2));
}


void TraCIDemoRSU11p::handleLowerMsg(cMessage* msg) {
    EV << "receive message  !!!" << endl;
    //DemoBaseApplLayer::handleLowerMsg(msg);
    //消息传换成WSM
    BaseFrame1609_4* WSM = check_and_cast<BaseFrame1609_4*>(msg);
    //从WSM中解封数据包
    cPacket* enc = WSM->getEncapsulatedPacket();
    //数据包转换成BeaconRSU
    BSMMessage* data = dynamic_cast<BSMMessage*>(enc);

    if(a!=data->getVehicleID()){
      // RSUIndex.record(bc->getVehicleID());
       a=data->getVehicleID();
  }

  EV <<"send message RSU id: " <<data->getVehicleID() <<" speed = " << data->getSpeed()  <<" posX= " << data->getPosX() << " posY= "<< data->getPosY() << endl;
}

