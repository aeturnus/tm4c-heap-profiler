#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <malloc.h>

#include <command.h>

void print_commands(const command * commands, unsigned int num_commands)
{
    for (unsigned int i = 0; i < num_commands; ++i) {
        const command * cmd = &commands[i];
        printf("    %s %s : %s\n", cmd->cmd, cmd->args, cmd->desc);
    }
    printf("\n");
}