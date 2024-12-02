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

#include "veins/modules/application/traci/VehicleFlowRSU.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/modules/phy/Decider80211p.h"
#include "veins/modules/phy/DeciderResult80211.h"
#include "veins/base/phyLayer/PhyToMacControlinfo.h"
//#include "veins/modules/mobility/traci/TraCIScenarioManagerAccess.h"
#include <fstream>
#include <math.h>


using namespace veins;
using veins::TraCIMobilityAccess;

Define_Module(veins::VehicleFlowRSU);

void VehicleFlowRSU::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        EV << "Initializing RSU" << std::endl;
        test = new cMessage("test");
        fileName = "c:\\Users\\a6518\\Desktop\\vanet\\omnetpp-5.6.2\\samples\\VanetExperiment\\out\\result.csv";
        outFile.open(fileName);
        outFile << "sendTime,total Vehicle,average Speed,receive packet,predict Vehicle Number,predict packet,packet loss,vehicle loss,RSSI" << std::endl;

    }
    else if (stage == 1) {
        scheduleAt(simTime()+ 5,test);
    }
}



void VehicleFlowRSU::onWSA(DemoServiceAdvertisment* wsa)
{
    // if this RSU receives a WSA for service 42, it will tune to the chan
    EV << "DemoServiceAdvertisment receive" << std::endl;
    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    }
}

void VehicleFlowRSU::onWSM(BaseFrame1609_4* frame)
{
    EV<<"onWSA BaseFrame1609_4 receive" <<std::endl;
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);
    // this rsu repeats the received traffic update in 2 seconds plus some random delay
    sendDelayedDown(wsm->dup(), 2 + uniform(0.01, 0.2));
}


void VehicleFlowRSU::handleLowerMsg(cMessage* msg) {
    BaseFrame1609_4* WSM = check_and_cast<BaseFrame1609_4*>(msg);    //消息传换成WSM
    cPacket* enc = WSM->getEncapsulatedPacket();              //从WSM中解封数据包
    BSMMessage* data = dynamic_cast<BSMMessage*>(enc);        //数据包转换成BeaconRSU


    EV <<"vehicleID = "<< data->getVehicleID() << " receive message !" << " posX = " << data->getPosX() << "posY = "<< data->getPosY()<< " distance = "<<Distance(data->getPosX(),data->getPosY()) <<" RSSI = " << check_and_cast<DeciderResult80211*>(check_and_cast<PhyToMacControlInfo*>(WSM->getControlInfo())->getDeciderResult())->getRecvPower_dBm() <<endl;

    int flag = 0;                                                      //旗標控制differ_pseudo

    if(data->getLaneID() != "gneE81_0" || data->getLaneID() != "gneE81_1"){                               //判斷車道
        if(data->getSpeed() > 2.5){
            count++;
            speed_sum = speed_sum + (data->getSpeed() * 3.6);
        }
        totalVehicle = totalVehicle + 1;
        packet_receive = packet_receive + 1;
        rssi = check_and_cast<DeciderResult80211*>(check_and_cast<PhyToMacControlInfo*>(WSM->getControlInfo())->getDeciderResult())->getRecvPower_dBm();
    }
}

void VehicleFlowRSU::handleSelfMsg(cMessage* msg){
    if (msg == test) {
        if(speed_sum == 0){
            EV<<" ERROR2" << std::endl;
            speed_sum++;
        }

        if(count==0){
            EV<<" ERROR2 " << std::endl;
            count++;
        }


        packet_receive_store[packet_time_count] = packet_receive;                          //將1秒鐘所收到的封包數量存進陣列中
        for(int i=0 ;i<5;i++){
          total_packet_receive = total_packet_receive + packet_receive_store[i];         //計算一段時間內所收到的封包數量
          EV << "debug " <<packet_receive_store[i] <<std::endl;
        }


        averageSpeed = speed_sum / count;

        if(averageSpeed == 1){
            averageSpeed = vehicle_speed_store[speed_time_count];
        }

        mobility = TraCIMobilityAccess().get(getParentModule()->getParentModule());
        auto traci = mobility->getCommandInterface();


        //std::string tlState1 =  traci->trafficlight("gneJ55").getCurrentState();
        //std::string tlState2 = traci->trafficlight("gneJ54").getCurrentState();
        //EV << tlState1 <<std::endl;                                               //紅路燈處理
        /**if(tlState1[0] == 'G' && tlState2[0] == 'G') {
            if(traffic_light_time <5){
                traffic_light_time ++ ;
            }
            else{
                Update_Vehicle_Speed(speed_time_count,averageSpeed);
                speed_time_count++;
            }
        }
        else{
            traffic_light_time = 0;
        }      **/



        EV << "simtime = "<<simTime()<<std::endl;

        for(int i=0;i<60;i++){
           total_speed_sum = total_speed_sum + vehicle_speed_store[i];
        }

        if(total_speed_sum ==0){                   //錯誤處零
           total_speed_sum = 1;
        }

        averageSpeed = total_speed_sum / 60;          //得到一段時間的平均速度


        double predictVehicle = Predict_Vehicle_Number(averageSpeed);
        int predictPacket = predictVehicle * 4.6;


        outFile<< simTime() << "," << totalVehicle << "," << averageSpeed << "," << total_packet_receive << ","  << predictVehicle << "," << predictPacket << "," <<total_packet_receive - predictPacket << "," <<totalVehicle - predictVehicle  << "," << rssi << std::endl;



        if(speed_time_count == 60){
           speed_time_count = 0;
        }

        packet_time_count++;
        if(packet_time_count == 5){          //一段時間packet_count歸零
          packet_time_count = 0;
        }

        total_speed_sum = 0;
        total_packet_receive = 0;
        packet_receive = 0;
        averageSpeed = 0;

        count = 0;
        speed_sum = 0;

        totalVehicle = 0;
    }

    scheduleAt(simTime()+ 1,test);      //統計時間間隔
}

void VehicleFlowRSU::finish(){
    DemoBaseApplLayer::finish();

    // statistics recording goes here
    outFile.close();
}

void VehicleFlowRSU::Update_Vehicle_Speed(int speed_time_count,double speed){
    vehicle_speed_store[speed_time_count] = averageSpeed;
}

double VehicleFlowRSU::Predict_Vehicle_Number(double v){
    return k_j * (1 - v / v_f);
}

double VehicleFlowRSU::Distance(double x,double y){
    x = PosX - x;
    y = PosY - y;
    x = pow(x,2);
    y = pow(y,2);
    double distance = sqrt(x + y);
    return distance;
}






