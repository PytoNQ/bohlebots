#include "play.h"

Bohlebots bot;

int getSign(int num) {
    return num < 0 ? -1 : 1;
}

void Strategy::run() {
    if (!this->isEnabled && this->canBeDisabled) { return; }

    if (this->isFirstCycle) {
        this->runFirstCycle();
        this->isFirstCycle = false;
    }
    this->main();

}

void Strategy::setEnabled(bool _isEnabled) {
    this->isEnabled = _isEnabled;
    if (!_isEnabled) {
        this->isFirstCycle = true;
    }
}

void Idle::main() {
    if (!bot.getBoardButton(3)) {
        bot.set_i2c_LED(1, 1, 0);
        bot.set_i2c_LED(1, 2, 0);
        return;
    }
    if (bot.seesBall) {
        bot.set_i2c_LED(1, 1, BLAU);
    } else {
        bot.set_i2c_LED(1, 1, MAGENTA);
    }
    if (bot.seesGoal) {
        bot.set_i2c_LED(1, 2, ROT);
    } else if (bot.seesOwnGoal) {
        bot.set_i2c_LED(1, 2, GRUEN);
    } else {
        bot.set_i2c_LED(1, 2, MAGENTA);
    }
}

void Idle::runFirstCycle() {

}

void Play::main() {

    if (!bot.seesBall) {
        return;
    }
    if (!bot.hasBall) {
        tryGetBall();
    }
}

void Play::runFirstCycle() {
    bot.set_i2c_LED(1, 1, BLAU);
    bot.setCompassHeading();
}

void Play::tryGetBall() {
    if (abs(bot.ballDirection) >= 4) { // Move behind ball with compass
        moveBehindBall();
    }
}

void Play::moveBehindBall() {
    int ballDirection = abs(bot.ballDirection);
    int direction = 0;
    int speed = 0;
    if (ballDirection >= 4 && ballDirection <= 7) {
        direction = 180;
        speed = 100;
    }
    if (ballDirection == 8) {
        int sign = getSign(bot.ballDirection); //TODO check for walls in the way etc
        speed = 100;
        direction = 120 * sign;
    }
    int rotation = -bot.compassDirection; //TODO test rotation modifiers

    bot.drive(direction, speed, rotation);

}



void Anstoss::main() {

}

void Anstoss::runFirstCycle() {

}

void Debug::main() {

}

void Debug::runFirstCycle() {

}