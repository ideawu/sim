include ../../build_config.mk

CFLAGS += -I ../
OBJS=line_message.o line_parser.o

all: $(OBJS)

.cpp.o:
	$(CXX) ${CFLAGS} -c $< -o $@

clean:
	rm -rf *.o *.a *.out

