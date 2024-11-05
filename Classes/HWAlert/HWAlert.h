//
//  HWAlert.h
//  skaterbro-mobile
//
//  Created by Harrison White on 4/5/18.
//

#ifndef __HWALERT_H__
#define __HWALERT_H__

#include <functional>

// platform-specific implementations of HWAlert will implement these methods

class HWAlert;

typedef std::function<void(HWAlert* alert, long index)> HWAlertCallback;

// Cannot declare virtual static methods so create() is implemented normally

// used const char* instead of std::string* so HWAlert can be called with variadic arguments
// __attribute__((sentinel)) ensures that the variadic arguments are null-terminated

class HWAlert {
public:
    static HWAlert* create(const char* title, const char* message, const char* cancelButtonTitle, ...) __attribute__((sentinel));
    virtual void show() {}
    
    void setCallback(HWAlertCallback callback) {
        this->callback = callback;
    }
    HWAlertCallback getCallback() {
        return this->callback;
    }
protected:
    HWAlertCallback callback;
};

#endif
