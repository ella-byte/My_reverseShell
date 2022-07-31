# What is a reverse shell
A reverse shell is a program shellcode that allows you to run commands over a connection from a remote device to your device.

A reverse shell is used to gain control over a device after it has been compromised.

## How does it work

Most of the time, a hacker will share and spread malicious software and start a listener that will wait for someone to run that malware. Once someone runs that malware, a reverse shell will be executed, allowing the hacker to run commands on the remote device over the internet.

## Why a Reverse Shell

The reverse shell is really useful when you want to infect as many people as possible. You can just spread the malware and setup a listener and wait for a victim to fall into the trap. You could also code a script for the client so that anytime a reverse shell is connected, it will run a script to assure persistence (backdoor).

## Why this project

This project is for the development class of cyber security that takes place at Henallux.

The purpose is to make a reverse shell in C to understand how the memory works under the hood (unlike when programming using Python, for example).

## How to compile

- client

`gcc client.c -o client -lreadline -O3`

`-lreadline` to add the readline lib when linking

`-O3` to optimise the executable to __blazingly fast__

- server

`gcc server.c -o server -O3`

## TODO

- [x] Respect the rules

- [x] Software compile and run without errors

- [x] Respect the good ways seen during the classes

- [x] The project is made ouf of two C software, one being the client and the other being the server, they can communcate using sockets

- [x] The server software is able to execute basic commands such as: whoami, ifconfig, cat /etc/passwd and send the results to the client that prints them to the screen

- [x] The IP and PORT must be given to the software using main argument (argc, argv), this is true for the server AND the client

- [x] This software is compatible with Linux distributions like Debian/Kali Linux

- [x] The deadline is respected

- [x] Error handling

- [x] Clean way of stopping the malware (server)

- [x] Able to change process directory

- [x] Versioning (GIT)
