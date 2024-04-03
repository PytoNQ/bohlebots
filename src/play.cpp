#include "play.h"

Bohlebots bot;


void Strategy::run(bool runFirstCycle, bool isEnabled) {
    if (!isEnabled) { // If the strategy is not enabled, run the disabled function
        this->disabledFunction();
        return;
    }

    if (runFirstCycle) {
        this->firstCycleFunction();
    }
    this->main();

}



void Idle::main() {
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

void Idle::disabledFunction() {
    bot.set_i2c_LED(1, 1, 0);
    bot.set_i2c_LED(1, 2, 0);
}

void Idle::firstCycleFunction() {

}

void Play::main() {
    if (!bot.seesBall) {
        return;
    }
    if (!bot.hasBall) {
        tryGetBall();
    }

}


void Play::firstCycleFunction() {
    bot.set_i2c_LED(1, 1, BLAU);
    bot.setCompassHeading();
}

void Play::disabledFunction() {

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

void Anstoss::disabledFunction() {

}

void Anstoss::firstCycleFunction() {

}

void Debug::main() {
}

void Debug::disabledFunction() {

}

void Debug::firstCycleFunction() {
    bot.set_i2c_LED(1, 1, ROT);
    bot.setCompassHeading();
}