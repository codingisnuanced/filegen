ftoh.o: ftoh.c ftoh.h fgfile.h log.h
	gcc -c ftoh.c

fgfile.o: fgfile.c fgfile.h log.h
	gcc -c fgfile.c

jvutils.o: jvutils.c jvutils.h log.h
	gcc -c jvutils.c

log.o: log.c log.h
	gcc -c log.c

ftoh: ftoh.o fgfile.o jvutils.o log.o
	gcc ftoh.o fgfile.o jvutils.o log.o -o ftoh