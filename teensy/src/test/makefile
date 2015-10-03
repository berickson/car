TESTED_CPPS = ../Task.cpp ../Sequence.cpp ../Pid.cpp ../Fsm.cpp ../Beep.cpp ../Beeper.cpp 


always: test_output.txt
	@echo Test Output
	@echo ------------
	@cat test_output.txt

test_output.txt: test makefile
	./test > test_output.txt


test: *.cpp $(TESTED_CPPS) *.h  ../*.cpp ../*.h
	g++ -o test *.cpp $(TESTED_CPPS) -I . -I .. -std=c++11 
