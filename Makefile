main: ./src/main.c
	gcc -std=c99 -o ./build/main ./src/main.c -Wall && ./build/main