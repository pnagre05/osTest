CXX = g++
CXXFLAGS = -std=c++17 -pthread

all: my-dining

my-dining: main.cpp LocksFree.h
	$(CXX) $(CXXFLAGS) main.cpp -o my-dining

clean:
	rm -f my-dining