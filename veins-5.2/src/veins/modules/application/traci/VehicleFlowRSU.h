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

#pragma once

#include "veins/veins.h"
#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/messages/BSMMessage_m.h"
#include <math.h>


namespace veins {

/**
 * Small RSU Demo using 11p
 */
class VEINS_API VehicleFlowRSU : public  DemoBaseApplLayer {
public:
    double v;    // 當前車速 (公里/小時)
    double v_f = 50;  // 自由流速 (公里/小時)
    //double k_j = 55;  // 擁擠密度 (輛/公里)
    double k_j = 70;
protected:
    void initialize(int stage) override;
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;
    void handleLowerMsg(cMessage* msg) override;
    void handleSelfMsg(cMessage* msg) override;
    void finish() override;

    void Update_Vehicle_Speed(int speed_time_count,double speed);
    double Predict_Vehicle_Number(double v);
    double Distance(double x,double y);

    double PosX = 25;
    double PosY = 200;

    int totalVehicle = 0;
    int count=0;
    double speed_sum=0;           //每秒鐘速度和
    double total_speed_sum = 0;   //一段時間速度和

    double averageSpeed = 0;      //每秒鐘平均速度以及一段時間平均速度

    int receiveNumber = 0;
    int packet_time_count = 0;
    int packet_receive = 0;
    int total_packet_receive = 0;
    int speed_time_count = 0;
    double rssi = 0;

    int packet_receive_store[5];
    double vehicle_speed_store[60];
    int differ_pseudo[50] = {0};
    int traffic_light_time = 0;

    std::string fileName;
    std::ofstream outFile;

    cMessage* test;
};

}
