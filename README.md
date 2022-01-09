# Projeto RC

The goal of this project is to develop a prototype of a centralized messaging service.

The development of the project requires implementing a Directory Server (DS) and a
User Application (User). The DS server and the various User application instances are
intended to operate on different machines connected to the Internet.

The DS will be running on a machine with known IP address and ports.

## File organization

**common** *useful functions used across the project*

**server** *server functionality of the project*

**user** *user functionality of the project*

### common

Useful functions used across the Project

#### debug.h

Debug related utility, not meant to be shown in the release version

#### libio

Library to print formated messages to the user

#### util

Library with some utility, e.g. parsing messages and files

### server

#### server.c

Main file of the server functionality
Handles all the initializations and parsing of user input

#### commands

Functionalities that resolve commands received from a user

#### connection

Functionalities related to handling sockets and the udp and tcp protocols

#### file_management

Handles the file managing functionalities which keep the information the server presistes each session

#### strlinkedlist

Implementation of a linked list

#### Presistence Information storing system

**SERVER**
&emsp;|-> **USERS**
&emsp;&emsp;|-> ***UID***
&emsp;&emsp;&emsp;|-> **pass.txt** *File that stores user with the correspondant UID 's pass*
&emsp;&emsp;&emsp;|-> **login** *File created if the user is logged in into the system*
&emsp;|-> **GROUPS**
&emsp;&emsp;|->***GID***
&emsp;&emsp;&emsp;|-> **MSG**
&emsp;&emsp;&emsp;&emsp;|-> ***MID***
&emsp;&emsp;&emsp;&emsp;&emsp;|-> **FILE**
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;|-> ___/*___ *File sent anexed to the message by the user*
&emsp;&emsp;&emsp;&emsp;&emsp;|-> **author.txt** *File with the message's author's uid*
&emsp;&emsp;&emsp;&emsp;&emsp;|-> **text.txt** *File with the message's text*
&emsp;&emsp;&emsp;|-> ***UID*** *Group creator's uid*
&emsp;&emsp;&emsp;|-> **name.txt** *File with the groups's name*

### user

#### client.c

Main file of the user functionality
Handles all the initializations and parsing of user input

#### commands

Functionalities that resolve commands intended to be sent to the server

#### connection

Functionalities related to handling sockets and the udp and tcp protocols.
Also handles presistente information kept from session to session by the user

## Authors

*Ana Rita Duarte* 95531

*Eduardo Barrancos* 95566

*Rui Pires* 95670

## Notes

Msg author:
> GROUPS/01/MSG/0001/author.txt

Msg txt:
> GROUPS/01/MSG/0001/text.txt

Msg anexed file:
> GROUPS/01/MSG/0001/FILE/*
