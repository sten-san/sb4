./build/main: ./main.cpp
	mkdir -p ./build
	g++ -W -Wall -std=c++17 -I ./sb4 ./main.cpp -o ./build/main

