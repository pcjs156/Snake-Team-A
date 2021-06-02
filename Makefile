all:
	g++ -std=c++11 -o snake snakegame.cpp Manager.h Manager.cpp Objects.h Snake.h Snake.cpp -lncursesw -Wmultichar