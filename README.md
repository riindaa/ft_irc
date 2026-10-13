*This project has been created as part of the 42 curriculum by ssivilay, tdelmas2, akaddo.*

# ft_irc

## Description

**ft_irc** is an IRC (Internet Relay Chat) server written in C++98. The goal of the project is to understand how a real network server works: TCP sockets, non-blocking I/O, a single `poll()` loop handling every client, and a text protocol that real IRC clients can talk to.

The server can be used with a standard IRC client (our reference client is **irssi**) and supports:

- **Authentication and registration**: `PASS`, `NICK`, `USER`
- **Messaging**: `PRIVMSG` to a user or to a channel
- **Channels**: `JOIN` (with key support, and several channels at once: `JOIN #a,#b key_a,key_b`)
- **Channel operator commands**:
  - `KICK`: eject a client from the channel
  - `INVITE`: invite a client to a channel
  - `TOPIC`: change or view the channel topic
  - `MODE`: change the channel modes
    - `i`: set/remove invite-only channel
    - `t`: restrict `TOPIC` to channel operators
    - `k`: set/remove the channel key (password)
    - `o`: give/take channel operator privilege
    - `l`: set/remove the user limit

Any other command (`PART`, `QUIT`, `PING`, `CAP`, ...) is not implemented and gets a `421 Unknown command` reply. A client leaves the server by closing its connection.

### Bonus

- **File transfer**: handled through **DCC** (Direct Client-to-Client). The sending client opens a port and sends a CTCP message (`PRIVMSG <nick> :\x01DCC SEND <file> <ip> <port> <size>\x01`). The server relays it unchanged to the recipient, which then connects directly to the sender to download the file. The file data never goes through the server.
- **Bot**: a built-in bot named `bot` answers commands sent in private or in a channel:
  - `!hello`: greets the user
  - `!help`: lists the available commands
  - `!users`: lists the members of the current channel (channel only)

### Architecture

```
include/
└── irc.hpp             # common include for main.cpp
src/
├── main.cpp            # argument parsing, signals, server startup
├── server/             # Server: sockets, poll() loop, input/output buffers
├── parser/             # Parser (raw line -> Command) and Dispatcher (Command -> handler)
├── state/              # Client, Channel, ServerState, numeric replies
├── commands/           # one file per IRC command
└── bot/                # built-in bot
```

## Instructions

### Requirements

- A C++ compiler that supports C++98
- `make`
- An IRC client to test with (irssi recommended) and/or `nc`
- `clang-format` (optional, only for `make format` / `make check-format`)

### Compilation

```bash
make        # builds the ircserv binary
make clean  # removes object files
make fclean # removes object files and the binary
make re     # rebuilds everything

make format       # formats the sources with clang-format
make check-format # checks the formatting without modifying the files
```

### Execution

```bash
./ircserv <port> <password>
```

- `port`: the port the server listens on for incoming IRC connections
- `password`: the password every client must provide with `PASS`

Example:

```bash
./ircserv 6667 pw
```

### Connecting with irssi

```bash
irssi -c 127.0.0.1 -p 6667 -w pw -n alice
```

Then, inside irssi:

```
/join #general
/msg #general Hello everyone
/msg bot !help
```

### Connecting with nc

```bash
nc 127.0.0.1 6667
PASS pw
NICK alice
USER alice 0 * :Alice
JOIN #general
PRIVMSG #general :Hello everyone
```

Commands can be sent in several pieces (for example with `Ctrl+D` in `nc`): the server buffers the input until a full line is received.

### Testing the file transfer

With two irssi clients (`alice` and `bob`):

```
# alice
/dcc send bob ./test.txt

# bob
/dcc get alice
```

## Resources

### Documentation

- [RFC 2812: Internet Relay Chat: Client Protocol](https://www.rfc-editor.org/info/rfc2812/#section-3.1.3)
- [irssi documentation](https://irssi.org/documentation/): reference client

### Use of AI

AI was used as a support tool to clarify some protocol concepts, to help with testing and to draft this README. All suggestions were reviewed and tested by the team.
