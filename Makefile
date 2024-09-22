# Makefile for Windows using MinGW

CC=gcc
CFLAGS=-Wall -Wextra 

TARGET=print.exe
TARGET2=change.exe

all: $(TARGET) $(TARGET2)

$(TARGET): print.c
	$(CC) $(CFLAGS) -o $(TARGET) print.c

$(TARGET2): change.c
	$(CC) $(CFLAGS) -o $(TARGET2) change.c

clean:
	del $(TARGET) $(TARGET2)
