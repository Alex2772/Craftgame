#/bin/bash
g++ -w -c *.cpp -DLINUX -DSERVER --std=c++11 -I../../libs/boost_1_64_0/
g++ *.o -w -o craftgame-server --std=c++11 -L../../libs/boost_1_64_0/stage/lib/ -l:libboost_python.a -lpython3.6m -lpthread -ldl -lX11 -lGL -lGLU -l:libGLEW.a -lopenal -lvorbis -logg -lpng -ljpeg -lcurl -lfreetype -lassimp -lutil -lvorbisfile
#rm *.o
