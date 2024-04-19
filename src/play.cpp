#include "play.h"

Bohlebots bot;
ncSerial nc(1312, ENABLE_WIFI);
espOTA ota("Bohlebots", "soccer2018", ENABLE_WIFI);


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
    checkLackOfProgress();
    if (corner != 0) {
        executeCorners();
        return;
    }
    if (!bot.seesBall) {
        moveToOwnGoal();
        return;
    }
    if (!bot.hasBall) {
        tryGetBall();
        nc.sendMessage("try get ball");
        return;
    }
    if (bot.hasBall) {
        playOffensive();
        return;
    }

}

void Play::checkLackOfProgress() {
    if (bot.hasBall != lastBallPossession) {

        lastBallPossession = bot.hasBall;
        timeSinceLastBallPossessionChange = 0;
    }
    if (bot.ballDirection != lastBallDirection) {
        lastBallDirection = bot.ballDirection;
        timeSinceLastBallDirectionChange = 0;
    }
    if (abs(bot.compassDirection - lastCompassDirection) > compassNoiseLimit) {
        lastCompassDirection = bot.compassDirection;
        timeSinceLastCompassDirectionChange = 0;
    }


    bool isDistanceChange = false;

    for (int i = 0; i < 4; ++i) {
        if (abs(bot.distances[i] - lastDistances[i]) > distanceNoiseLimit) {
            lastDistances[i] = bot.distances[i];
            timeSinceLastDistanceChange[i] = 0;
        }

        if (timeSinceLastDistanceChange[i] < 1000) {
            isDistanceChange = true;
        }
    }
    isDistanceChange = false; //TODO remove this by fixing distance as lack of progress indicator

//    nc.sendMessage("lack? " + String(isLackOfProgress ? "yes" : "no")
//                   + " ball_dir " + String(timeSinceLastBallDirectionChange)
//                   + " compass " + String(timeSinceLastCompassDirectionChange)
//                   + " ball pos " + String(timeSinceLastBallPossessionChange)
//                   + " distanceChanged? " + String(isDistanceChange ? "yes" : "no"));


    isLackOfProgress = (!isDistanceChange && timeSinceLastCompassDirectionChange > 1000 &&
                        timeSinceLastBallDirectionChange > 1000 && timeSinceLastBallPossessionChange > 1000);


    if (corner != 0 && !bot.hasBall && timeSinceLastBallPossessionChange > 2000) {
        //ball has been lost → stop corner mode
        corner = 0;
        cornerTimer = 0;
    }


    if (!isLackOfProgress) {
        return;
    }

    if (abs(corner) == isLackOfProgress && cornerTimer > 1000) {
        corner *= 2;
    }


    if (bot.hasBall) {
        if (bot.distances[1] < 10 && bot.distances[3] < 10) {
            ///bot durch gegnerischen bot blockiert
        } else {
            //bot hat freie fahrt in eine richtung
            corner = (bot.distances[1] > bot.distances[3]) ? 1 : -1;
            cornerTimer = 0;
        }
    }
}

void Play::executeCorners() {
//TODO differentiate corner 1 and 2
    int cornerGoalDirection = (corner < 0) ? -1 : 1;
    cornerGoalDirection = -1; //TODO remove this line
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

void Play::evaluateSensorData() {

}

void Play::firstCycleFunction() {
    bot.setCompassHeading();
    timeSinceLastBallPossessionChange = 0;
    timeSinceLastBallDirectionChange = 0;
    timeSinceLastCompassDirectionChange = 0;
    for (int i = 0; i < 4; ++i) {
        timeSinceLastDistanceChange[i] = 0;
    }

    corner = 0;
    cornerTimer = 0;

    isLackOfProgress = false;

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

    int scale = sqrt(pow(offsetToCenter_X, 2) + pow(distanceToGoal, 2)) / 10; //TODO adjust scale

    bot.omnidrive(offsetToCenter_X, distanceToGoal * 0, 0, scale);


}


void Play::tryGetBall() {
    bot.setRotation(0);
    int absoluteBallDirection = abs(bot.ballDirection);

    int x_vector = 0;
    int y_vector = 0;
    int scale = 70;
    if (absoluteBallDirection == 8) {
        x_vector = (bot.distances[1] > bot.distances[3]) ? 1 : -1;
    } else if (absoluteBallDirection == 7) {
        x_vector = (bot.ballDirection > 0) ? -2 : 2;
        y_vector = -4;
    } else if (absoluteBallDirection == 6) {
        x_vector = (bot.ballDirection > 0) ? -1 : 1;
        y_vector = -2;
    } else if (absoluteBallDirection >= 4) {
        x_vector = 0;
        y_vector = -1;
    } else if (absoluteBallDirection == 3) {
        x_vector = (bot.ballDirection > 0) ? 1 : -1;
        y_vector = -1;
    } else if (absoluteBallDirection == 2) {
        y_vector = 2;
        x_vector = (bot.ballDirection > 0) ? 2 : -2;
        scale = 60;
    } else if (absoluteBallDirection == 1) {
        y_vector = 1;
        x_vector = (bot.ballDirection > 0) ? 2 : -2;
        scale = 50;
    } else if (absoluteBallDirection == 0) {
        x_vector = 0;
        y_vector = 1;
        scale = 100;
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

void i2cScanner::main() {

}

void i2cScanner::disabledFunction() {
    Serial.println("-------------- Scanning --------------");


    int deviceCount = 0;

    for (int address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        int error = Wire.endTransmission();
        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) {
                Serial.print("0");
            }
            Serial.println(address, HEX);
        } else if (error == 4) {
            Serial.print("Unknown error at address 0x");
            if (address < 16) {
                Serial.print("0");
            }
            Serial.println(address, HEX);
        }
    }
    Serial.println("Found " + String(deviceCount) + " devices");
    Serial.println("-------------- Scanning done --------------");
}

void i2cScanner::firstCycleFunction() {

}

void motorTest::main() {

}

void motorTest::disabledFunction() {}

void motorTest::firstCycleFunction() {

}

void Debug::main() {

}


void Debug::disabledFunction() {

}

void Debug::firstCycleFunction() {
//    bot.setCompassHeading();
}