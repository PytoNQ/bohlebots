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
    bot.omnidrive(0, 0, 0, 0);
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
    bot.stop();
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

    int distanceToGoal = -bot.distances[2] + FIELD_Y / 10; // wanted distance from goal


    bot.omnidrive(offsetToCenter_X, distanceToGoal * 0, 0, 50);
//    if (abs(distanceToGoal) <= 5) { //TODO: adjust value
//        bot.drive(0, 0);
//        return;
//    }
//    if (abs(offsetToCenter_X) >= 5) { //TODO: adjust value
//        bot.drive(120 * offsetToCenter_X > 0 ? 1 : -1, 100);
//    }
//    bot.drive(0, distanceToGoal > 0 ? 100 : -100);

}




void Play::tryGetBall() {
    bot.setRotation(0);
    int absoluteBallDirection = abs(bot.ballDirection);

    int x_vector = 0;
    int y_vector = 0;
    int scale = 100;
    if (absoluteBallDirection == 8) {
        x_vector = (bot.distances[1] > bot.distances[3]) ? 1 : -1;
    } else if (absoluteBallDirection == 7) {
        x_vector = (bot.ballDirection > 0) ? -1 : 1;
        y_vector = -1;
    } else if (absoluteBallDirection == 6) {
        x_vector = (bot.ballDirection > 0) ? -1 : 1;
        y_vector = -2;
    } else if (absoluteBallDirection >= 3) {
        x_vector = 0;
        y_vector = -1;
    } else if (absoluteBallDirection == 2) {
        y_vector = -1;
        x_vector = (bot.ballDirection > 0) ? 2 : -2;
    } else if (absoluteBallDirection == 1) {
        y_vector = -1;
        x_vector = (bot.ballDirection > 0) ? 2 : -2;
    } else if (absoluteBallDirection == 0) {
        x_vector = 0;
        y_vector = 1;
    }
    Serial.println(bot.ballDirection);
    bot.omnidrive(x_vector, y_vector, 0, scale);
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
    int cornerGoalDirection = 1;
    if (bot.compassDirection * -1 * cornerGoalDirection > 20) {
        bot.omnidrive(-3 * cornerGoalDirection, 2, 0, 50);
        bot.set_i2c_LED(1, 1, BLAU);
        return;
    }
    if (bot.seesGoal && bot.goalDirection * cornerGoalDirection < 20) {
        bot.setRotation(0);
        bot.omnidrive(cornerGoalDirection * 3, 4, 0, 30);
        bot.set_i2c_LED(1, 1, GELB);
        return;
    }
    bot.setRotation(cornerGoalDirection * 10);
    bot.set_i2c_LED(1, 1, ROT);
    bot.omnidrive(cornerGoalDirection * 5, 2, 0, 50);
}


void Debug::disabledFunction() {

}

void Debug::firstCycleFunction() {
//    bot.setCompassHeading();
}