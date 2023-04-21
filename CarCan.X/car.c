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
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

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
CARSTATE myCar;

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
    car->hours = 23;
    car->rpm = 0;
    car->minutes = 59;
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
    car->startStop = 0;
    car->count50Ms = 0;
    car->count1sec = 0;
    car->seconds = 30;
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
    static uint8_t lastVal = 0;
    if (lastVal != val) {
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
        lastVal = val;
    }
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

void sendPwrMotor(CARSTATE *car, uint8_t percentage, uint8_t starter) {
    static uint8_t lastPercent = 0;
    static uint8_t lastStarter = 0;
    if ((lastPercent != percentage) || (lastStarter != starter)) {
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
        lastPercent = percentage;
        lastStarter = starter;
    }
}

void sendPwrBreak(CARSTATE *car, uint8_t percentage) {
    static uint8_t lastPercent = 0;
    if (lastPercent != percentage) {
        CAN_TX_MSGOBJ txObj;
        uint8_t txd[1];
        txd[0] = percentage;
        txObj.bF.id.ID = (PWRBREAK << 4) + car->carId;
        txObj.bF.ctrl.DLC = CAN_DLC_1;
        txObj.bF.ctrl.RTR = 0;
        txObj.bF.id.SID11 = 0;
        txObj.bF.ctrl.FDF = 0;
        txObj.bF.ctrl.IDE = 0;
        txObj.bF.ctrl.BRS = 0;
        txObj.bF.ctrl.ESI = 0;
        CanSend(&txObj, txd);
        lastPercent = percentage;
    }
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

void startEngine(CARSTATE *car) {
    if (car->rpm == 0) {
        sendPwrMotor(car, 12, 1);
    } else {
        sendPwrMotor(car, 12, 0);
    }
    sendLightFront(car, 50);
    sendLightBack(car, 50);
    car->alreadyStarted = 1;
}

void resetCar(CARSTATE *car) {
    car->alreadyStarted = 0;
    sendLightBack(car, 0);
    sendLightFront(car, 0);
    sendPwrMotor(car, 0, 0);
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
    car->startStop = 0;
    car->count50Ms = 0;
    car->count1sec = 0;
}

void breakManagement(CARSTATE *car) {
    //Break mamagement
    sendPwrBreak(car, car->breakPedal);
    if (car->breakPedal > 20) {
        sendLightBack(car, 100);
    } else {
        sendLightBack(car, 50);
    }
}

void gazManagement(CARSTATE *car) {
    //Management of gas pedal
    if (car->rpm < 6500) {
        if (car->rpm != 0) {
            sendPwrMotor(car, MAX(car->accelPedal, 12), 0);
        } else {
            sendPwrMotor(car, MAX(car->accelPedal, 12), 1);
        }
    } else {
        sendPwrMotor(car, car->accelPedal - 20, 0);
    }
}

void compaereAndUptadeCar(CARSTATE *car) {

    if (car->contactKey == 1) {
        //Management of first start
        if (car->gearSelected == 'P') {
            breakManagement(car);
            if (car->alreadyStarted == 0) {
                startEngine(car);
            } else if (car->accelPedal > 20) {
                if (car->startStop == 1) {
                    sendPwrMotor(car, 12, 1);
                    car->startStop = 0;
                } else {
                    gazManagement(car);
                }
            } else {
                if (car->breakPedal > 20) {
                    sendPwrMotor(car, 0, 0);
                    car->startStop = 1;
                }
            }
        }

        if (car->gearSelected == 'N') {
            breakManagement(car);
            if (car->accelPedal > 10) {
                if (car->startStop == 1) {
                    sendPwrMotor(car, 12, 1);
                    car->startStop = 0;
                } else {
                    gazManagement(car);
                }
            } else {
                if ((car->speed < 5) && (car->breakPedal > 20)) {
                    sendPwrMotor(car, 0, 0);
                    car->startStop = 1;
                }
            }
        }

        if (car->gearSelected == 'R') {
            if ((car->gearLevel == 0) && (car->accelPedal > 20)) {
                if ((car->startStop == 1) && (car->rpm == 0)) {
                    sendPwrMotor(car, 12, 1);
                    car->startStop = 0;
                } else {
                    sendGearLevel(car, 1);
                }
            } else if (car->gearLevel != 0) {
                if (car->rpm > 5500) {
                    sendPwrMotor(car, 50, 0);
                } else {
                    sendPwrMotor(car, MAX(car->accelPedal, 12), 0);
                }
                breakManagement(car);
                if ((car->speed < 5) && (car->breakPedal > 20)) {
                    sendGearLevel(car, 0);
                    sendPwrMotor(car, 0, 0);
                    car->startStop = 1;
                }
            } else {
                sendPwrBreak(car, 100);
            }
        }

        //Gear management
        if (car->gearSelected == 'D') {
            if ((car->gearLevel == 0) && (car->accelPedal > 20)) {
                if ((car->startStop == 1)&& (car->rpm == 0)) {
                    sendPwrMotor(car, 12, 1);
                    car->startStop = 0;
                    sendPwrBreak(car, 0);
                } else {
                    sendGearLevel(car, 1);
                }
            } else if (car->gearLevel != 0) {
                if (car->tempomat == 1) {
                    if (car->accelPedal > 20) {
                        sendPwrMotor(car, car->accelPedal, 0);
                    } else {
                        if (car->speed > car->tempoSpeed) {
                            sendPwrMotor(car, 20, 0);
                        } else {
                            sendPwrMotor(car, 90, 0);
                        }
                        if (car->breakPedal > 20) {
                            sendTempoOff(car);
                        }
                    }

                } else {
                    if (car->speed > 270) {
                        sendPwrMotor(car, 50, 0);
                    } else {
                        sendPwrMotor(car, MAX(car->accelPedal, 12), 0);
                    }
                }

                if (car->breakPedal > 20) {
                    breakManagement(car);
                }

                if ((car->rpm > 5500) && (car->gearLevel != 5)) {
                    sendGearLevel(car, car->gearLevel + 1);
                } else if ((car->rpm < 2000) && (car->gearLevel != 1)) {
                    sendGearLevel(car, car->gearLevel - 1);
                } else if ((car->speed < 20) && (car->breakPedal > 20)) {
                    sendGearLevel(car, 0);
                    sendPwrMotor(car, 0, 0);
                    car->startStop = 1;
                }
            } else {
                sendPwrBreak(car, 100);
            }
        }
    } else if (car->alreadyStarted == 1) {
        resetCar(car);
    }
}

void calculateKm(CARSTATE *car){
    
}