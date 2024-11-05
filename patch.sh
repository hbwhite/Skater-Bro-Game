#!/bin/bash

# Author: 	Harrison White
# Date:		12/27/18
# Info:		Applies custom patches I've made to fresh cocos2d-x projects.

patch -p0 < patch_font.diff
