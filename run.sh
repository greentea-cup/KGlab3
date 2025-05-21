#!/bin/bash
export WINEPREFIX="${HOME}/.wine-msvc"
./build.sh && (cd KGlab && wine ./x64/Release/KGlab.exe)
