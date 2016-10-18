all:
	g++ main.cc -o zenu -std=c++11 `sdl2-config --cflags --libs` -lstdc++ -lm -lGLESv2 -D_GNU_SOURCE -ffast-math

gcw0:
	/opt/gcw0-toolchain/usr/bin/mipsel-linux-g++ main.cc -o zenu -std=c++11 -I/opt/gcw0-toolchain/usr/mipsel-gcw0-linux-uclibc/sysroot/usr/include/SDL2 -D_REENTRANT -L/opt/gcw0-toolchain/usr/mipsel-gcw0-linux-uclibc/sysroot/usr/lib -lstdc++ -lm -lSDL2 -lpthread -lGLESv2 -D_GNU_SOURCE -ffast-math -DGCW0
