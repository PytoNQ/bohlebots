#include "play.h"

void Strategy::run() {
    if (!this->isEnabled && this->canBeDisabled) { return; }

    if (this->isFirstCycle) {
        this->runFirstCycle();
        this->isFirstCycle = false;
    }
    this->main();

}

void Strategy::setEnabled(bool _isEnabled) {
    this->isEnabled = _isEnabled;
    if (!_isEnabled) {
        this->isFirstCycle = true;
    }
}

void Idle::main() {
    // do nothing
}

void Idle::runFirstCycle() {
    // do nothing
}

void Play::main() {

}

void Play::runFirstCycle() {

}

void Anstoss::main() {

}

void Anstoss::runFirstCycle() {

}

void Debug::main() {

}

void Debug::runFirstCycle() {

}