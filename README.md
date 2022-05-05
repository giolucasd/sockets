# sockets

Used to maintain code developed for computer network laboratory course.

The project aims to exercise sockets programming within computer networks field.

The idea is to have a fully functional video streaming plataform with client-server archtecture and hand built transport protocol.

## Running

First, you will have to compile the program:

```
cd client-server/PROTOCOL
gcc -o server.out server.c
gcc -o client.out client.c
```

Note that PROTOCOL may be only "tcp" or "udp".

Then, you need to set the server up in one terminal/machine:

```
./server.out
```

And run the client in another terminal/machine, being sure to provide the server address:

```
./client.out ADDRESS
```

If you're running both server and client on the same machine, ADDRESS will probably be "127.0.0.1".

From there on, the client interface should be intuitive enough :)