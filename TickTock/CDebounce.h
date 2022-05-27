
#ifndef _CDebounce_h_
#define _CDebounce_h_
#include <elapsedMillis.h>

//set the pin mode separately
class Debounce {
public:
    int DebounceTimeMs;                             //time that changes will be ignored after a change
    Debounce(int Pin, int DebounceMillis = 50);     //default debounce is 50ms
    int ReadDirect(void);                           //read the pin, bypassing debounce
    int Read(void);                                 //read the pin and return the current debounced state
    bool Changed(int Dirn);                         //debounce the pin and return true if the desired change just happened
                                                    //Dirn = LOW/0; HIGH/1; 2 (both directions)
private:
    int Pin;
    int Prev;
    int State;
    elapsedMillis t;
};

#endif  // _CDebounce_h_
