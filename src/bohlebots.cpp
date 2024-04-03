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
    setBoardLED(1, AUS);
    setBoardLED(2, AUS);
}

void Bohlebots::init() {

    for (int i = 0; i < 8; i++) {
        Wire.beginTransmission(i2c_button_adresses[i]);
        byte error = Wire.endTransmission();
        if (error == 0) {
            is_i2c_port_enabled[i] = true;
        }


        Serial.print("LED_Tast : " + String(i) + " : ");
        Serial.println(error == 0 ? "true" : "false");

    }

    delay(100);
    Wire.beginTransmission(COMPASS_ADDRESS);
    byte error = Wire.endTransmission();
    if (error == 0) {
        isCompassEnabled = true;
    }

    delay(100);
    Wire.beginTransmission(PIXY_ADDRESS);
    error = Wire.endTransmission();
    if (error == 0) {
        isPixyEnabled = true;
        pixy.init(PIXY_ADDRESS);
    }

    Serial.print("Compass : ");
    Serial.println(isCompassEnabled ? "true" : "false");
    Serial.print("Pixy : ");
    Serial.println(isPixyEnabled ? "true" : "false");
    Serial.println("init done");
}

void Bohlebots::wait(int ms) {
    delayMillisTimer = 0;
    updateBot();
    while (delayMillisTimer < ms) {
        if (delayMillisTimer % 10 == 0) {
            updateBot();
        } else {
            delay(1);
        }
    }
}

void Bohlebots::updateBot() {
    sync_i2c_IO();
//    getIRData();
    getUSData();
    getCompassData();
    getPixyData();
    motor1.updateMotorSpeed();
    motor2.updateMotorSpeed();
    motor3.updateMotorSpeed();
}




/*
 * ---------------------- I2C ----------------------
 */

void Bohlebots::sync_i2c_IO() {

    for (int i = 0; i < 8; i++) {
        if (!is_i2c_port_enabled[i]) {
            continue;
        }
        int ledValue = 255 - _i2c_led1_array[i] - _i2c_led2_array[i];
        Wire.beginTransmission(i2c_button_adresses[i]);
        Wire.write(ledValue);
        Wire.endTransmission();

        Wire.requestFrom(i2c_button_adresses[i], 1);
        if (Wire.available()) {
            int data = 255 - Wire.read();
            data %= 128;
            _i2c_button2_array[i] = (data > 63);
            data %= 2;
            _i2c_button1_array[i] = (data > 0);
        }
    }
}

void Bohlebots::getIRData() {
    int irRingData = 0;
    Wire.requestFrom(IR_ADDRESS, 1);
    if (Wire.available()) {
        irRingData = Wire.read();
    }
    ballDistance = irRingData / 16; // not accurate
    seesBall = ballDistance != 0;
    ballDirection = seesBall ? (irRingData % 16) - 7 : 1000;
}

void Bohlebots::getCompassData() {
    if (!isCompassEnabled) {
        return;
    }
    Wire.beginTransmission(COMPASS_ADDRESS);
    Wire.write(0x01);
    Wire.endTransmission();
    Wire.requestFrom(COMPASS_ADDRESS, 2);
    if (Wire.available() >= 2) {
        int high = Wire.read();
        int low = Wire.read();
        compassDirection = ((high << 8) | low) * 360;
        compassDirection /= 65536;
        compassDirection = ((compassDirection - compassHeading + 180 + 360) % 360) - 180;

    }

}

void Bohlebots::setCompassHeading() {
    compassHeading = 0;
    getCompassData();
    compassHeading = compassDirection;
}

void Bohlebots::getPixyData() {
    if (!isPixyEnabled) {
        return;
    }
    pixy.ccc.getBlocks();
    seesGoal = false;
    seesOwnGoal = false;

    if (pixy.ccc.numBlocks == 0) {
        return;
    }

    Block block = pixy.ccc.blocks[0];
    if (block.m_signature == 1) { // Goal
        seesGoal = true;
        goalDirection = block.m_x - 79; //316/4; 316 is pixy width
    } else if (block.m_signature == 2) { // own Goal
        seesOwnGoal = true;
        ownGoalDirection = block.m_x - 79;
    }
}

void Bohlebots::getUSData() {
    Wire.requestFrom(US_ADDRESS, sizeof(distances));

    if (Wire.available() >= sizeof(distances)) {
        Wire.readBytes((uint8_t *) distances, sizeof(distances));

    }
}

/*
 * ---------------------- IO ----------------------
 */

void Bohlebots::set_i2c_LED(int device, int nr, int color) {
    if (device < 0 || device > 7) {
        return;
    }
    if (color < 0 || color > 7) {
        return;
    }

    if (nr == 1) {
        _i2c_led1_array[device] = color * 2;
    }
    if (nr == 2) {
        color *= 16;
        if (color > 63) {
            color += 64;
        }
        _i2c_led2_array[device] = color;
    }
}

bool Bohlebots::get_i2c_Button(int device, int button) {
    if (device < 0 || device > 7) {
        return false;
    }
    if (button == 1) {
        return _i2c_button1_array[device];
    }
    if (button == 2) {
        return _i2c_button2_array[device];
    }
    return false;
}

void Bohlebots::setBoardLED(int led, int color) {
    if (color < 0 || color > 7) {
        return;
    }
    if (led == 1) {
        setRGB(led1r, led1g, led1b, color);

    }
    if (led == 2) {
        setRGB(led2r, led2g, led2b, color);
    }
}

void Bohlebots::setRGB(int r, int g, int b, int color) {
    digitalWrite(r, !(color & 2));
    digitalWrite(g, !(color & 1));
    digitalWrite(b, !(color & 4));
}

bool Bohlebots::getBoardButton(int button) {
    return getInput(button) == 0;
}

int Bohlebots::getInput(int input) {
    if (input == 1) {
        return analogRead(INPUT1);
    }
    if (input == 2) {
        return analogRead(INPUT2);
    }
    if (input == 3) {
        return analogRead(INPUT3);
    }
    if (input == 4) {
        return analogRead(INPUT4);
    }
    return 4096;
}

/*
 * ---------------------- FAHREN -----------------------
 */

void Bohlebots::drive(double x_speed, double y_speed, double w_speed, int scale) {

    if (abs(compassDirection) > 7) {
        w_speed = 10 * (compassDirection < 0 ? -1 : 1);
    }

    double m1 = x_speed / 2 + y_speed * sqrt(3) / 2 + w_speed;
    double m2 = -x_speed + w_speed;
    double m3 = x_speed / 2 - y_speed * sqrt(3) / 2 + w_speed;

    double max = std::max({std::abs(m1), std::abs(m2), std::abs(m3)});

    m1 = m1 / max * scale;
    m2 = m2 / max * scale;
    m3 = m3 / max * scale;

    Serial.print("m1: ");
    Serial.println(m1);
    Serial.print("m2: ");
    Serial.println(m2);
    Serial.print("m3: ");
    Serial.println(m3);
    Serial.println(" ");

    motor1.drive(static_cast<int>(m1));
    motor2.drive(static_cast<int>(m2));
    motor3.drive(static_cast<int>(m3));


}


/*
 * ---------------------- MOTOREN ----------------------
 */
Motor::Motor(int pin, int pwnChannel) {
    this->pin = pin;
    this->pwnChannel = pwnChannel;
}

void Motor::drive(int speed) {
    this->nominalSpeed = speed;
    this->setSpeed(speed);
}

void Motor::setSpeed(int speed) {
    this->currentSpeed = speed;
    speed = std::min(std::max(speed, -100), 100);
    int pwm = static_cast<int>(std::round(std::abs(speed) * 2.55));
    int dir = speed < 0 ? LOW : HIGH;
    digitalWrite(pin, dir);
    ledcWrite(pwnChannel, pwm);
}


void Motor::updateMotorSpeed() {
    return; //TDOD fix
    if (nominalSpeed == currentSpeed) {
        return;
    }
    unsigned long currentMillis = millis();
    auto deltatime = static_cast<double>((currentMillis - lastRunMillis) / 1000.0);
    lastRunMillis = currentMillis;
    int change = static_cast<int>(round(static_cast<double>((nominalSpeed - currentSpeed)) * deltatime * 200));
    setSpeed(currentSpeed + change);
}