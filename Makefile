ROOT=$(realpath .)

all:
	mkdir -p build && cd $(ROOT)/lib/algorithms && gcc sequential.c -o $(ROOT)/build/sequential

clean:
	rm -rf build
