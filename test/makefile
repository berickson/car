always: test_output.txt
	@echo Test Output
	@echo ------------
	@cat test_output.txt

test_output.txt: test makefile
	./test > test_output.txt


test: *.cpp *.h
	g++ -o test *.cpp -std=c++11
