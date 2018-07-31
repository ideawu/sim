include ../../build_config.mk

CFLAGS += -I ../
OBJS = line_message.o line_parser.o line_server.o

all: $(OBJS)
	ar -cru ./libline.a ${OBJS}

.cpp.o:
	$(CXX) ${CFLAGS} -c $< -o $@

clean:
	rm -rf *.o *.a *.out

