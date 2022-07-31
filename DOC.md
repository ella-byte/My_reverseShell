# Documentation for the Reverse Shell

Let's start with the least interesting, the client.

## Client

The usage of the client program

`./client [IP] [PORT]`

So, for example, this is a valid usage of the client software:

`./client 172.16.21.3 7898`

The client will try to connect to that address using a TCP/IP connection. If it didn't succeed in connecting, the program would just show that it couldn't and stop. If it is able to connect to the server, it will create a 4 KB buffer to handle user input (which will be used to send commands). I used the `readline` library, which allows you to handle arrow keys and basic keyboard shortcuts to edit the command.

There are two reserved commands:

- `exit: close` the connection but let the server continue listening.

- `STOP`: close the connection and tell the server to shut down.

If the user's input is none of those commands, then the remote server (reverse shell) will execute them and send back the output of the command.

I used `read` and `write` syscalls instead of `send` and `recv` because the

The first ones are more generic and work for any `file descriptor,` and since a connection in Linux uses a `file descriptor,` I preferred using the generic one. Also, I wasn't going to use the `flag` parameter from the `send/recv` syscall.

As you can see, the client does nothing fancy, simply:

`get input` -> `execute input` -> `get output`

and start over.

You can basically use netcat as the client, you just won't get a prompt and the readline advantages.

# Server (reverse shell)

Now comes the most interesting part: the reverse shell itself!

I will skip the socket part since it's basically just a tcp/ip listener that listens to the address given in the argument. I'll directly jump to the interesting part, the client handling, except that I added the flag `SO_REUSEADDR,` which allows us to quickly reuse the socket after the connection is closed.

The server can handle one client at a time but does not stop after a client disconnects (even after a non-graceful disconnection).

Just like the client, the server has two reserved inputs (that it gets from the connection):

- `exit`: close the connection. The server continues listening.

- `STOP`: close the connection and the server shuts down.

When the server gets an input from the user, it will first check if it's a reserved word. If not, it will try to execute that command.

## Execution part

Most of the time, the command will be run using `popen,` which allows us to run a program and get a pipe from it. Since we won't insert data in the `stdin` of it, we just open the pipe as `read only`.

Before running the command with `popen,` we check if the command contains `cd` at the beginning. If yes, we try to use the `chdir` (change directory) syscall to change the current process directory.

When running a program with `popen,` since it works mostly like the `system` function and not like the `execve`'s syscall family, we can write some bash directly in the command we want to execute:

`ls | grep myfile` is a valid command to run.

# Error handling

I think I handled the most important errors, such as:

- not enough arguments (argv[3] if argc is equal to 1, for example)

- not being able to connect to the server

- not being able to bind the server connection

- not crash or weird behavior on closing the connection in a dirty way

- basic return code from command
