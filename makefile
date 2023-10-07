CXX = clang++
CXXFLAGS = -std=c++11

all: main

main: main.cpp
	$(CXX) $(CXXFLAGS) -o main main.cpp

clean:
	rm main
