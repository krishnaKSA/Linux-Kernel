CC=gcc  #compiler
TARGET1=ioctl_userapp #target file name
TARGET2=chardriver_userapp
TARGET3=chardrivermuldevice_userapp

all:	ioctl_userapp.o	chardriver_userapp.o	chardrivermuldevice_userapp.o
	$(CC) ioctl_userapp.c -o $(TARGET1)
	$(CC) chardriver_userapp.c -o $(TARGET2)
	$(CC) chardrivermuldevice_userapp.c -o $(TARGET2)

clean:
	rm *.o $(TARGET1)
	rm *.o $(TARGET2)
	rm *.o $(TARGET3)