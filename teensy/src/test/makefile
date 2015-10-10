GTEST_DIR = ~/git/googletest/googletest
TESTED_CPPS = ../Task.cpp ../Sequence.cpp ../Pid.cpp ../Fsm.cpp ../Beep.cpp ../Beeper.cpp ../Statistics.cpp


always: test_output.txt
	@echo Test Output
	@echo ------------
	@cat test_output.txt

test_output.txt: test makefile
	./test > test_output.txt

gtest: *.cpp *.h
	g++ -isystem ${GTEST_DIR}/include  -pthread -o gtest gtest.cpp Arduino.cpp $(TESTED_CPPS) ${GTEST_DIR}/libgtest.a -I . -I .. -std=c++11 


test: *.cpp $(TESTED_CPPS) *.h  ../*.cpp ../*.h
	g++ -o test *.cpp $(TESTED_CPPS) -I . -I .. -std=c++11 
