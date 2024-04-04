//
// Created by antho on 17.03.24.
//

#ifndef ZWAMBO_PLAY_H
#define ZWAMBO_PLAY_H

#include <Wire.h>
#include <Arduino.h>
#include "bohlebots.h"

extern Bohlebots bot;

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


class Debug : public Strategy {
public:
    Debug() : Strategy() {}

private:
    void main() override;

    void disabledFunction() override;

    void firstCycleFunction() override;
};


#endif //ZWAMBO_PLAY_H
