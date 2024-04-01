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
    if (abs(bot.ballDirection) >= 3) { // Move behind ball with compass
        moveBehindBall();
    }
    int degreeBallDirection = bot.ballDirection * 360 / 16;
    // Absolute ballDirection adjusted for bot rotation
    int adjustedBallDirection = ((degreeBallDirection + bot.compassDirection + 180 + 360) % 360) - 180;
    

}

void Play::moveBehindBall() {

}



void Anstoss::main() {

}

void Anstoss::runFirstCycle() {

}

void Debug::main() {

}

void Debug::runFirstCycle() {

}