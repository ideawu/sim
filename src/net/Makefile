CFLAGS=
OBJS = message.o decoder.o sim.o
CXX=g++

all: $(OBJS)
	g++ test.cpp $(OBJS)
	
.cpp.o:
	$(CXX) -c $< -o $@

clean:
	rm -rf *.o a.out

