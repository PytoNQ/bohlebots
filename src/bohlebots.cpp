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
    pinMode(DRIVE4_DIR, OUTPUT);

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
    ledcAttachPin(DRIVE4_PWM, 4);
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

    delay(100);
    Wire.beginTransmission(IR_ADDRESS);
    error = Wire.endTransmission();
    if (error == 0) {
        isIREnabled = true;
    }

    delay(100);
    Wire.beginTransmission(US_ADDRESS);
    error = Wire.endTransmission();
    if (error == 0) {
        isUSEnabled = true;
    }


    setCompassHeading();

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
    getIRData();
    getUSData();
    getCompassData();
    getPixyData();
//    motor1.updateMotorSpeed();
//    motor2.updateMotorSpeed();
//    motor3.updateMotorSpeed();
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

    if (getInput(4) > 4000) {
        lightBarrierTimer = 0;
    }

    hasBall = lightBarrierTimer < 75;


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
        compassDirection = ((compassDirection - compassOffset + 180 + 360) % 360) - 180;

    }

    calculateAcceleration();
    checkForMovement();

}

void Bohlebots::calculateAcceleration() {
    int data[] = {0, 0};

    uint8_t error;
    uint8_t high_byte;
    uint8_t low_byte;


    for (int i = 0; i < 2; i++) {
        Wire.beginTransmission(COMPASS_ADDRESS);
        Wire.write(acceleration_high_adresses[i]);
        error = Wire.endTransmission();

        if (error != 0) { return; }

        Wire.requestFrom(COMPASS_ADDRESS, 2);
        while (Wire.available() < 2) {}
        high_byte = Wire.read();
        low_byte = Wire.read();


        data[i] = (int16_t) ((high_byte << 8) | low_byte);

        acceleration[i] = static_cast<float>(data[i]) / 256.0 * 100;

    }


}


void Bohlebots::checkForMovement() {
    if (acceleration[0] < 60 && acceleration[1] < 60) {
        if (accelerationTimer > 2000) {
            isAccelerating = false;
        }
    } else {
        accelerationTimer = 0;
        isAccelerating = true;
    }
}


void Bohlebots::setCompassHeading() {
    set_i2c_LED(1, 1, BLAU);
    compassOffset = 0;
    wait(50);
    getCompassData();
    compassOffset = compassDirection;
    set_i2c_LED(1, 1, AUS);

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
        goalDirection = block.m_x / 2 - 79; //316/4; 316 is pixy width
    } else if (block.m_signature == 2) { // own Goal
        seesOwnGoal = true;
        ownGoalDirection = block.m_x / 2 - 79;
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

void Bohlebots::turnLEDsOff() {
    setBoardLED(1, AUS);
    setBoardLED(2, AUS);
    for (int i = 0; i < 8; i++) {
        if (!is_i2c_port_enabled[i]) {
            continue;
        }
        set_i2c_LED(i, 1, 0);
        set_i2c_LED(i, 2, 0);
    }
}

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

void Bohlebots::drive(int direction, int speed, int rotation) {
    int rotationOffset = botRotation - compassDirection;


    direction /= 60;
    int max = std::abs(speed) + std::abs(rotation);
    if (max > 100) {
        speed = speed * 100 / max;
        rotation = rotation * 100 / max;
    }

    if (direction == 0) // geradeaus
    {
        motor1.drive(-speed + rotation);
        motor2.drive(+rotation);
        motor3.drive(speed + rotation);
    }

    if (direction == 1) // 60 Grad rechts
    {
        motor1.drive(+rotation);
        motor2.drive(-speed + rotation);
        motor3.drive(speed + rotation);
    }

    if (direction == -1) // -60 Grad links
    {
        motor1.drive(-speed + rotation);
        motor2.drive(speed + rotation);
        motor3.drive(+rotation);
    }

    if (direction == 3) // zurück
    {
        motor1.drive(speed + rotation);
        motor2.drive(+rotation);
        motor3.drive(-speed + rotation);
    }

    if (direction == -2) // -120 Grad links
    {
        motor1.drive(+rotation);
        motor2.drive(speed + rotation);
        motor3.drive(-speed + rotation);
    }

    if (direction == 2) // 120 Grad rechts
    {
        motor1.drive(speed + rotation);
        motor2.drive(-speed + rotation);
        motor3.drive(+rotation);
    }
}

void Bohlebots::stop() {
    motor1.drive(0);
    motor2.drive(0);
    motor3.drive(0);
}

void Bohlebots::omnidrive(double x_speed, double y_speed, double w_speed, int scale) {

    int maxVector = std::max(std::abs(x_speed), std::abs(y_speed));


    if (maxVector != 0) {
        x_speed = x_speed / maxVector;
        y_speed = y_speed / maxVector;
    }
    int maxW = 25;
    int factor = 22 - (7 * scale / 100);

    double rotationOffset = botRotation - compassDirection;

    w_speed += -exp(-abs(rotationOffset) / factor) * maxW + maxW;


    w_speed *= (rotationOffset > 0 ? -1 : 1);


    double m1 = (-x_speed / 2 + y_speed * sqrt(3) / 2) * scale;
    double m2 = (x_speed) * scale;
    double m3 = (-x_speed / 2 - y_speed * sqrt(3) / 2) * scale;

    double max = std::max({std::abs(m1 + w_speed), std::abs(m2 + w_speed), std::abs(m3 + w_speed)});

//    Serial.print("max: ");
//    Serial.println(max);

    if (max > 100) {
        m1 = m1 * (100 - w_speed) / max;
        m2 = m2 * (100 - w_speed) / max;
        m3 = m3 * (100 - w_speed) / max;
    }

    m1 += w_speed;
    m2 += w_speed;
    m3 += w_speed;

//    Serial.print("m1: ");
//    Serial.println(m1);
//    Serial.print("m2: ");
//    Serial.println(m2);
//    Serial.print("m3: ");
//    Serial.println(m3);


    motor1.drive(static_cast<int>(m1));
    motor2.drive(static_cast<int>(m2));
    motor3.drive(static_cast<int>(m3));


}


void Bohlebots::setRotation(int _botRotation) {
    botRotation = _botRotation;
}


/*
 * ---------------------- MOTOREN ----------------------
 */
Motor::Motor(int pin, int pwnChannel) {
    this->pin = pin;
    this->pwnChannel = pwnChannel;
}

void Motor::drive(int speed) {
    this->setSpeed(speed);
}

void Motor::setSpeed(int speed) {
//    this->currentSpeed = speed;
    speed = std::min(std::max(speed, -100), 100);
    int pwm = static_cast<int>(std::round(std::abs(speed) * 255 / 100));
    int dir = speed < 0 ? LOW : HIGH;
    digitalWrite(pin, dir);
    ledcWrite(pwnChannel, pwm);
}

void Motor::test(int delayTime) {

}


/*
 * ---------------------- TIME BASED INT CHANGER ----------------------
 */

void TimeBasedIntChanger::change(int _goal, int _duration) {
    goal = _goal;
    duration = _duration;
    startTime = millis();
}

void TimeBasedIntChanger::setDirectly(int _goal) {
    goal = _goal;
    number = _goal;
}

void TimeBasedIntChanger::update() {
    unsigned long elapsed = millis() - startTime;
    if (elapsed >= duration) {
        number = goal;
    } else {
        number = number + (goal - number) * (static_cast<float>(elapsed) / duration);
    }
}

int TimeBasedIntChanger::getNumber() {
    return number;
}
