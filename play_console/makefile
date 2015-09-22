always: test_output.txt
	@echo Test Output
	@echo ------------
	@cat test_output.txt

test_output.txt: play_console makefile
	./play_console > test_output.txt


play_console: *.cpp *.h
	g++ -o play_console *.cpp -std=c++11
