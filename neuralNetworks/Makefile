all: trial1 trial2 trial3

trial1: trial1.cpp
	gcc -Wall -o trial1 trial1.cpp -lncurses -lm

trial2: trial2.cpp
	gcc -Wall -o trial2 trial2.cpp -lncurses -lm

trial3: trial3.cpp
	gcc -Wall -o $@  $^ -lncurses -lm

clean:
	rm -f trial1 trial2 trial3
