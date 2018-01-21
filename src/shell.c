#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <malloc.h>

#define BUFFER_SIZE 256


int cmd_help(int argc, char ** argv);
int cmd_echo(int argc, char ** argv);

// shell stuff

#define ARRAY_LEN(x) (sizeof(x)/sizeof(x[0]))

typedef struct _command
{
    const char * cmd;
    const char * args;
    const char * desc;
    int (* func) (int argc, char ** argv);
} command;

const command cmds[] =
{
    {"help", "", "Prints this prompt", cmd_help},
    {"echo", "<string>", "Echoes string to console", cmd_echo},
};

int cmd_help(int argc, char ** argv)
{
    printf("Heap profiler commands:\n");
    for (int i = 0; i < ARRAY_LEN(cmds); ++i) {
        const command * cmd = &cmds[i];
        printf("    %s %s : %s\n", cmd->cmd, cmd->args, cmd->desc);
    }
    printf("\n");
    return 0;
}

int cmd_echo(int argc, char ** argv)
{
    int i;
    for (i = 1; i < argc - 1; ++i) {
        printf("%s ", argv[i]);
    }
    printf("%s\n", argv[i]);
    return 0;
}

static
void read_line(char * buffer)
{
    size_t pos = 0;
    char c;
    do {
        c = fgetc(stdin);
        if (c == '\n')
            break;
        buffer[pos++] = c;
    } while (pos < 255);
    buffer[pos] = '\0';
}

static
void prompt(void)
{
    printf("$ ");
}

static
int parse(char * buffer)
{
    char * argv[32];
    int argc = 0;
    
    const char * del = " \t";
    char * tok = strtok(buffer, del);
    while (tok != NULL) {
        argv[argc++] = tok;
        tok = strtok(NULL, del);
    }

    for (int i = 0; i < ARRAY_LEN(cmds); ++i) {
        if (strcmp(cmds[i].cmd, argv[0]) == 0) {
            return cmds[i].func(argc, argv);
        }
    }

    printf("Command \"%s\" not recognized\n", argv[0]);
    return -1;
}

void shell(void)
{
    static char buffer[BUFFER_SIZE];
    printf("Use \"help\" to list commands\n\n");
    while (1) {
        prompt();
        read_line(buffer);
        parse(buffer);
    }
}