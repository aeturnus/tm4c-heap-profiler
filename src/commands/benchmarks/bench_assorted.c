#include <stdio.h>t
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <malloc.h>
#include <stdint.h>

#include <Random.h>

#include <btn/vector.h>
#include <btn/tokenizer.h>

void benchmark_vector(uint32_t actions)
{
    printf("Pushing %d uint32_t's into a vector\n", actions);
    vector(uint32_t) v;

    vector_ctor(&v, sizeof(uint32_t), NULL, NULL);

    for (uint32_t i = 0; i < actions; ++i) {
        uint32_t val = Random();
        vector_push_back(&v, &val);
    }

    uint32_t * arr = vector_to_array(&v);

    free(arr);
    vector_dtor(&v);
}

void benchmark_fixed(uint32_t size, uint32_t actions)
{
    printf("Allocating %dB objects %d times\n", size, actions);
    void ** ptrs = (void **) malloc(sizeof(void *) * actions);

    for (uint32_t i = 0; i < actions; ++i) {
        ptrs[i] = (void *) malloc(size);
    }

    for (uint32_t i = 0; i < actions; ++i) {
        free(ptrs[i]);
    }

    free(ptrs);
}

const char * lorem = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
void benchmark_tokenize(void)
{
    puts("Tokenizing the \"Lorem ipsum\" and copying strings to array");
    tokenizer tok;
    tokenizer_ctor(&tok, lorem, " ");
    
    char ** tokens = tokenizer_tokens_copy(&tok);
    size_t num_tok = tokenizer_num_tokens(&tok);
    for (size_t i = 0; i < num_tok; ++i) {
        free(tokens[i]);
    }
    free(tokens);

    tokenizer_dtor(&tok);
}