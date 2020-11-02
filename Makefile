rootfinder: rootfinder.c
	gcc -g -Wall -Wstrict-prototypes -Wshadow -Wconversion tinyexpr.c rootfinder.c -lm

clean:
	rm -fr *.exe *.o *.stackdump rootfinder

