CPP_FILES := $(wildcard ../../cpp/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
LD_FLAGS := -std=c++14 -pthread -lncurses -lwiringPi
CC_FLAGS := -std=c++14 -pthread -lncurses -lwiringPi -DRASPBERRY_PI

car-service: $(OBJ_FILES)
	g++ $(LD_FLAGS) -o $@ $^

obj/%.o: ../../cpp/%.cpp makefile
	g++ $(CC_FLAGS) -c -o $@ $<

