OS := $(shell uname)
ARCH := $(shell uname -m)
LEAP_SDK_PATH := /lib/Leap_Motion_SDK_Linux_2.3.1/LeapDeveloperKit_2.3.1+31549_linux/LeapSDK
INCLUDE = -I/usr/local/opencv4
OPENCV_LIBS := $(shell pkg-config --libs opencv4)
OPENCV_INCLUDE := $(shell pkg-config --cflags opencv4)
GL_LIBS := -lGL -lGLU


ifeq ($(OS), Darwin)
	LEAP_LIBRARY := $(LEAP_SDK_PATH)/lib/libLeap.dylib -Wl,-rpath,$(LEAP_SDK_PATH)/lib
else
	LEAP_LIBRARY := $(LEAP_SDK_PATH)/lib/x64/libLeap.so -Wl,-rpath,$(LEAP_SDK_PATH)/lib/x64
endif

Main: Main.cpp SampleListener.cpp
	@rm -f Main
	$(CXX) -Wall -g -I$(LEAP_SDK_PATH)/include $(OPENCV_INCLUDE)  $(GL_LIBS)  Main.cpp SampleListener.cpp -o Main $(LEAP_LIBRARY) $(OPENCV_LIBS) 

clean:
	@rm -rf Main Main.dSYM
.PHONY: rebuild
rebuild: clean Main









