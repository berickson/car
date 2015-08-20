test_output.txt: stream_scrubber makefile
	./stream_scrubber > test_output.txt
	@echo Test Output
	@echo ------------
	@cat test_output.txt


stream_scrubber: stream_scrubber.cpp
	g++ -o stream_scrubber stream_scrubber.cpp -std=c++11
