include ../../build_config.mk
CFLAGS += -I../
OUTPUT_DIR=../../api/cpp

all: lib
	${CXX} -I$(OUTPUT_DIR) -o demo demo.cpp $(OUTPUT_DIR)/libsim-client.a

lib: client.h client.cpp
	$(CXX) $(CFLAGS) -c client.cpp
	ar -cru libsim-client.a\
		client.o\
		../sim.o ../message.o ../decoder.o ../link.o
	cp client.h ../message.h libsim-client.a ../../api/cpp

clean:
	rm -rf *.o *.a *.out
