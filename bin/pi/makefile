CPP_FILES := $(wildcard ../../cpp/*.cpp)
H_FILES   := $(wildcard ../../cpp/*.h)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
LD_FLAGS := -g -std=c++17 -pthread -L/usr/local/lib -lopencv_core -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_features2d -lopencv_xfeatures2d -lopencv_calib3d -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4
CC_FLAGS := -g -std=c++17 -pthread -L/usr/local/lib -lopencv_core -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_features2d -lopencv_xfeatures2d -lopencv_calib3d -DRASPBERRY_PI -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4

car-service: $(OBJ_FILES)
	g++ $(LD_FLAGS) -o $@ $^

obj/%.o: ../../cpp/%.cpp $(H_FILES) makefile
	g++ $(CC_FLAGS) -c -o $@ $<

