#/bin/bash
g++ -Wall -c *.cpp -D LINUX -D SERVER --std=c++11 -I../glm -l:libfbxsdk.a
g++ *.o -Wall /usr/lib/gcc4/x64/release/libfbxsdk.a -o craftgame-server --std=c++11 -lpthread -ldl
rm *.o
