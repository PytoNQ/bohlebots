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
    Strategy() : isEnabled(false), isFirstCycle(true), canBeDisabled(true) {}

    void run();

    virtual void setEnabled(bool _isEnabled);

protected:
    bool canBeDisabled = true;
    bool isEnabled = false;
    bool isFirstCycle = true;
private:
    virtual void main() = 0;

    virtual void runFirstCycle() = 0;
};


class Idle : public Strategy {
public:
    Idle() : Strategy() {
        this->canBeDisabled = false;
    }


private:
    void main() override;

    void runFirstCycle() override;
};


class Play : public Strategy {
public:
    Play() : Strategy() {}

private:
    void main() override;

    void runFirstCycle() override;

    void tryGetBall();

    int calculateGoalRotation(int drehMod);


};


class Anstoss : public Strategy {
public:
    explicit Anstoss(int dir) : Strategy() { this->direction = dir; }

private:
    void main() override;

    void runFirstCycle() override;

    int direction = 0;
};


class Debug : public Strategy {
public:
    Debug() : Strategy() {
        this->canBeDisabled = false;
    }

private:
    void main() override;

    void runFirstCycle() override;
};


#endif //ZWAMBO_PLAY_H
