# Simple Shell
Simple OS shell for a Unix-type system, made in C as part of an assignment at the University of Strathclyde.

## Usage
You can get a compiled version of the shell from the releases section, or if you'd like to compile it yourself, you can run:
```bash
gcc -o simple-shell shell.c alias.c history.c
```
Following this, you can run the shell with:
```bash
./simple-shell
```

## Commands
The shell supports the following commands:<br><br>

`cd` – Change working directory. <br><br>

`getpath` – Print system path. <br><br>

`setpath` – Set system path. <br><br>

`history` – Print history contents (numbered list of commands in history, 
            including their parameters, in ascending order from least to most recent). <br><br>

`!!` –  Invoke the last command from history (e.g. if your last command
        was ‘ls –lF’ then ‘!!’ will execute ‘ls –lF’). <br><br>

`!<no>` - Invoke command with number <no> from history (e.g. !5 will
          execute the command with number 5 from the history). <br><br>
  
`!-<no>` -  Invoke the command with number the number of the current
            command minus <no> (e.g. !-3 if the current command number is 5 will
            execute the command with number 2, or !-1 will execute again the last
            command). <br><br>
  
`alias` – print all set aliases (alias plus aliased command)<br><br>

`alias <name> <command>` -  Alias name to be the command. Note that the command may 
                            also include any number of parameters. <br><br>

`unalias <command>` - Remove any associated alias. <br><br>
  
## Contributors
Developed as a Group Project, members:
* James Dick
* Yohanes Zhelezov
* Cameron MacReady
* Jamie McKitting
* Mary Amanda Iferi-Norman
