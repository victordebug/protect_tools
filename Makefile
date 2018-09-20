objects = main.o md5.o log.o package.o

protect_tool: $(objects)
	cc -o protect_tool $(objects)


main.o: main.c md5.h main.h log.h package.h
	cc -c main.c
md5.o: md5.c md5.h main.h
	cc -c md5.c
log.o: log.c log.h main.h
	cc -c log.c
package.o: package.c package.h log.h md5.h
	cc -c package.c

.PHONY : clean
clean:
	rm -rf protect_tool $(objects)
