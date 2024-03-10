targets := $(wildcard *.o)

main : main.o texture.o
	g++ @opcjeCpp ${targets} -o main -lsfml-graphics -lsfml-window -lsfml-system

%.o: %.cpp
	g++ @opcjeCpp $*.cpp -c

clean:
	rm -f *.o