all:
	g++ -std=c++11 -o snake snakegame.cpp Manager.h Objects.h Snake.h -lncursesw -Wmultichar