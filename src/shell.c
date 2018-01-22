#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <command.h>

#define BUFFER_SIZE 256


int cmd_help(int argc, char ** argv);
int cmd_echo(int argc, char ** argv);
int cmd_set_impl(int argc, char ** argv);
int cmd_stats(int argc, char ** argv);
int cmd_reset(int argc, char ** argv);
int cmd_benchmark(int argc, char ** argv);

// shell stuff

#define ARRAY_LEN(x) (sizeof(x)/sizeof(x[0]))

static
const command cmds[] =
{
    {"help", "", "Prints this prompt", cmd_help},
    {"echo", "<string>", "Echoes string to console", cmd_echo},
    {"set-impl", "<implementation>", "Sets the allocator implementation. Will reset heap and stats.", cmd_set_impl},
    {"stats", "", "Print name and stats of current implementation since last set", cmd_stats},
    {"reset", "", "Reinitializes the heap of the current implementation", cmd_reset},
    {"bench", "<benchmark name>", "Runs a benchmark. Resets the heap before hand.", cmd_benchmark},
};

int cmd_stats(int argc, char ** argv)
{
    malloc_print_stats();
    return 0;
}

int cmd_reset(int argc, char ** argv)
{
    malloc_reset();
    puts("Reset current implementation");
    return 0;
}

int cmd_help(int argc, char ** argv)
{
    printf("Heap profiler commands:\n");
    print_commands(cmds, ARRAY_LEN(cmds));
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
        if (c == 0x7F) {
            if (pos > 0) {
                --pos;
                fputc(c, stdout);
            }
        } else {
            buffer[pos++] = c;
            fputc(c, stdout);
        }
    } while (pos < 255);
    buffer[pos] = '\0';
    
    fputc('\n', stdout);
}

static
void prompt(void)
{
    printf("\n$ ");
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

static char buffer[BUFFER_SIZE];
void repl(void)
{
    int run = 1;
    while (run) {
        prompt();
        read_line(buffer);
        parse(buffer);
    }
}

void shell(void)
{
    
    puts("Use \"help\" to list commands\n");
    
    repl();
}