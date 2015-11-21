TESTED_CPPS = ../Task.cpp ../Sequence.cpp ../Pid.cpp ../Fsm.cpp ../Beep.cpp ../Beeper.cpp ../Statistics.cpp


always: test_output.txt
	@echo Test Output
	@echo ------------
	@cat test_output.txt

test_output.txt: test makefile
	./test > test_output.txt

test: *.cpp *.h
	g++ -pthread -o test main.cpp Arduino.cpp $(TESTED_CPPS) gtest/libgtest.a -I . -I .. -std=c++11 

