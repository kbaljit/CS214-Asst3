all: netfileserver libnetfiles

netfileserver:
	gcc -Wall -pthread -o netfileserver netfileserver.c

libnetfiles:
	gcc -Wall -o libnetfiles libnetfiles.c

clean:
	rm ./netfileserver ./libnetfiles