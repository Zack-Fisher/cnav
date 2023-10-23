# cnav shell

## installation

To install and use the shell as the current user:

```sh
make
sudo make install
make chsh
```

## overview

cnav is a Linux shell that is written entirely in C with minimal configuration
language. It's meant to be configured through the source code, and the program
is written with that in mind.

It operates in modes, each "mode" changing the effect of the command inputted.
Any input that starts with _ is considered a mode switch command.
For example, write \_c in the shell to switch to c mode,
where every command you enter will be compiled and run as a child process.

## usage

### scripting

cnav can take in scripts like a normal shell, and can thus
be used as a shebang line. Modality is still active in the 
script headless mode, so make sure to specify the proper starting mode
with -f if you're using a mode other than "shell" as the initial.

change the mode that cnav starts in with 
```sh
cnav -f <mode_string>
```
