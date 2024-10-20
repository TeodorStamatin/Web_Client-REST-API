CC=g++
CCFLAGS=-Wall -Werror -Wno-error=unused-variable -g

client: client.cpp requests.cpp helpers.cpp buffer.cpp server.cpp
	$(CC) -o $@ $^ $(CCFLAGS)

clean:
	rm -f *.o client
