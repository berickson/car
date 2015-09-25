always: test_output.txt
	@echo Test Output
	@echo ------------
	@cat test_output.txt

test_output.txt: test makefile
	./test > test_output.txt


test: *.cpp *.h ../Pid.cpp
	g++ -o test *.cpp -I . -I .. \
	  ../Task.cpp ../Sequence.cpp ../Pid.cpp \
	  ../Fsm.cpp ../Beep.cpp ../Beeper.cpp \
	  -std=c++11
