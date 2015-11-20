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

launch-default:
	./ftoh -d audio/ -f 100 -n 10 -p 10

valgrind:
	gcc -g ftoh.c fgfile.c jvutils.c log.c -o ftoh && valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./ftoh -d audio/ -f 100 -n 10 -p 10

clean:
	rm -rf *.o html/ html_dirs.txt html_files.txt index.html ftoh.log

clean-nginx:
	sudo rm -r /usr/share/nginx/filegen/html/ /usr/share/nginx/filegen/html_dirs.txt /usr/share/nginx/filegen/html_files.txt /usr/share/nginx/filegen/index.html

nginx: ftoh
	sudo mkdir /usr/share/nginx/filegen/
	sudo cp --recursive html/ html_dirs.txt html_files.txt index.html /usr/share/nginx/filegen/