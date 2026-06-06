CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra \
           -I./include \
           -I../Core/include

SRC = src/*.cpp

OBJ = obj/*.o

all:
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $(SRC)
	mv *.o obj/

clean:
	rm -rf obj

rebuild: clean all