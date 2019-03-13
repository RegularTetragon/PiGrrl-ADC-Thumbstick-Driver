PIGRRLREAD = ./src/pigrrlread.c ./src/pigrrlread.h
PIGRRLOUT = ./src/pigrrlout.c

tester: ./bin/tester
	./bin/tester
driver:
	make -C ./src
	
#./bin/pigrrl2controller: ${PIGRRLREAD} ./src/driver.c
#	gcc -Wall -g -o ./bin/pigrrl2controller ./src/pigrrlread.c ./src/driver.c -lwiringPi
./bin/tester: ${PIGRRLREAD} ./src/tester.c
	gcc -Wall -g -o ./bin/tester ./src/tester.c ./src/pigrrlout.c ./src/pigrrlread.c -lwiringPi

clean:
	rm ./bin/*

clean-driver:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
