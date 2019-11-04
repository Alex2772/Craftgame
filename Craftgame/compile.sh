#/bin/bash
#g++ -w -c *.cpp -D LINUX --std=c++11 -I../../libs/boost_1_64_0/
g++ *.o -w -o craftgame --std=c++11 -L../../libs/boost_1_64_0/stage/lib/ -l:libboost_python.a -lpython3.6m -lpthread -ldl -lX11 -lGL -lGLU -l:libGLEW.a -lopenal -lvorbis -logg -lz -lpng -ljpeg -l:libcurl.a -lfreetype -l:libassimp.a -lutil -lvorbisfile
#rm *.o
