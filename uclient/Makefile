objects=client.o backup.o rabin.o recipe.o sha1.o bnet.o

client:$(objects)
	gcc -o client $(objects) -lssl -lcrypto

$(objects):client.h backup.h rabin.h recipe.h sha1.h bnet.h

.PHONY:clean
clean:
	-rm client $(objects) *.o
