CPP_FILES := $(wildcard ../../cpp/*.cpp)
H_FILES   := $(wildcard ../../cpp/*.h)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
LD_FLAGS := -std=c++17 -pthread -L/usr/local/lib -lopencv_core -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_features2d -lopencv_xfeatures2d -lopencv_calib3d
CC_FLAGS := -std=c++17 -pthread -L/usr/local/lib -lopencv_core -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_features2d -lopencv_xfeatures2d -lopencv_calib3d -DRASPBERRY_PI

car-service: $(OBJ_FILES)
	g++ $(LD_FLAGS) -o $@ $^

obj/%.o: ../../cpp/%.cpp $(H_FILES) makefile
	g++ $(CC_FLAGS) -c -o $@ $<

