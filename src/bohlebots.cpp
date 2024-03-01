//
// Created by antho on 01.03.24.
//

#include "bohlebots.h"


Bohlebots::Bohlebots() {
    Serial.begin(115200);
    Wire.begin(SDA, SCL);

    pinMode(DRIVE_DIS, OUTPUT);
    digitalWrite(DRIVE_DIS, LOW);
    pinMode(DRIVE1_DIR, OUTPUT);
    pinMode(DRIVE2_DIR, OUTPUT);
    pinMode(DRIVE3_DIR, OUTPUT);

    pinMode(INPUT1, INPUT);
    pinMode(INPUT2, INPUT);
    pinMode(INPUT3, INPUT);
    pinMode(INPUT4, INPUT);

    pinMode(led1r, OUTPUT);
    digitalWrite(led1r, HIGH);
    pinMode(led1g, OUTPUT);
    digitalWrite(led1g, HIGH);
    pinMode(led1b, OUTPUT);
    digitalWrite(led1b, HIGH);
    pinMode(led2r, OUTPUT);
    digitalWrite(led2r, HIGH);
    pinMode(led2g, OUTPUT);
    digitalWrite(led2g, HIGH);
    pinMode(led2b, OUTPUT);
    digitalWrite(led2b, HIGH);

    Wire.begin(SDA, SCL);

    ledcAttachPin(DRIVE1_PWM, 1);
    ledcSetup(1, 1000, 8);
    ledcWrite(1, 0);
    ledcAttachPin(DRIVE2_PWM, 2);
    ledcSetup(2, 1000, 8);
    ledcWrite(2, 0);
    ledcAttachPin(DRIVE3_PWM, 3);
    ledcSetup(3, 1000, 8);
    ledcWrite(3, 0);
    ledcSetup(4, 1000, 8);
    ledcWrite(4, 0);
    boardled(1, AUS);
    boardled(2, AUS);
}

void Bohlebots::init() {
    for (int i = 0; i < 8; i++) {
        Wire.beginTransmission(tastLedID[i]);
        byte error = Wire.endTransmission();
        if (error == 0) {
            portena[i] = true;
        }


        Serial.print("LED_Tast : " + String(i) + " : ");
        Serial.println(error == 0 ? "true" : "false");

    }
    delay(100);
    Wire.beginTransmission(KOMPASS_ADRESSE);
    byte error = Wire.endTransmission();
    if (error == 0) {
        kompass_ena = true;
        Serial.println("Kompass true");
    } else { Serial.println("Kompass false"); }

    if (hatPixy) {
        Serial.print("Warte auf Pixy2 auf i2c 0x54...");
        pixy.init(0x54);
        Serial.println("done");
    }
}

void Bohlebots::wait(int ms) {
    delayMillisTimer = 0;
    getSensorData();
    while (delayMillisTimer < ms) {
        if (delayMillisTimer % 10 == 0) {
            getSensorData();
        } else {
            delay(1);
        }
    }
}


/*
 * ---------------------- MOTOREN ----------------------
 */
Motor::Motor(int pin, int pwnChannel) {
    this->pin = pin;
    this->pwnChannel = pwnChannel;
}

void Motor::setSpeed(int speed) {
    speed = std::min(std::max(speed, -100), 100);
    int pwm = static_cast<int>(std::round(std::abs(speed) * 2.55));
    int dir = speed < 0 ? LOW : HIGH;
    digitalWrite(pin, dir);
    ledcWrite(pwnChannel, pwm);
}