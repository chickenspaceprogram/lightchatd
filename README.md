# lightchatd

A lightweight, multithreaded chat server, written in C++ (honestly it's mostly just C with classes, all I wanted was a hashmap).

This server is unfortunately not cross-platform and only runs on Linux due to the usage of `epoll(7)`. However, I am working on getting a Docker container configured for it. If this was a proper server I'd use something like `libevent`, but I wanted to mess around with sockets, so here we are.

For a description of the `lightchat` protocol that this server uses, check out protocol.md.

This chat server communicates via TCP and is completely unencrypted. It does not cache messages, so you must be actively connected to the server to receive messages. It also does not separate messages into channels, all messages are sent into a global channel and all users connect to that global channel.

This is intended to be turned into the backend for a proper IRC server daemon at some undefined point in future, but I don't know if/when that will happen.

This project is partly just an excuse to learn about networking and multithreading, but I also intend to make it a legitimately useful (if barebones) application.