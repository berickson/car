test_output.txt: play_console makefile
	./play_console > test_output.txt
	@echo Test Output
	@echo ------------
	@cat test_output.txt


play_console: main.cpp
	g++ -o play_console main.cpp -std=c++11
