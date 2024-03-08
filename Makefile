%.e : %.o
	g++ @opcjeCpp $*.o -o $*.e -lsfml-graphics -lsfml-window -lsfml-system

%.o: %.cpp
	g++ @opcjeCpp $*.cpp -c

clean:
	rm -f *.o