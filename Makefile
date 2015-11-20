build: ftoh.o fgfile.o jvutils.o log.o
	gcc bin/ftoh.o bin/fgfile.o bin/jvutils.o bin/log.o -o ftoh
	mv ftoh bin/

install:
	sudo cp bin/ftoh /usr/local/bin
	sudo chmod 755 /usr/local/bin/ftoh

uninstall:
	sudo rm /usr/local/bin/ftoh

bin:
	mkdir bin/

ftoh.o: bin ftoh.c ftoh.h fgfile.h log.h
	gcc -c ftoh.c
	mv ftoh.o bin/

fgfile.o: bin fgfile.c fgfile.h log.h
	gcc -c fgfile.c
	mv fgfile.o bin/

jvutils.o: bin jvutils.c jvutils.h log.h
	gcc -c jvutils.c
	mv jvutils.o bin/

log.o: bin log.c log.h
	gcc -c log.c
	mv log.o bin/

default:
	ftoh -d audio/ -f 100 -n 10 -p 10 -rv

valgrind:
	gcc -g ftoh.c fgfile.c jvutils.c log.c -o ftoh && valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ftoh -d audio/ -f 100 -n 10 -p 10

clean:
	rm -rf *.o html/ html_dirs.txt html_files.txt index.html ftoh.log bin

clean-nginx:
	sudo rm -r /usr/share/nginx/filegen/html/ /usr/share/nginx/filegen/html_dirs.txt /usr/share/nginx/filegen/html_files.txt /usr/share/nginx/filegen/index.html

nginx: clean-nginx ftoh
	sudo mkdir /usr/share/nginx/filegen/
	sudo cp --recursive html/ index.html /usr/share/nginx/filegen/
	sudo chmod 755 /usr/share/nginx/filegen/html