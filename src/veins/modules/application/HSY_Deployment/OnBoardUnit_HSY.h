/*
 * HSY_Deployment.h
 *
 *  Created on: 2020年10月13日
 *      Author: artisan_tian
 *      补充注释，明确版权、说明功能
 */

#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

#include "veins/modules/application/HSY_Deployment/Beacon_HSY_m.h"


namespace veins {

/**
 * @brief
 *
 * the app module for vehicles.
 *
 */

class VEINS_API OnBoardUnit_HSY : public DemoBaseApplLayer {
public:
    ~OnBoardUnit_HSY();
    void initialize(int stage) override;

protected:
    cMessage* beaconEvt;

    enum OnBoardUnit {
            SEND_BEACON_HSY,
           };

    int myIndex;
    int LaneId;
    int stopPosition;

protected:
    void onWSM(BaseFrame1609_4* wsm) override;

    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;
    void handleLowerMsg(cMessage* msg) override;

    void onBeacon(Beacon_HSY* beacon);
    void populateBeacon(BaseFrame1609_4* wsm, LAddress::L2Type rcvId = LAddress::L2BROADCAST(), int serial = 0);

};

} // namespace veins
