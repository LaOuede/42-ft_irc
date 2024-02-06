<p align="center">
  <img src="https://github.com/LaOuede/42-project-badges/blob/main/badges/ft_irce.png" />
</p>

<h1 align=center>ft_irc</h1>

<div align="center">

The <b>ft_irc</b> project involves the implementation of an Internet Relay Chat (IRC) server and client.
The purpose is to create a scalable and efficient server that can handle multiple clients concurrently.
The project encompasses features such as channel creation, private messaging, and handling various IRC commands,
fostering a comprehensive understanding of networking, system calls, and inter-process communication.

Go to [42 Québec](https://42quebec.com/) to discover the course ! 👈
</div>

---

<h3 align="left">What I've learned 📚</h3>

I've acquired basic knowledge in :
- <b>Internet Relay Chat (IRC) Protocol</b>: Understanding the IRC protocol, including its commands, message formats, and server-client interactions.
- <b>Networking Protocols</b>: Knowledge of TCP/IP networking protocols, including how data is transmitted over the internet using TCP sockets.
- <b>Socket Programming</b>: Understanding how to create, bind, listen on, accept, connect to, read from, and write to sockets in C programming.
- <b>Error Handling and Recovery</b>: Strategies for detecting, reporting, and recovering from errors in network communication, including graceful shutdowns and error logging.
- <b>Testing and debugging</b>

---

<h3 align="left">If I had to do it all over again 🗒</h3>

- I would create a User class rather than a structure in the server class.
- I would optimize the use of the abstract class ACommand.
- I would group together all error and reply definitions in a .hpp file.

---

<h3 align="left">Usage 🛠</h3>

1- Compilation :
```bash
make
```

2- Execution :
```bash
./ircserv <port> <password>
```

3- Connection :

<img width="649" alt="Screen Shot 2024-02-06 at 9 01 39 AM" src="https://github.com/LaOuede/42-ft_irc/assets/114024436/744274fa-2cae-4e14-ba12-8a3b6533db6a">

4- Commands implemented :

<code>INVITE</code>: Allows inviting a user to join a specific channel.</br>
<code>JOIN</code>: Allows a user to join an existing channel.</br>
<code>KICK</code>: Allows kicking a user from a given channel.</br>
<code>MODE</code>: Allows modifying the modes of a user or a channel.
- Flag i: Enables/Disables the invitation-only mode of a channel.</br>
- Flag k: Enables/Disables the password of a channel.</br>
- Flag l: Enables/Disables a user limit in a channel.</br>
- Flag o: Grants/Revokes operator rights of the channel.</br>
- Flag t: Enables/Disables restrictions for modifying the topic of a channel by a user.</br>

<code>NAMES</code>: Lists all of the nicknames on the server.</br>
<code>NICK</code>: Allows a user to change their nickname.</br>
<code>PART</code>: Allows a user to leave a channel.</br>
<code>PASS</code>: Allows a client to provide a password to identify with the server.</br>
<code>PING</code>: Used to test the presence of an active client.</br>
<code>PRIVMSG</code>: Sends a message to the specified user or channel.</br>
<code>TOPIC</code>: Sets message as the channel topic.</br>

---

<div align="center">

This project was realized as a team effort with [TwindZ](https://github.com/TwindZ) and [Prezcoder](https://github.com/Prezcoder) 🚀

</div>

---

<div align="center">

To look at [my next 42 project](https://github.com/LaOuede/ft_transcendence) !
</div>
