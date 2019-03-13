./bin/driver: ./src/driver.c
	gcc -Wall -g -o ./bin/driver ./src/driver.c -lwiringPi

run: ./bin/driver
	./bin/driver
clean:
	rm ./bin/driver
