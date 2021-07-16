/*
 * HSY_Deployment.h
 *
 *  Created on: 2020年10月16日
 *      Author: artisan_wen
 *      补充注释，明确版权、说明功能
 */

#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/base/utils/Coord.h"
#include "veins/modules/application/HSY_Deployment/Beacon_HSY_m.h"
#include <map>
#include <string>
#include <vector>

using namespace std;

namespace veins {

/**
 * @brief
 *
 * the app module for rsu.
 *
 */
typedef struct NeighborStruct {
    string name;
    Coord pos;
    double speed;
    int lane;
};


class VEINS_API RoadSideDevice_HSY : public DemoBaseApplLayer {
public:
    ~RoadSideDevice_HSY();
    void initialize(int stage) override;

protected:
    cMessage* beaconEvt;

    enum RoadSideDevice {
            SEND_BEACON_HSY,
           };

    int myIndex;
    std::string fileName;

    //flow
    int flow_HSY = 0;
    int temp = 0;

    //occupancy
    float occupancy_HSY = 0.0;

    //speed
    float speed_HSY = 0.0;
    double count = 0;
    float sumSpeed_HSY = 0.0;
protected:
    void onWSM(BaseFrame1609_4* wsm) override;

    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;
    void handleLowerMsg(cMessage* msg) override;

    void onBeacon(Beacon_HSY* beacon);
    void populateBeacon(BaseFrame1609_4* wsm, LAddress::L2Type rcvId = LAddress::L2BROADCAST(), int serial = 0);

private:
    typedef map<int , NeighborStruct> NeighborListMap;
    NeighborListMap neighborListMap;
    NeighborStruct neighborInfo;
};

} // namespace veins
