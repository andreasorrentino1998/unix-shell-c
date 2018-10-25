all: shell

shell: main.o
	g++ -o shell main.o

main.o: main.c
	g++ -c main.c

clean:
	rm -f *.o
	rm -f ./shell
	rm -f *~
