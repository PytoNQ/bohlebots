#include <Arduino.h>
#include <memory>
#include "bohlebots.h"
#include "play.h"

Bohlebots bot;
Strategy strategies[5] = {Idle(), Play(), Anstoss(1), Anstoss(-1), Debug()};
int strategyColors[5] = {AUS, GRUEN, BLAU, ROT, MAGENTA};
int currentStrategy = 0;

void setup() {
    bot.init();
}


bool modusButtonPressed = false;

void testButtons() {
    if (bot.get_i2c_Button(1, 1)) {
        if (currentStrategy == 0) {
            currentStrategy = 1;
        }
        strategies[currentStrategy].setEnabled(true);
    }
    if (bot.get_i2c_Button(1, 2)) {
        strategies[currentStrategy].setEnabled(false);
        currentStrategy = 0;
    }
    if (bot.getBoardButton(1)) {
        if (!modusButtonPressed) {
            modusButtonPressed = true;
            strategies[currentStrategy].setEnabled(false);
            currentStrategy = (currentStrategy + 1) % sizeof(strategies);
            bot.setBoardLED(1, strategyColors[currentStrategy]);
        }
    } else { modusButtonPressed = false; }
}

void loop() {
    bot.wait(5);
    testButtons();
    strategies[currentStrategy].run();
}