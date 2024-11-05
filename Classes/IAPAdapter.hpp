//
//  IAPAdapter_h
//  chocolate
//
//  Created by Harrison White on 12/30/18.
//
//

#ifndef IAPAdapter_h
#define IAPAdapter_h

#ifdef SDKBOX_ENABLED

#include "pluginiap/PluginIAP.h"

class IAPAdapter : public sdkbox::IAPListener {
    void onInitialized(bool ok) override {};
    void onSuccess(sdkbox::Product const& p) override {};
    void onFailure(sdkbox::Product const& p, const std::string &msg) override {};
    void onCanceled(sdkbox::Product const& p) override {};
    void onRestored(sdkbox::Product const& p) override {};
    void onProductRequestSuccess(std::vector<sdkbox::Product> const &products) override {};
    void onProductRequestFailure(const std::string &msg) override {};
    void onRestoreComplete(bool ok, const std::string &msg) override {};
};


#endif

#endif /* IAPAdapter_h */
