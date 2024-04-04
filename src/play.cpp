#include "play.h"

Bohlebots bot;


#define FIELD_X 122
#define FIELD_Y 183


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
        moveToOwnGoal();
        return;
    }
    if (!bot.hasBall) {
        tryGetBall();
        return;
    }
    if (bot.hasBall) {
        playOffensive();
    }

}


void Play::firstCycleFunction() {
    bot.set_i2c_LED(1, 1, BLAU);
    bot.setCompassHeading();
}

void Play::disabledFunction() {

}


void Play::moveToOwnGoal() {
    bot.setRotation(0);
    if (abs(bot.compassDirection) >= 10) {
        return;
    }

    // Calculate x_position of bot
    int offsetToCenter_X = (bot.distances[1] - bot.distances[3]) / 2;
    int distanceToGoal = bot.distances[2] - FIELD_Y / 10; // wanted distance from goal

    if (abs(distanceToGoal) <= 5) { //TODO: adjust value
        bot.drive(0, 0);
        return;
    }
    if (abs(offsetToCenter_X) >= 5) { //TODO: adjust value
        bot.drive(120 * offsetToCenter_X > 0 ? 1 : -1, 100);
    }
    bot.drive(0, distanceToGoal > 0 ? 100 : -100);

}




void Play::tryGetBall() {
    if (abs(bot.ballDirection) >= 3) {
        moveBehindBall();
        return;
    }
    if (abs(bot.ballDirection) != 0) {
        int angularBallDirection = bot.ballDirection * 360 / 16;

        if (abs(bot.compassDirection + angularBallDirection) < 50) {
            bot.setRotation(bot.compassDirection + angularBallDirection);
        } else {
            bot.setRotation(0);
        }
    } else if (bot.ballDirection == 0) {
        bot.drive(0, 100);
    }



}

void Play::moveBehindBall() {
    bot.setRotation(0);
    if (abs(bot.ballDirection) <=
        6) { //TODO kann ich bei 7 auch gerade nach hinten fahren?, lohnt es sich mehr, schon ab z.b. 5 mit 120° zu fahren, um zeit zu sparen
        bot.drive(180, 100);
        return;
    }
    // Ball liegt so hinter mir, dass ich nicht gerade fahren kann

    int direction = 120 * (bot.distances[1] > bot.distances[3] ? 1 : -1); //TODO berücksichtigung des anderen bots?
    bot.drive(direction, 100);
}

void Play::playOffensive() {
    if (bot.seesGoal) {
        bot.setRotation(bot.compassDirection + bot.goalDirection * 60);
    }

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
    bot.setCompassHeading();
}