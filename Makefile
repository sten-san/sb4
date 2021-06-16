./build/main: ./main.cpp
	mkdir -p ./build
	g++ -W -Wall -std=c++17 -I ./ ./main.cpp -o ./build/main

