/*
 * RoadSideDevice.cc
 *
 *  Created on: 2020年10月16日
 *      Author: wen
 */

#include <veins/modules/application/HSY_Deployment/RoadSideDevice_HSY.h>


using namespace std;
using namespace veins;

Define_Module(veins::RoadSideDevice_HSY);

RoadSideDevice_HSY::~RoadSideDevice_HSY()
{
    cancelAndDelete(beaconEvt);
}

void RoadSideDevice_HSY::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);

    if (stage == 0) {
        myIndex = getParentModule()->getIndex();

        beaconEvt = new cMessage("beaconEvt_HSY", SEND_BEACON_HSY);



        EV << "rsu " << myIndex << " is initialized." << endl;
        string myName = getParentModule()->getFullName();
        EV << "getId: " << getParentModule()->getId() << endl;
//        EV << myName << " is initialized." << endl;
        string fileType = ".csv";
        fileName = myName + fileType;
        ofstream outFile(fileName,std::ofstream::app|std::ofstream::ate);
        outFile << "time" << "," << "flow" << "," << "occupancy" << "," << "speed" << endl;
        outFile.close();
//        EV << "=====================================================================";

    }
//    else if (stage == 1) {
//        simtime_t firstBeacon = simTime();
//
//        if (par("avoidBeaconSynchronization").boolValue() == true) {
//
//            EV << "avoidBeaconSynchronization is true." << endl;
//            simtime_t randomOffset = dblrand() * beaconInterval;
//            firstBeacon = simTime() + randomOffset;
//
//            if (sendBeacons) {
//                scheduleAt(firstBeacon, beaconEvt);
//            }
//        }
//    }
}

void RoadSideDevice_HSY::onWSM(BaseFrame1609_4* frame)
{
    // save for the further
}

void RoadSideDevice_HSY::handleSelfMsg(cMessage* msg)
{
//    DemoBaseApplLayer::handleSelfMsg(msg);


    switch (msg->getKind()) {
        case SEND_BEACON_HSY: {

            simTime().dbl();
            speed_HSY = sumSpeed_HSY/count;
            flow_HSY = temp/simTime()*3600;
            occupancy_HSY = 64/speed_HSY;
//            cout << " time " << simTime() << " rsu " << myIndex << " position " << curPosition.x <<" speed: " << speed_HSY << " flow: " << flow_HSY << " occupancy: "<< occupancy_HSY << endl;
            sumSpeed_HSY = 0;
            count = 0;

            scheduleAt(simTime() + beaconInterval, beaconEvt);
            ofstream outFile(fileName,std::ofstream::app|std::ofstream::ate);
            outFile << simTime().dbl() << ","<< flow_HSY << "," << occupancy_HSY << "," << speed_HSY << endl;
            outFile.close();
            break;
        }
    }


}

void RoadSideDevice_HSY::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);

}

void RoadSideDevice_HSY::handleLowerMsg(cMessage* msg)
{
    BaseFrame1609_4* frame = dynamic_cast<BaseFrame1609_4*>(msg);
    ASSERT(frame); //断言，如果返回FALSE则终止程序

    if (Beacon_HSY* beacon = dynamic_cast<Beacon_HSY*>(frame)) {
        onBeacon(beacon);
    }
    else {
        // save for the further
    }
    delete (msg);
}

void RoadSideDevice_HSY::onBeacon(Beacon_HSY* beacon)   //通过这个函数收集邻居节点的信息存到本地邻居节点信息集内
{
    EV << "i am rsu , received beacon from car " << beacon->getSenderIndex() << \
            " : position " << beacon->getSenderPos().info().data() << \
            " , speed " << beacon->getSenderSpeed() << endl;
    neighborInfo.pos = beacon->getSenderPos();
    neighborInfo.speed = beacon->getSenderSpeed();
    neighborInfo.lane = beacon->getSenderLaneId();
    findHost()->getDisplayString().updateWith("r=75,green");

    /* process the information from VANET */
    //Delete Info
    if(!neighborListMap.empty()){
     for(NeighborListMap::iterator it=neighborListMap.begin(); it != neighborListMap.end(); it++) {
        std::map<int,NeighborStruct>::iterator delete_id;
        if(neighborListMap.find(beacon->getSenderIndex()) != neighborListMap.end())
        {
            //说明id存在于neighborListMap;
        delete_id = neighborListMap.find(beacon->getSenderIndex());
            neighborListMap.erase(delete_id);
                }
            }
        }

    // insert NeighborList


    neighborListMap.insert(make_pair(beacon->getSenderIndex(), neighborInfo));

    double RSUposition = curPosition.x;   //rsu的位置
    double range = 75;


    //Delete information beyond the communication range
    if(!neighborListMap.empty()){
             for(NeighborListMap::iterator it=neighborListMap.begin(); it != neighborListMap.end(); it++) {
                std::map<int,NeighborStruct>::iterator delete_name;
          //      if(neighborInfo.pos.x - 1000.000 > 800)

                if((RSUposition - it->second.pos.x) > range || it->second.pos.x - RSUposition > 20)
                {
                    //说明id存在于neighborListMap;
                    delete_name = neighborListMap.find(it->first);
                    neighborListMap.erase(delete_name);
                }
            }
        }

    EV << "This rsu name is : rsu " << myIndex << " posx is " << RSUposition << endl;
    EV << "My Neighbor List " << "( " << neighborListMap.size() << " neighbors ) :" << endl;
    for(NeighborListMap::iterator it=neighborListMap.begin(); it != neighborListMap.end(); it++) {




        EV << "Neighbor ID: " << it->first << " , " << "The Neighbor Name: car[" << it->first << "] , " << "The position: " << it->second.pos << " , " << "The speed: " << it->second.speed << "laneid: "<< it->second.lane << endl;
        if((RSUposition - it->second.pos.x) < range && (RSUposition - it->second.pos.x) > 20){
//            EV << "valid position : " << it->second.pos.x << "valid speed : " << it->second.speed << endl;
            if(it->second.speed == 0)
            {
//                cout << "car[" << it->first << "] had a problem " << endl;
                findHost()->getDisplayString().updateWith("r=40,red");  //原来是80
            }
            //speed
            sumSpeed_HSY += it->second.speed;
            count++;
            if(it->first > temp){
                temp = it->first;
            }

        }
    }
}
void RoadSideDevice_HSY::populateBeacon(BaseFrame1609_4* bf, LAddress::L2Type rcvId, int serial)      //using L2Type = long;
{
    bf->setRecipientAddress(rcvId);
    bf->setBitLength(headerLength);

    if (Beacon_HSY* beacon = dynamic_cast<Beacon_HSY*>(bf)) {
        // process beacon message
        beacon->setSenderIndex(myIndex);
        beacon->setSenderPos(curPosition);
//        beacon->setSenderSpeed(curSpeed);
        beacon->setChannelNumber(static_cast<int>(Channel::cch));
        beacon->addBitLength(beaconLengthBits);
        bf->setUserPriority(beaconUserPriority);
    }
    else {
        // process data message
        if (dataOnSch)
            bf->setChannelNumber(static_cast<int>(Channel::sch1)); // will be rewritten at Mac1609_4 to actual Service Channel. This is just so no controlInfo is needed
        else
            bf->setChannelNumber(static_cast<int>(Channel::cch));
        bf->addBitLength(dataLengthBits);
        bf->setUserPriority(dataUserPriority);
    }
}

