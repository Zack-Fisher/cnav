# cnav shell

## overview

cnav is a non-POSIX Linux shell that is written entirely in C with minimal configuration
language. It's meant to be configured through the source code, and the program
is written with that in mind.

It operates in modes, each "mode" changing the effect of the command inputted.
Any input that starts with _ is considered a mode switch command.
For example, write \_c in the shell to switch to c mode,
where every command you enter will be compiled and run as a child process.

cnav is not a programming language. After variable expansion, the \_shell mode string
will be passed as-is to the system shell (/bin/sh). This means that most tricks that
don't rely on expansion, eg &, &&, ||, [], () and whatnot will still work if they 
work in your /bin/sh symlink. Shell expansion tricks like $VARIABLES, aliases and
others will work in a customized and potentially different way.

## installation

To install and use the shell as the current user:

```sh
make # compile
sudo make install # symlink cnav to /bin/cnav
make chsh # activate the shell for the current user, requires reboot to fully happen.
```

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

### startup

Add a script to $HOME/.cnavrc and it will be parsed/run at runtime.

### aliasing

Like normal POSIX sh, use:
```sh
alias from=to
```

The expansion is really simple, it just searches through each word in the input
and replaces from=to using a hashmap. I don't think that's how normal POSIX sh
works.
