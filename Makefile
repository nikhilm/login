#Makefile for Login
CC=g++
CFLAGS=`sdl-config --cflags --libs` -Weffc++ -Wall
SOURCES=login.cpp

login:login.cpp
	$(CC) -o login $(CFLAGS) $(SOURCES)
	
install:login
	cp login /usr/bin/login_game
