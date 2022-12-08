# FTP-Server
A Simple FTP server: use the Unix Socket API to construct a minimal ftp server.
## Goals
- To learn how to program with TCP sockets in C
- To learn how to read and implement a well-specified protocol.
- To develop your programming and debugging skills as they relate to the use of sockets in C
- To implement the server side of the FTP protocol
- To develop general networking experimental skills
- To develop a further understanding of what TCP does, and how to manage multiple TCP connections from the server perspective.
## State diagram for server and client model
<img width="384" alt="image" src="https://user-images.githubusercontent.com/62523802/205857683-be127271-3cf3-4343-b707-b41219f210fd.png">

## What to Implement

The server is to implement the server part of the ftp protocol as specified in [RFC 959](https://www.ietf.org/rfc/rfc959.txt) . Section 5.1 outlines the minimum requirement for an ftp server but we are going to strip down that a bit more and add a couple of commands. The commands from the client to the server you are to support are (relevant RFC sections are in parenthesis):<br>

- USER - (4.1.1)
- QUIT - (4.1.1)
- CWD - (4.1.1) For security reasons you are not accept any CWD command that starts with ./ or ../ or contains ../ in it. (hint see the chdir system call.)
- CDUP - (4.1.1) For security reasons do not allow a CDUP command to set the working directory to be the parent directory of where your ftp server is started from. (hint use the getcwd system call to get the initial working directory so that you know where things are started from and then if a CDUP command is received while in that diredtory return an appropriate error code to the client.)
- TYPE - (4.1.1) you are only to support the Image and ASCII type (3.1.1, 3.1.1.3)
- MODE - you are only to support Stream mode (3.4.1)
- STRU - you are only to support File structure type (3.1.2, 3.1.2.1)
- RETR - (4.1.3)
- PASV - (4.1.1)
- NLST - (4.1.3) to produce a directory listing, you are only required to support the version of NLST with no command parameters. Respond with a 501 if the server gets an NLST with a parameter.
For any command that your server does not support respond with a reply code of 500.

Your program will implement a scaled back ftp server. Since it is a server the functionality of your implementation will be judged on how it interacts with real ftp clients.
