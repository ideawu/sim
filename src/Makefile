include ../build_config.mk

CFLAGS += -I .
OBJS = 

all: ${OBJS}

.cpp.o:
	$(CXX) ${CFLAGS} -c $< -o $@

clean:
	rm -rf *.o *.a *.out

