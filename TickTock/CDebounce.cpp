#include "CDebounce.h"


Debounce::Debounce(int Pin, int DebounceMillis){
    this->Pin = Pin;
    DebounceTimeMs = DebounceMillis;
    t = DebounceTimeMs;     //ensures we start out of debounce
}

int Debounce::ReadDirect(void){
    return digitalRead(Pin);
}

int Debounce::Read(void){
    int Sw;
    Sw = digitalRead(Pin);
    if (t > DebounceTimeMs){
        State = Sw;
    }
    if (Sw != Prev){
        t = 0;
        Prev = Sw;
    }
    return State;
}

bool Debounce::Changed(int Dirn){
    int prevState = State;
    int newState = (Read());
    if (Dirn == HIGH){
        if (prevState == LOW && newState == HIGH) return true;
    } else if (Dirn == LOW) {
        if (prevState == HIGH && newState == LOW) return true;
    } else if (Dirn == 2) {
        if (prevState != newState) return true;
    }
    return false;
}
