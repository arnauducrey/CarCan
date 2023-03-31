/*
 * File:   car.c
 * Author: Arnaud
 *
 * Created on 28. mars 2023, 12:51
 */


#include <xc.h>
#include "car.h"
#include "can_defines.h"
#include "can.h" 

#define LIGHTFRONT 0x11
#define LIGHTBACK 0x12
#define TIME 0x13
#define GEARLEVEL 0x14
#define PWRMOTOR 0x16
#define PWRBREAK 0x17
#define TEMPOOFF 0x18
#define PULSE 0x19
#define AUTOSTEERING 0x1A
#define VOLUME 0x15
#define FRONTSENS 0x03
#define STEERINGPOS 0x09

void initialiseCar(CARSTATE *car) {
    car->accelPedal = 0;
    car->backLight = 0;
    car->breakPedal = 0;
    car->carId = 0;
    car->colon = 1;
    car->contactKey = 0;
    car->driveMode = 0;
    car->frontLight = 0;
    car->frontSens = 0;
    car->gearLevel = 0;
    car->gearSelected = 'P';
    car->hours = 0;
    car->rpm = 0;
    car->minutes = 0;
    car->sensLeft = 0;
    car->sensRight = 0;
    car->speed = 0;
    car->steeringPosition = 0;
    car->tempoSpeed = 0;
    car->tempomat = 0;
    car->steeringAuto = 0;
    car->motorVolume = 0;
    car->driveWheel = 1;
    car->alreadyStarted = 0;
    car->accelPedalHold = 0;
}

void updateCarstate(CARSTATE *car, CAN_RX_MSGOBJ rxObj) {
    uint8_t rxd[8];
    if (CanReceive(&rxObj, rxd) == 0) {
        switch (rxObj.bF.id.ID >> 4) {
            case 0x01: //tempomat
                car->tempomat = rxd[0];
                car->tempoSpeed = rxd[1];
                break;
            case 0x02: //gear Selected
                car->gearSelected = rxd[0];
                break;
            case 0x03: //DriveMode or front sensor
                if (car->driveMode == 0) {
                    uint16_t data;
                    data = rxd[0];
                    data = data << 8;
                    data += rxd[1];
                    car->frontSens = data;
                } else {
                    car->sensLeft = rxd[0];
                    car->sensRight = rxd[1];
                }
                break;
            case 0x04: //motor status
            {
                uint16_t data;
                data = rxd[0];
                data = data << 8;
                data += rxd[1];
                car->rpm = data;
                data = 0;
                data = rxd[2];
                data = data << 8;
                data += rxd[3];
                car->speed = data;
            }
                break;
            case 0x06: //break pedal
                car->breakPedal = rxd[0];
                break;
            case 0x07: //accel pedal
                car->accelPedal = rxd[0];
                break;
            case 0x08: //contact key
                car->contactKey = rxd[0];
                break;
            case 0x09: //steering position
                car->steeringPosition = rxd[0];
                break;
            case 0x0D: //Drive mode
                car->driveMode = rxd[0];
                break;
            default:
                break;
        }
    }
}

void sendLightFront(CARSTATE *car, uint8_t val) {
        CAN_TX_MSGOBJ txObj;
        uint8_t txd[1] = {val};
        txObj.bF.id.ID = (LIGHTFRONT << 4) + car->carId;
        txObj.bF.ctrl.DLC = CAN_DLC_1;
        txObj.bF.ctrl.RTR = 0;
        txObj.bF.id.SID11 = 0;
        txObj.bF.ctrl.FDF = 0;
        txObj.bF.ctrl.IDE = 0;
        txObj.bF.ctrl.BRS = 0;
        txObj.bF.ctrl.ESI = 0;
        CanSend(&txObj, txd);
        car->frontLight = val;
}

void sendLightBack(CARSTATE *car, uint8_t val) {
        CAN_TX_MSGOBJ txObj;
        uint8_t txd[1] = {val};
        txObj.bF.id.ID = (LIGHTBACK << 4) + car->carId;
        txObj.bF.ctrl.DLC = CAN_DLC_1;
        txObj.bF.ctrl.RTR = 0;
        txObj.bF.id.SID11 = 0;
        txObj.bF.ctrl.FDF = 0;
        txObj.bF.ctrl.IDE = 0;
        txObj.bF.ctrl.BRS = 0;
        txObj.bF.ctrl.ESI = 0;
        CanSend(&txObj, txd);
        car->backLight = val;
}

void sendTime(CARSTATE *car, uint8_t hour, uint8_t minute, uint8_t colon) {
    CAN_TX_MSGOBJ txObj;
    uint8_t txd[3] = {hour, minute, colon};
    txObj.bF.id.ID = (TIME << 4) + car->carId;
    txObj.bF.ctrl.DLC = CAN_DLC_3;
    txObj.bF.ctrl.RTR = 0;
    txObj.bF.id.SID11 = 0;
    txObj.bF.ctrl.FDF = 0;
    txObj.bF.ctrl.IDE = 0;
    txObj.bF.ctrl.BRS = 0;
    txObj.bF.ctrl.ESI = 0;
    CanSend(&txObj, txd);
    car->hours = txd[0];
    car->minutes = txd[1];
    car->colon = txd[2];
}

void sendGearLevel(CARSTATE *car, uint8_t value) {
        CAN_TX_MSGOBJ txObj;
        uint8_t txd[1] = {value};
        txObj.bF.id.ID = (GEARLEVEL << 4) + car->carId;
        txObj.bF.ctrl.DLC = CAN_DLC_1;
        txObj.bF.ctrl.RTR = 0;
        txObj.bF.id.SID11 = 0;
        txObj.bF.ctrl.FDF = 0;
        txObj.bF.ctrl.IDE = 0;
        txObj.bF.ctrl.BRS = 0;
        txObj.bF.ctrl.ESI = 0;
        CanSend(&txObj, txd);
        car->gearLevel = value;
}

void sendPwrMotor(CARSTATE *car, CARSTATE *oldCar, uint8_t percentage, uint8_t starter) {
    if ((car->accelPedal != oldCar->accelPedal) || (car->alreadyStarted == 0) || (car->accelPedalHold == 1)) {
        CAN_TX_MSGOBJ txObj;
        uint8_t txd[2] = {percentage, starter};
        txObj.bF.id.ID = (PWRMOTOR << 4) + car->carId;
        txObj.bF.ctrl.DLC = CAN_DLC_2;
        txObj.bF.ctrl.RTR = 0;
        txObj.bF.id.SID11 = 0;
        txObj.bF.ctrl.FDF = 0;
        txObj.bF.ctrl.IDE = 0;
        txObj.bF.ctrl.BRS = 0;
        txObj.bF.ctrl.ESI = 0;
        CanSend(&txObj, txd);
        car->accelPedal = percentage;
    }
}

void sendPwrBreak(CARSTATE *car, uint8_t percentage) {
        CAN_TX_MSGOBJ txObj;
        uint8_t txd[1] = {percentage};
        txObj.bF.id.ID = (PWRBREAK << 4) + car->carId;
        txObj.bF.ctrl.DLC = CAN_DLC_1;
        txObj.bF.ctrl.RTR = 0;
        txObj.bF.id.SID11 = 0;
        txObj.bF.ctrl.FDF = 0;
        txObj.bF.ctrl.IDE = 0;
        txObj.bF.ctrl.BRS = 0;
        txObj.bF.ctrl.ESI = 0;
        CanSend(&txObj, txd);
        car->breakPedal = percentage;
}

void sendTempoOff(CARSTATE *car) {
    CAN_TX_MSGOBJ txObj;
    uint8_t txd[1];
    txObj.bF.id.ID = (TEMPOOFF << 4) + car->carId;
    txObj.bF.ctrl.DLC = CAN_DLC_0;
    txObj.bF.ctrl.RTR = 0;
    txObj.bF.id.SID11 = 0;
    txObj.bF.ctrl.FDF = 0;
    txObj.bF.ctrl.IDE = 0;
    txObj.bF.ctrl.BRS = 0;
    txObj.bF.ctrl.ESI = 0;
    CanSend(&txObj, txd);
    car->tempomat = 0;
}

void sendKmPulse(CARSTATE *car) {
    CAN_TX_MSGOBJ txObj;
    uint8_t txd[1];
    txObj.bF.id.ID = (PULSE << 4) + car->carId;
    txObj.bF.ctrl.DLC = CAN_DLC_0;
    txObj.bF.ctrl.RTR = 0;
    txObj.bF.id.SID11 = 0;
    txObj.bF.ctrl.FDF = 0;
    txObj.bF.ctrl.IDE = 0;
    txObj.bF.ctrl.BRS = 0;
    txObj.bF.ctrl.ESI = 0;
    CanSend(&txObj, txd);
}

void sendAutoSteering(CARSTATE *car, int8_t value, uint8_t autoMode) {
    CAN_TX_MSGOBJ txObj;
    uint8_t txd[2] = {value, autoMode};
    txObj.bF.id.ID = (AUTOSTEERING << 4) + car->carId;
    txObj.bF.ctrl.DLC = CAN_DLC_2;
    txObj.bF.ctrl.RTR = 0;
    txObj.bF.id.SID11 = 0;
    txObj.bF.ctrl.FDF = 0;
    txObj.bF.ctrl.IDE = 0;
    txObj.bF.ctrl.BRS = 0;
    txObj.bF.ctrl.ESI = 0;
    CanSend(&txObj, txd);
    car->steeringPosition = value;
    car->steeringAuto = autoMode;
}

void sendAudio(CARSTATE *car, uint8_t volume, uint8_t drive) {
    CAN_TX_MSGOBJ txObj;
    uint8_t txd[2] = {volume, drive};
    txObj.bF.id.ID = (VOLUME << 4) + car->carId;
    txObj.bF.ctrl.DLC = CAN_DLC_2;
    txObj.bF.ctrl.RTR = 0;
    txObj.bF.id.SID11 = 0;
    txObj.bF.ctrl.FDF = 0;
    txObj.bF.ctrl.IDE = 0;
    txObj.bF.ctrl.BRS = 0;
    txObj.bF.ctrl.ESI = 0;
    CanSend(&txObj, txd);
    car->driveWheel = drive;
    car->motorVolume = volume;
}

void getFrontSens(CARSTATE *car) {
    CAN_TX_MSGOBJ txObj;
    uint8_t txd[1];
    txObj.bF.id.ID = (FRONTSENS << 4) + car->carId;
    txObj.bF.ctrl.DLC = CAN_DLC_0;
    txObj.bF.ctrl.RTR = 1;
    txObj.bF.id.SID11 = 0;
    txObj.bF.ctrl.FDF = 0;
    txObj.bF.ctrl.IDE = 0;
    txObj.bF.ctrl.BRS = 0;
    txObj.bF.ctrl.ESI = 0;
    CanSend(&txObj, txd);
}

void getSteeringPosition(CARSTATE *car) {
    CAN_TX_MSGOBJ txObj;
    uint8_t txd[1];
    txObj.bF.id.ID = (STEERINGPOS << 4) + car->carId;
    txObj.bF.ctrl.DLC = CAN_DLC_0;
    txObj.bF.ctrl.RTR = 1;
    txObj.bF.id.SID11 = 0;
    txObj.bF.ctrl.FDF = 0;
    txObj.bF.ctrl.IDE = 0;
    txObj.bF.ctrl.BRS = 0;
    txObj.bF.ctrl.ESI = 0;
    CanSend(&txObj, txd);
}

void startEngine(CARSTATE *car, CARSTATE *oldCar) {
    if (car->rpm == 0) {
        sendPwrMotor(car, oldCar, 12, 1);
    } else {
        sendPwrMotor(car, oldCar, 12, 0);
    }
    sendLightFront(car, 50);
    sendLightBack(car, 50);
    car->alreadyStarted = 1;
}

void resetCar(CARSTATE *car, CARSTATE *oldCar) {
    car->alreadyStarted = 0;
    sendLightBack(car, 0);
    sendLightFront(car, 0);
    sendPwrMotor(car, oldCar, 0, 0);
    car->accelPedal = 0;
    car->backLight = 0;
    car->breakPedal = 0;
    car->colon = 1;
    car->contactKey = 0;
    car->driveMode = 0;
    car->frontLight = 0;
    car->frontSens = 0;
    car->gearLevel = 0;
    car->gearSelected = 'P';
    car->hours = 0;
    car->rpm = 0;
    car->minutes = 0;
    car->sensLeft = 0;
    car->sensRight = 0;
    car->speed = 0;
    car->steeringPosition = 0;
    car->tempoSpeed = 0;
    car->tempomat = 0;
    car->steeringAuto = 0;
    car->motorVolume = 0;
    car->driveWheel = 1;
    car->accelPedalHold = 0;
}

void compaereAndUptadeCar(CARSTATE *car, CARSTATE *oldCar) {
    /*if (car->contactKey == 1) {
        //Detection of the car turning on
        if (oldCar->contactKey == 0) {
            startEngine(car, oldCar);
        } /*else {

            //Management of gas pedal
            if (car->accelPedal == oldCar->accelPedal) {
                car->accelPedalHold = 1;
                if (car->rpm < 6500) {
                    if (car->rpm != 0) {
                        sendPwrMotor(car, oldCar, car->accelPedal, 0);
                    } else {
                        sendPwrMotor(car, oldCar, car->accelPedal, 1);
                    }
                } else {
                    sendPwrMotor(car, oldCar, car->accelPedal / 2, 0);
                }
            } else {
                car->accelPedalHold = 0;
            }
        }
        //Management of break pedal
        /*if(car->breakPedal =! oldCar->breakPedal){
            sendPwrMotor(car,car->breakPedal, 0);
        }

    } else if (oldCar->contactKey == 1) { //Detection of the car turning off
        resetCar(car,oldCar);
    }
     */       
    if(car->contactKey == 1){
        if(car->alreadyStarted == 0){
            startEngine(car, oldCar);
        } else {
            //Management of gas pedal
            if ((car->accelPedal == oldCar->accelPedal) && (car->accelPedal != 0)) { //CE IF FAIT TOUT PLANTER
                car->accelPedalHold = 1;
                if (car->rpm < 6500) {
                    if (car->rpm != 0) {
                        sendPwrMotor(car, oldCar, car->accelPedal, 0);
                    } else {
                        sendPwrMotor(car, oldCar, car->accelPedal, 1);
                    }
                } else {
                    sendPwrMotor(car, oldCar, car->accelPedal / 2, 0);
                }
            } else {
                car->accelPedalHold = 0;
            }
        }
    } else if(car->alreadyStarted == 1) {
        resetCar(car,oldCar);
    }
    *oldCar = *car;
}