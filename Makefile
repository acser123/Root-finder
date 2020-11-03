rootfinder: rootfinder.c
	gcc -g -Wall -Wstrict-prototypes -Wshadow -o rootfinder tinyexpr.c rootfinder.c -lm


clean:
	rm -fr *.exe *.o *.stackdump rootfinder

