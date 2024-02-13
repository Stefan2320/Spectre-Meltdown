#gcc -D__Kernel__ -isystem /lib/modules/$(uname -r)/build/include -DMODULE -march=native PlaceSecretInKernel.c -o PlaceSecretKernel.ko

KVERSION=$(shell uname -r)

# Create object file and link to a .ko file
obj-m += PlaceSecretInKernel.o

#First it changes the dir to the one indicated by -C  then with M=$(PWD) it moves back to the module source  
#insmod file.ko: insert module into the linux kernel at runtime
#dmesg to see output 
all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean 
