#include <Arduino.h>
#include "play.h"


bool isEnabled = false;
Strategy *strategies[] = {new Idle(), new Play(), new Anstoss(1), new Anstoss(-1), new Debug()};
int strategyColors[5] = {AUS, GRUEN, BLAU, ROT, MAGENTA};
int currentStrategy = 0;

void setup() {
    bot.init();
    bot.setBoardLED(1, strategyColors[currentStrategy]);
}


bool modusButtonPressed = false;

void testButtons() {
    bot.setBoardLED(2, bot.getInput(1) ? ROT : GRUEN);


    if (bot.get_i2c_Button(1, 1)) {
        if (currentStrategy == 0) {
            currentStrategy = 1;
        }
        Serial.println("Strategy enabled");
        strategies[currentStrategy]->run(true, true);
        isEnabled = true;
    }
    if (bot.get_i2c_Button(1, 2)) {
        Serial.println("Strategy disabled");
        isEnabled = false;
        currentStrategy = 0;

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


void loop() {
    bot.wait(5);
    testButtons();
    strategies[currentStrategy]->run(false, isEnabled);
}

