#!/bin/bash
os=`uname -s`
if [[ $os = "Linux" ]]; then
	g++ -Wl,-rpath -Wl,'$ORIGIN' -L. -g -o demomulti demomulti.cpp -ltoupcam
else
	clang++ -Wl,-rpath -Wl,. -L. -g -o demomulti demomulti.cpp -ltoupcam
fi
