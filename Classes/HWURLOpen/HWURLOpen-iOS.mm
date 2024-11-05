//
//  HWURLOpen-iOS.cpp
//  skaterbro-mobile
//
//  Created by Harrison White on 4/15/18.
//

#include "HWURLOpen.h"
#import <UIKit/UIKit.h>

void HWURLOpen::openURL(const char *url) {
    [[UIApplication sharedApplication]openURL:[NSURL URLWithString:[NSString stringWithUTF8String:url]]];
}
