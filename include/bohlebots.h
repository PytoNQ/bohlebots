#ifndef ZWAMBO_BOHLEBOTS_H
#define ZWAMBO_BOHLEBOTS_H

#include <Wire.h>
#include <Arduino.h>
#include <Pixy2I2C.h>
#include <elapsedMillis.h>


enum FARBE {
    AUS = 0,
    GRUEN,
    ROT,
    GELB,
    BLAU,
    CYAN,
    MAGENTA,
    WEISS
};

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

#define PIXY_ADDRESS 0x54
#define IR_ADDRESS 0x55
#define US_ADDRESS 0x56
#define COMPASS_ADDRESS 0x60


class TimeBasedIntChanger {
    unsigned long startTime = 0;
    int number = 0;
    int duration = 0;
    int goal = 0;
public:
    void change(int _goal, int _duration);

    void setDirectly(int _goal);

    int getNumber();

    void update();
};


class Motor {
public:
    Motor(int pin, int pwnChannel);

    void drive(int speed);


    void setSpeed(int speed);


private:
    TimeBasedIntChanger speedChanger;
    int pin;
    int pwnChannel;

};

class Bohlebots {
public:


    Bohlebots();

    void init();

    void updateBot();

    void wait(int ms);

    void drive(int direction, int speed);


    int getInput(int input);

    bool get_i2c_Button(int device, int button);

    bool getBoardButton(int button);

    void set_i2c_LED(int device, int nr, int color);

    void setBoardLED(int led, int color);

    void setCompassHeading();

    void setRotation(int _botRotation);


    int distances[4] = {0, 0, 0, 0};
    int ballDirection = 0;
    int ballDistance = 0;
    bool seesBall = false;
    bool hasBall = false;

    int goalDirection = 0;
    bool seesGoal = false;
    //int goalDistance = 0;

    int ownGoalDirection = 0;
    bool seesOwnGoal = false;
    //int ownGoalDistance = 0;

    int compassDirection = 0;


private:
    void setRGB(int r, int g, int b, int color);

    void sync_i2c_IO();

    void getIRData();

    void getUSData();

    void getCompassData();

    void getPixyData();


    int compassOffset = 0;

    elapsedMillis delayMillisTimer = 0;
    bool isCompassEnabled = false;
    bool isPixyEnabled = false;
    Pixy2I2C pixy;
    bool is_i2c_port_enabled[8] =
            {false, false, false, false, false, false, false, false};
    int i2c_button_adresses[8] =
            {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};

    bool _i2c_button1_array[8] = {false, false, false, false, false, false, false, false};
    bool _i2c_button2_array[8] = {false, false, false, false, false, false, false, false};
    int _i2c_led1_array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int _i2c_led2_array[8] = {0, 0, 0, 0, 0, 0, 0, 0};


    Motor motor1 = Motor(DRIVE1_DIR, 1); // rechts
    Motor motor2 = Motor(DRIVE2_DIR, 2); // hinten
    Motor motor3 = Motor(DRIVE3_DIR, 3); // links


    int botRotation = 0;


};


#endif //ZWAMBO_BOHLEBOTS_H
