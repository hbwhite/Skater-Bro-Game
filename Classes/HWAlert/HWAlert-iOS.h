//
// Created by Harrison White on 4/9/18.
//

#ifndef __HWALERT_IOS_H__
#define __HWALERT_IOS_H__

#include "HWAlert.h"
#include <vector>

class HWAlertiOS : public HWAlert {
public:
    static HWAlertiOS* create(const char* title, const char* message, const char* cancelButtonTitle, va_list otherButtonTitles);
    void show() override;
private:
    HWAlertiOS(const char* title, const char* message, const char* cancelButtonTitle);
    const char* title;
    const char* message;
    const char* cancelButtonTitle;
    std::vector<const char*> otherButtonTitles;
};

#endif
