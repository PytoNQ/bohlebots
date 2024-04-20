//
// Created by antho on 17.03.24.
//

#ifndef ZWAMBO_PLAY_H
#define ZWAMBO_PLAY_H

#include <Wire.h>
#include <Arduino.h>
#include "bohlebots.h"
#include "espOTA.h"
#import "ncSerial.h"

#define ENABLE_WIFI false

extern Bohlebots bot;
extern espOTA ota;
extern ncSerial nc;

int getSign(int num);


class Strategy {
public:
    void run(bool runFirstCycle, bool isEnabled);

private:
    virtual void main() = 0;

    virtual void disabledFunction() = 0;

    virtual void firstCycleFunction() = 0;
};


class Idle : public Strategy {
public:
    Idle() : Strategy() {}


private:
    void main() override;

    void disabledFunction() override;

    void firstCycleFunction() override;
};


class Play : public Strategy {
public:
    Play() : Strategy() {}

private:
    void main() override;

    void disabledFunction() override;

    void firstCycleFunction() override;

    void tryGetBall();

    void moveBehindBall();


    void moveToOwnGoal();

    void playOffensive();

    void checkLackOfProgress();

    bool isLackOfProgress = false;

    bool lastBallPossession = false;
    elapsedMillis timeSinceLastBallPossessionChange = 0;

    int lastBallDirection = 0;
    elapsedMillis timeSinceLastBallDirectionChange = 0;

    int lastCompassDirection = 0;
    elapsedMillis timeSinceLastCompassDirectionChange = 0;
    int compassNoiseLimit = 3;

    int lastDistances[4] = {0, 0, 0, 0};
    elapsedMillis timeSinceLastDistanceChange[4] = {0, 0, 0, 0};
    int distanceNoiseLimit = 5;

    int corner = 0;
    elapsedMillis cornerTimer = 0;


    void evaluateSensorData();

    int approx_x; //range from -2 to 2; far-left to far-right
    int approx_y; //range from -2 to 2; far-back to far-front
    void executeCorners();

    void freeLackOfProgress();
};


class Anstoss : public Strategy {
public:
    explicit Anstoss(int dir) : Strategy() { this->direction = dir; }

private:
    void main() override;

    void disabledFunction() override;

    void firstCycleFunction() override;

    int direction = 0;
};

class i2cScanner : public Strategy {
public:
    i2cScanner() : Strategy() {}

private:
    void main() override;

    void disabledFunction() override;

    void firstCycleFunction() override;

};

class motorTest : public Strategy {
public:
    motorTest() : Strategy() {}

private:
    void main() override;

    void disabledFunction() override;

    void firstCycleFunction() override;

};

class Debug : public Strategy {
public:
    Debug() : Strategy() {}

private:
    void main() override;

    void disabledFunction() override;

    void firstCycleFunction() override;

};


#endif //ZWAMBO_PLAY_H
