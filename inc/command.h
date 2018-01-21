#ifndef COMMAND_H
#define COMMAND_H

typedef struct _command
{
    const char * cmd;   // blank cmd is a line skip in the help print
    const char * args;
    const char * desc;
    int (* func) (int argc, char ** argv);
} command;

void print_commands(const command * commands, unsigned int num_commands);

#endif//COMMAND_H