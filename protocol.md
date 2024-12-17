# LIGHTCHAT protocol - version 0.0

## Introduction

This is UNFINISHED as of writing (17 Dec 2024). It will change.

This protocol is perhaps not specified as good as it could be. This is because, at some point, I plan to turn the core of `lightchat` into a proper IRC server daemon, and that would obviously use the IRC protocol. Given that this protocol is so simple, the lack of detail is probably not an issue.

The syntax of this protocol is vaguely inspired by HTTP and the IRC specification (RFC 1459). It's not intended to be a copy of either, but I did opt for a text-based protocol to enable users to easily connect via `telnet` or similar software, thus making a proper `lightchat` client unnecessary (though convenient).

A binary protocol would have been both less annoying to parse and more efficient, but removing the need for a client was very convenient and was deemed a useful feature.

## Message grammar

Messages sent to and from servers using the LIGHTCHAT/0.0 protocol have the same general format. 

The grammar of a message in pseudo-BNF is as follows. Regular expressions are denoted as `/regex/`, where `regex` is a regular expression.

~~~
<message> ::= "LIGHTCHAT/" <version> " " <command> <args> "\r\n"

<version> ::= <number> "." <number>
<number> ::= /[0-9]+/

<command> ::= /[A-Z][A-Z0-9]*/

<args> ::= <arg> <args> | <text>
<arg> ::= / [^\0\r\n ]+/
<text> ::= /:[^\0\r\n]+/ | ""
~~~

For those not familiar with BNF, a message will roughly look like either of the following (assuming that the lines are terminated with a Windows-style CRLF):

~~~
LIGHTCHAT/0.0 COMMAND1 ARG1 ARG_2:3ARG
~~~

~~~
LIGHTCHAT/0.0 COMMAND1:text goes here qwertyuiop
~~~

Do note that commands are all uppercase, and must start with a letter.

## Commands

### Client-sent commands

~~~
Command: CONNECT
Parameters: <uname>
Returns: none

This command connects a user to the server with username <uname>. <uname> has a maximum length defined in a server's configuration file, and must not contain NUL, CR, LF, or any whitespace characters.

<uname> ::= [^\0\r\n\t\f\v ]+

Example usage:

LIGHTCHAT/0.0 CONNECT:foo1234
~~~

~~~
Command: MSG
Parameters: <msg-text>

This command sends a message to other users connected to the server.

Example usage:

LIGHTCHAT/0.0 MSG:this is a message!
~~~

~~~
Command: UNAMELEN
Parameters: none

Causes the server to send an OK response containing the maximum allowed username length.

Example usage (client):

LIGHTCHAT/0.0 UNAMELEN:

Example response (server):

LIGHTCHAT/0.0 OK UNAMELEN:123
~~~

~~~
Command: KILL
Parameters: [reason]

Notifies the server that the connection has been terminated by the client.

Example:

LIGHTCHAT/0.0 KILL
~~~

~~~
Command: PONG
Parameters: none

Response to a server's PING message. Failure to respond to a PING message will result in the server sending a KILL message and terminating the connection.

Example:

LIGHTCHAT/0.0 PONG
~~~

### Server-sent commands

~~~
Command: OK
Parameters: <command>
            [response]

Acknowledges the successful receipt and processing of a command by the server.

Example:

LIGHTCHAT/0.0 OK:CONNECT
~~~

~~~
Command: ERR
Parameters: <type>
            [message]

Returns an error with its type and an optional message. The error types are specifically defined here, the messages are implementation-specific (although examples are provided here).

Example:

LIGHTCHAT/0.0 ERR UNAME-IN-USE:The username you selected is already in use. 

Errors:

UNAME-IN-USE:The username you provided is already in use.
UNAME-BAD-CHARS:The username provided contained invalid characters.
BAD-COMMAND:The command provided was invalid.
BAD-PARAMS:Invalid parameters were provided with your command.
BAD-VERSION:Your client protocol version is invalid. Maybe switch protocols?
~~~

~~~
Command: KILL
Parameters: [reason]

Notifies the client that the connection has been terminated by the server.

Example:

LIGHTCHAT/0.0 KILL:Did not respond to ping.
~~~

~~~
Command: PING
Parameters: none

Checks whether the client is still able to receive and respond to messages. Sent at an interval defined in the server's configuration file. Clients are expected to respond with a PONG message.

Example:

LIGHTCHAT/0.0 PING

Example client response:

LIGHTCHAT/0.0 PONG
~~~

## Miscellaneous information

### Version numbers

The version number before the decimal point is the server version number. It denotes the set of messages that the server can send out and that the client can receive.

The version number after the decimal point is the client version number. It denotes the set of messages that the client can send and that the server can receive.

When any commands are removed, added, or edited to either the server or client set of commands, the appropriate version number shall be incremented. 

For example, if a new command was added allowing clients to send a private message to one another, the client version number would be incremented. 

Deciding which version numbers to allow and reject is implementation-specific. If a server implementation accepts a client with a specific version, it must communicate with that client using the protocol for that version, and it must list its version as that version. For example, when a server that can communicate with LIGHTCHAT/6.5 sends an `OK` response a client with LIGHTCHAT/5.4 support, the response is as follows:

~~~
LIGHTCHAT/5.4 OK CONNECT:Connection successful!
~~~



The current version of the LIGHTCHAT specification is `0.0`. This number is distinct from the version number of `lightchatd`.