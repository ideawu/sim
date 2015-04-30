$(shell sh build.sh 1>&2)
include build_config.mk

all:
	cd src/net; make
	cd src/test; make
	
clean:
	cd src/net; make clean
	cd src/test; make clean
	rm -rf *.o a.out
