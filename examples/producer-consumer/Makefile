all: p c true false

p: p.c
	gcc -Wall $< -o $@

c: c.c
	gcc -Wall $< -o $@

true: true.c
	gcc -Wall $< -o $@

false: false.c
	gcc -Wall $< -o $@

clean:
	rm -f p c true false

.PHONY: all clean
