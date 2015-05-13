all: main

main: main.cc
	g++ -g -c main.cc -o main.o $(CFLAGS) -I/home/seri/ceph/src/include
	g++ -g main.o -L/home/seri/ceph/src/.libs -lrados -o stub $(LDFLAGS)

clean:
	rm -f main.o stub
