#!/bin/bash

# Author:	Harrison White
# Date:		12/27/18
# Info:		Installs required SDKBOX packages into fresh cocos2d-x projects.

# For interstitial and banner ads
sdkbox import admob

# For Game Center (iOS) and Google Play Games (Android) integration
sdkbox import sdkboxplay

# For sharing high scores via Email, SMS, Facebook, Twitter
sdkbox import share

# For Facebook sharing via the "share" plugin above
sdkbox import facebook
