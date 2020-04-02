CC ?= cc

all: rndaddentropy-hwrng

clean:
	rm -f rndaddentropy-hwrng

rndaddentropy-hwrng: clean
	$(CC) -s rndaddentropy-hwrng.c -o rndaddentropy-hwrng


.PHONY: clean
