objects = main.o texture.o map.o window.o

main : ${objects}
	g++ @opcjeCpp ${objects} -o main -lsfml-graphics -lsfml-window -lsfml-system

%.o: %.cpp
	g++ @opcjeCpp $*.cpp -c

clean:
	rm -f *.o