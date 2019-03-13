obj-m += pigrrl_driver.o
pigrrl_driver-objs += pigrrl_controller.o pigrrlread.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
