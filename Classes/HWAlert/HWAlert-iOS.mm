//
//  HWAlert.mm
//  skaterbro-mobile
//
//  Created by Harrison White on 4/5/18.
//

#include "HWAlert-iOS.h"

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

// HWAlert Interface

HWAlert* HWAlert::create(const char* title, const char* message, const char* cancelButtonTitle, ...) {
    va_list args;
    va_start(args, cancelButtonTitle);
    HWAlert* retval = HWAlertiOS::create(title, message, cancelButtonTitle, args);
    va_end(args);
    return retval;
}

// public static method used to access private members of HWAlertiOS through HWAlert::create

HWAlertiOS* HWAlertiOS::create(const char* title, const char* message, const char* cancelButtonTitle, va_list otherButtonTitles) {
    // automatically manage memory
    HWAlertiOS* hwAlert = new HWAlertiOS(title, message, cancelButtonTitle);
    
    // handle variadic args
    for (const char* button = va_arg(otherButtonTitles, const char*); button != NULL; button = va_arg(otherButtonTitles, const char*)) {
        hwAlert->otherButtonTitles.push_back(button);
    }
    
    return hwAlert;
}

HWAlertiOS::HWAlertiOS(const char* title, const char* message, const char* cancelButtonTitle)
    :
    title(title),
    message(message),
    cancelButtonTitle(cancelButtonTitle),
    otherButtonTitles(std::vector<const char*>())
{
    
}

void HWAlertiOS::show() {
    NSString* titleStr = title ? [NSString stringWithUTF8String:title] : nil;
    NSString* messageStr = message ? [NSString stringWithUTF8String:message] : nil;
    NSString* cancelStr = cancelButtonTitle ? [NSString stringWithUTF8String:cancelButtonTitle] : nil;
    
    UIAlertController* alert = [UIAlertController alertControllerWithTitle:titleStr message:messageStr preferredStyle:UIAlertControllerStyleAlert];
    [alert addAction:[UIAlertAction actionWithTitle:cancelStr style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
        if (this->callback) {
            NSLog(@"calling callback");
            this->callback(this, 0);
        } else {
            NSLog(@"no callback for cancel");
        }
    }]];
    
    // for (std::vector<const char*>::iterator it = otherButtonTitles.begin(); it != otherButtonTitles.end(); it++) {
    for (int i = 0; i < otherButtonTitles.size(); i++) {
        const char* button = otherButtonTitles.at(i);
        
        [alert addAction:[UIAlertAction actionWithTitle:[NSString stringWithUTF8String:button] style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            if (this->callback) {
                // 0 is for the cancel button, higher numbers are for other buttons
                this->callback(this, i + 1);
            }
        }]];
    }
    
    UIViewController* vc = [[[UIApplication sharedApplication]keyWindow]rootViewController];
    if (vc) {
        [vc presentViewController:alert animated:YES completion:nil];
    } else {
        NSLog(@"[HWAlert] error: could not get root view controller to show alert!");
    }
}
