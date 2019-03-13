./bin/tester: ./src/pigrrlread.c
	gcc -Wall -g -o ./bin/tester ./src/tester.c ./src/pigrrlread.c -lwiringPi

tester: ./bin/tester
	./bin/tester
clean:
	rm ./bin/*
