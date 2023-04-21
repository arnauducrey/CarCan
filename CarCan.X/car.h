/* 
 * File:   car.h
 * Author: Arnaud
 *
 * Created on 28. mars 2023, 12:47
 */

#ifndef CAR_H
#define	CAR_H
#include "can.h"

    typedef struct  {
        uint16_t rpm;
        uint16_t speed;
        uint8_t tempomat;
        uint16_t tempoSpeed;
        char gearSelected;
        uint16_t frontSens;
        uint8_t sensLeft;
        uint8_t sensRight;
        uint8_t breakPedal;
        uint8_t accelPedal;
        uint8_t contactKey;
        int8_t steeringPosition;
        uint8_t steeringAuto;
        uint8_t startStop;
        uint8_t driveMode;
        uint8_t carId;
        uint8_t frontLight;
        uint8_t backLight;
        uint8_t hours;
        uint8_t minutes;
        uint8_t colon;
        uint8_t gearLevel;
        uint8_t motorVolume;
        uint8_t driveWheel;
        uint8_t alreadyStarted;
        uint8_t count1sec;
        uint8_t count50Ms;
        uint8_t seconds;
        
    } CARSTATE;
    extern CARSTATE myCar;
    void initialiseCar(CARSTATE *car);
    void updateCarstate(CARSTATE *car, CAN_RX_MSGOBJ rxObj);
    
    void sendLightFront(CARSTATE *car,uint8_t val);
    void sendLightBack(CARSTATE *car,uint8_t val);
    void sendTime(CARSTATE *car,uint8_t hour, uint8_t minute, uint8_t colon);
    void sendGearLevel(CARSTATE *car,uint8_t value);
    void sendPwrMotor(CARSTATE *car,uint8_t percentage, uint8_t starter);
    void sendPwrBreak(CARSTATE *car,uint8_t percentage);
    void sendTempoOff(CARSTATE *car);
    void sendKmPulse(CARSTATE *car);
    void sendAutoSteering(CARSTATE *car,int8_t value, uint8_t autoMode);
    void sendAudio(CARSTATE *car, uint8_t volume, uint8_t drive);
    void getFrontSens(CARSTATE *car);
    void getSteeringPosition(CARSTATE *car);
    
    void compaereAndUptadeCar(CARSTATE *car);
    void startEngine(CARSTATE *car);
    void resetCar(CARSTATE *car);
    void breakManagement(CARSTATE *car);
    void gazManagement(CARSTATE *car);
    void calculateKm(CARSTATE *car);

#endif