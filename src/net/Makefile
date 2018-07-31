include ../../build_config.mk

CFLAGS += -I ../
OBJS = fde.o link.o tcp_link.o

all: $(OBJS)
	
.cpp.o:
	$(CXX) ${CFLAGS} -c $< -o $@

clean:
	rm -rf *.o *.a *.out

