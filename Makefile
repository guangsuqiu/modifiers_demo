# Makefile for Windows using MinGW
CC=gcc
CFLAGS=-Wall -Wextra 
LIBS=-lpsapi

TARGET=print.exe
TARGET2=change.exe
TARGET3=basic_addr.exe

all: $(TARGET) $(TARGET2) $(TARGET3)

$(TARGET):
	$(CC) $(CFLAGS) -o $(TARGET) print.c

$(TARGET2):
	$(CC) $(CFLAGS) -o $(TARGET2) change.c

# 链接库的位置很关键，如果$(LIBS) 在-o的前面就会报错
$(TARGET3):
	$(CC) $(CFLAGS) -o $(TARGET3) basic_addr.c $(LIBS) 

clean:
	del $(TARGET) $(TARGET2) $(TARGET3)
