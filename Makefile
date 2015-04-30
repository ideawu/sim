$(shell sh build.sh 1>&2)
include build_config.mk

all:
	cd src; make
	
clean:
	cd src; make clean
	rm -rf *.o a.out
