//
// Created by Harrison White on 4/9/18.
//

#ifndef __HWALERT_ANDROID_H__
#define __HWALERT_ANDROID_H__

#include "HWAlert.h"
#include <vector>

class HWAlertAndroid : public HWAlert {
public:
    static HWAlertAndroid* create(const char* title, const char* message, const char* cancelButtonTitle, va_list otherButtonTitles);
    void show() override;
private:
    HWAlertAndroid(const char* title, const char* message, const char* cancelButtonTitle);
    HWAlertCallback callback;
    const char* title;
    const char* message;
    const char* cancelButtonTitle;
    std::vector<const char*> otherButtonTitles;
};

#endif
