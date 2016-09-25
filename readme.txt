Run "make" to compile all required files

To execute, do "./final"

The shell supports execution of basic commands, and also execution of installed commands (like pip etc.)
To run in background, you can type "&" after the command like you do in a normal shell

"cd", "echo", "pwd" commands have been implemented in system (i.e, even if base shell doesn't support them, this shell will)

"pinfo" command will display all info of all running processes
"pinfo [id]" will display info of that particular process with that ID.

"functions.h" file is the header where all functions are defined along with external variables
"executecommandbg.c" contains function for executing given command in background
"executecommand.c" contains function for executing given command in place
"changedir.c" contains function for executing the "cd" command
"showshellprompt.c" contains function for showing the default shell prompt along with some message, if any