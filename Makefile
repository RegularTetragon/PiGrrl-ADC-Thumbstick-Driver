PIGRRLREAD = ./src/pigrrlread.c ./src/pigrrlread.h
PIGRRLOUT = ./src/pigrrlout.c


all: ./bin/tester ./bin/calibrator

tester: ./bin/tester
	./bin/tester

calibrator: ./bin/calibrator
	./bin calibrator

./bin/calibrator: $(PIGRRLREAD) 
	gcc -Wall -g -o ./bin/calibrator ./src/pigrrlcalibrate.c ./src/pigrrlread.c -lwiringPi
	
./bin/tester: ${PIGRRLREAD} ./src/tester.c
	gcc -Wall -g -o ./bin/tester ./src/tester.c ./src/pigrrlout.c ./src/pigrrlread.c -lwiringPi

clean:
	rm ./bin/*

