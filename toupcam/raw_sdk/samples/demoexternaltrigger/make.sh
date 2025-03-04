#!/bin/bash
os=`uname -s`
if [[ $os = "Linux" ]]; then
	g++ -Wl,-rpath -Wl,'$ORIGIN' -L. -g -o demoexternaltrigger demoexternaltrigger.cpp -ltoupcam
else
	clang++ -Wl,-rpath -Wl,. -L. -g -o demoexternaltrigger demoexternaltrigger.cpp -ltoupcam
fi
