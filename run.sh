#!/bin/bash
./build.sh && (WINEPREFIX="${HOME}/.wine-msvc" cd KGlab && wine ./x64/Release/KGlab.exe)
