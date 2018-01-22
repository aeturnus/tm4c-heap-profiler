#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <command.h>

#define BUFFER_SIZE 128


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

typedef enum _ansi_code
{
    ANSI_NONE,
    ANSI_UP,
    ANSI_DOWN,
    ANSI_FORWARD,
    ANSI_BACKWARD
} ansi_code;

// interpret ansi coming in
static
ansi_code interpret_ansi(void)
{
    char c = fgetc(stdin);
    if (c != '[')
        return ANSI_NONE;
    
    c = fgetc(stdin);
    switch (c) {
    case 'A':
        return ANSI_UP;
    case 'B':
        return ANSI_DOWN;
    case 'C':
        return ANSI_FORWARD;
    case 'D':
        return ANSI_BACKWARD;
    }
    
    return ANSI_NONE;
}

#define HISTORY_SIZE 16

static char history[HISTORY_SIZE][BUFFER_SIZE];
static int put_idx = 0;
static int put_cnt = 0;

#define off(x, n) ((x + n) % HISTORY_SIZE)

static
void push_buffer(char * buffer)
{
    strcpy(history[put_idx], buffer);
    put_idx = off(put_idx, +1);
    put_cnt++;
}

static
void get_buffer(int offset, char * buffer)
{
    int idx = off(put_idx, -offset-1);
    strcpy(buffer, history[idx]);
}

static
void backspace(int amt)
{
    for (; amt > 0; --amt) {
        fputc(0x7F, stdout);
    }
}

static
void read_line(char * buffer)
{
    // form circular buffer
    int history_pos = -1;
    
    size_t pos = 0;
    char c;
    do {
        c = fgetc(stdin);
        
        if (c == 0x1B) {
            ansi_code code = interpret_ansi();
            
            if (code == ANSI_UP) {
                ++history_pos;
                if (history_pos < HISTORY_SIZE && history_pos < put_cnt) {
                    backspace(pos);
                    get_buffer(history_pos, buffer);
                    pos = strlen(buffer);
                    printf("%s", buffer);
                } else {
                    --history_pos;
                }
            } else if (code == ANSI_DOWN) {
                --history_pos;
                if (history_pos >= 0) {
                    backspace(pos);
                    get_buffer(history_pos, buffer);
                    pos = strlen(buffer);
                    printf("%s", buffer);
                } else if (history_pos == -1) {
                    backspace(pos);
                    pos = 0;
                    buffer[pos] = '\0';
                } else {
                    history_pos = -1;
                }
            }
            continue;
        }
        
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
    push_buffer(buffer);
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