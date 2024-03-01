#ifndef ZWAMBO_BOHLEBOTS_H
#define ZWAMBO_BOHLEBOTS_H

#include <Wire.h>
#include <Arduino.h>
#include <Pixy2I2C.h>
#include <elapsedMillis.h>


#define AUS 0
#define GRUEN 1
#define ROT 2
#define GELB 3
#define BLAU 4
#define CYAN 5
#define MAGENTA 6
#define WEISS 7

#define led1r 5
#define led1g 23
#define led1b 16

#define led2r 4
#define led2g 15
#define led2b 13


#define SDA 21
#define SCL 22

#define DRIVE_DIS 27
#define DRIVE1_PWM 18
#define DRIVE1_DIR 19
#define DRIVE2_PWM 14
#define DRIVE2_DIR 12
#define DRIVE3_PWM 25
#define DRIVE3_DIR 26


#define INPUT1 35
#define INPUT2 34
#define INPUT3 39
#define INPUT4 36

#define kicker 17
#define dribbler 2
#define IR_ADDRESS 0x55
#define KOMPASS_ADRESSE 0x60 // cmps11, cmps12, cmps14

#define ANGLE_8 1

class Motor {
public:
    Motor(int pin, int pwnChannel);

    void drive(int speed);

    void updateMotorSpeed();

private:
    void setSpeed(int speed);

    int currentNominalSpeed = 0;
    int currentSpeed = 0;
    int lastNominalSpeed = 0;
    int pin;
    int pwnChannel;
};

class Bohlebots {
public:
    int ballDirection = 0;
    int ballDistance = 0;
    bool seesBall = false;
    bool hasBall = false;

    int goalDirection = 0;
    int goalDistance = 0;
    bool seesGoal = false;

    int ownGoalDirection = 0;
    int ownGoalDistance = 0;
    bool seesOwnGoal = false;

    Bohlebots();

    void init();

    void wait(int ms);

    void drive(int direction, int speed, int rotation);

    void ena(bool isEna); // TODO verstehen + portena usw

    int getInput(int input);

    bool getButton(int device, int button);

    void setLED(int device, int color);

private:
    int speedToPWM(int speed);

    void getSensorData();

    void getPixyData();

    elapsedMillis delayMillisTimer = 0;


};


#endif //ZWAMBO_BOHLEBOTS_H
