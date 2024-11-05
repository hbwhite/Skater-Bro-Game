//
//  HWURLOpen.h
//  skaterbro-mobile
//
//  Created by Harrison White on 4/5/18.
//

#ifndef __HWURLOPEN_H__
#define __HWURLOPEN_H__

// Cannot declare virtual static methods so create() is implemented normally

class HWURLOpen {
public:
    static void openURL(const char* url);
};

#endif
