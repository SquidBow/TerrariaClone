CXX = g++

# Added School.cpp to the list
main: main.cpp
	$(CXX) main.cpp -o main -l raylib

clean:
	rm -f main
