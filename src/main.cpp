#include <Arduino.h>

#include "play.h"

bool isEnabled = false;
Strategy *strategies[] = {new Idle(), new Play(), new Anstoss(1), new Anstoss(-1), new Debug()};
int strategyColors[5] = {AUS, GRUEN, BLAU, ROT, MAGENTA};
int currentStrategy = 1;


void setup() {
    bot.init();
    bot.setBoardLED(1, strategyColors[currentStrategy]);
    ota.begin();
    nc.begin();

}


bool modusButtonPressed = false;
bool startButtonPressed = false;

void testButtons() {
    bot.setBoardLED(2, bot.getInput(1) ? ROT : GRUEN);


    if (bot.get_i2c_Button(1, 2)) {
        bot.setCompassHeading();
    }
    if (bot.get_i2c_Button(1, 1)) {
        if (!startButtonPressed) {
            isEnabled = !isEnabled;
            startButtonPressed = true;
            if (isEnabled) {
                strategies[currentStrategy]->run(true, true);
            }
        }
    } else {
        startButtonPressed = false;
    }
    if (bot.getBoardButton(1)) {
        if (!modusButtonPressed) {
            modusButtonPressed = true;
            isEnabled = false;
            currentStrategy = (currentStrategy + 1) % 5;
            bot.setBoardLED(1, strategyColors[currentStrategy]);
            Serial.println("Strategy switched");
        }
//        bot.turnLEDsOff();
    } else { modusButtonPressed = false; }

}

int x = 0;
int y = 0;
void loop() {
    bot.wait(5);
    testButtons();
    strategies[currentStrategy]->run(false, isEnabled);

    ota.handle();
    nc.handle();

    Serial.println(bot.hasBall ? "true" : "false");
//
//    for(int i = 0; i < 4; i++){
//        Serial.print(bot.distances[i]);
//        Serial.print(" ");
//    }
//    Serial.println();


}

void wasd_drive() {

    if (Serial.available() > 0) {
        char in = Serial.read();
        if (in == ' ') {
            x = 0;
            y = 0;
        } else if (in == 'a') {
            x = -1;
            y = 0;
        } else if (in == 'd') {
            x = 1;
            y = 0;
        } else if (in == 's') {
            x = 0;
            y = -1;
        } else if (in == 'w') {
            x = 0;
            y = 1;
        } else if (in == '1') {
            bot.setRotation(-45);
        } else if (in == '2') {
            bot.setRotation(0);
        } else if (in == '3') {
            bot.setRotation(45);
        } else if (in == 'r') {
            bot.setCompassHeading();
        }
    }
    bot.omnidrive(x, y, 0, 70);
}