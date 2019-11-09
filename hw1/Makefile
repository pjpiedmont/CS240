all: cpu-bound io-bound

cpu-bound: cpu-bound.c
	gcc cpu-bound.c -o cpu-bound

io-bound: io-bound.c
	gcc io-bound.c -o io-bound

clean:
	rm -f cpu-bound io-bound