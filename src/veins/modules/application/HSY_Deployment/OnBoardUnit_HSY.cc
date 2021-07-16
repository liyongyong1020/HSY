/*
 * HSY_Deployment.cc
 *
 *  Created on: 2020Äê10ÔÂ13ÈÕ
 *      Author: artisan_tian
 */


#include <veins/modules/application/HSY_Deployment/OnBoardUnit_HSY.h>
using namespace std;

using namespace veins;

Define_Module(veins::OnBoardUnit_HSY);


OnBoardUnit_HSY::~OnBoardUnit_HSY()
{
    cancelAndDelete(beaconEvt);
}

void OnBoardUnit_HSY::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);

    if (stage == 0) {
        myIndex = getParentModule()->getIndex();
        beaconEvt = new cMessage("beaconEvt_HSY", SEND_BEACON_HSY);
//        stopPosition = 900+rand()%450;
        stopPosition = 1059;
        if(mobility->getId() == 718)
        {
            cout << stopPosition << endl;
        }

        EV << "car " << myIndex << " is initialized." << endl;

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

void OnBoardUnit_HSY::onWSM(BaseFrame1609_4* frame)
{
    // save for the further
}

void OnBoardUnit_HSY::handleSelfMsg(cMessage* msg)
{
//    DemoBaseApplLayer::handleSelfMsg(msg);

    switch (msg->getKind()) {
        case SEND_BEACON_HSY: {
            EV << "I am car " << myIndex << " : position " << curPosition.info() << \
                        " , speed " << " , " << mobility->getSpeed() << endl;
            EV << mobility->getId()<<endl;
            if(mobility->getId() == 718 && curPosition.x > stopPosition)
            {
                traciVehicle->setSpeed(0);
                traciVehicle->setColor(TraCIColor(255,0,0,255));
                traciVehicle->changeLane(2, 10000);

            }
            EV<< "Index " <<mobility->getId()<<endl;
            Beacon_HSY* beaconHSY = new Beacon_HSY();
            populateBeacon(beaconHSY);
            sendDown(beaconHSY);
//            if(mobility->getSpeed() == 0)
//            {
//                std::cout << "car[" << myIndex << "] had a problem" << endl;
//            }

            scheduleAt(simTime() + beaconInterval, beaconEvt);
            break;
        }
        default: {
            if (msg) EV_WARN << "APP: Error: Got Self Message of unknown kind! Name: " << msg->getName() << endl;
            break;
        }
    }
    // ADD should identify Cars and RSUs
//      std::string temp;
//      temp = "rsu[0]";
//      if(myName == temp)
//      {
//          EV << "I am rsu, and my position is " << curPosition << endl;
//      }
//      else{
//          EV << "I am vechile" << endl;
//      }
}

void OnBoardUnit_HSY::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);

}

void OnBoardUnit_HSY::handleLowerMsg(cMessage* msg)
{
    BaseFrame1609_4* frame = dynamic_cast<BaseFrame1609_4*>(msg);
    ASSERT(frame);

    if (Beacon_HSY* beacon = dynamic_cast<Beacon_HSY*>(frame)) {
        onBeacon(beacon);
    }
    else {
        // save for the further
    }

    delete (msg);
}

void OnBoardUnit_HSY::onBeacon(Beacon_HSY* beacon)
{
    EV << "received beacon from rsu " << beacon->getSenderIndex() << \
            " : position " << beacon->getSenderPos().info() << \
            " , speed " << beacon->getSenderSpeed() << endl;

    EV << "whole lane " << traciVehicle->getLaneId() << endl;
    std::string wholeLaneId = traciVehicle->getLaneId();
    LaneId = std::stoi(wholeLaneId.substr(3,1));
    EV << "lane " << LaneId << endl;
    /* process the information from VANET */



}

void OnBoardUnit_HSY::populateBeacon(BaseFrame1609_4* bf, LAddress::L2Type rcvId, int serial)
{
    bf->setRecipientAddress(rcvId);
    bf->setBitLength(headerLength);

    if (Beacon_HSY* beacon = dynamic_cast<Beacon_HSY*>(bf)) {
        // process beacon message
        beacon->setSenderIndex(myIndex);
        beacon->setSenderPos(curPosition);
        beacon->setSenderSpeed(mobility->getSpeed());
        beacon->setSenderLaneId(LaneId);
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


