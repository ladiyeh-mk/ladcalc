#define DYNARR_IMPLEMENTATION
#include "../utils/dynarr.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <ctype.h>

typedef enum {

    TK_UNKNOWN,

    TK_NUMBER, TK_ADDITION, TK_SUBTRACTION,
    TK_MULTIPLICATION, TK_DIVISION, TK_OPENPARENTHESIS,
    TK_CLOSEPARENTHESIS,

} TokenType;

typedef struct {

    TokenType type;
    char *slice;

} Lexime;

void tokenizer_lex(Lexime **lexime_out_ptr, const char *expression) {
    if (!lexime_out_ptr || !*lexime_out_ptr || !expression) return;

    size_t expr_len = strlen(expression);
    for (size_t current_index = 0; current_index < expr_len; current_index++) {
        char ch = expression[current_index];
        Lexime lex = {0};

        if (ch == ' ' || ch == '\n' || ch == '\t') { continue; }

        if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')') {
            TokenType t = TK_UNKNOWN;
            if (ch == '+') t = TK_ADDITION;
            if (ch == '-') t = TK_SUBTRACTION;
            if (ch == '*') t = TK_MULTIPLICATION;
            if (ch == '/') t = TK_DIVISION;
            if (ch == '(') t = TK_OPENPARENTHESIS;
            if (ch == ')') t = TK_CLOSEPARENTHESIS;

            lex = (Lexime) {
                .type  = t,
                .slice = dynarr_create(sizeof(char), 2),
            };
            lex.slice[0] = ch;
            lex.slice[1] = '\0';
        }
        else if (isdigit(ch)) {
            lex = (Lexime) {
                .type  = TK_NUMBER,
                .slice = dynarr_create(sizeof(char), 2),
            };
            
            while (ch != '+' && ch != '-' && ch != '*' && ch != '/' && ch != '(' && ch != ')') {
                
            }
        }
        else {
            continue;
        }

        dynarr_append(*lexime_out_ptr, lex);
    }
}

int main(void) {
    const char *expression = "3 + 4 - 2 * 5 / 1 + (3 * 3)";
    printf("Lexing expression: \"%s\"\n\n", expression);

    // 1. Create the dynamic array
    Lexime *tokens = dynarr_create(sizeof(Lexime), 7);
    if (!tokens) {
        fprintf(stderr, "Failed to allocate token array.\n");
        return 1;
    }

    // 2. Run the tokenizer (passing &tokens so tokenizer_lex receives Lexime **)
    tokenizer_lex(&tokens, expression);

    // 3. Print the token stream
    size_t count = dynarr_peek_count(tokens);
    printf("Total tokens found: %zu\n", count);
    printf("----------------------------------\n");
    for (size_t i = 0; i < count; i++) {
        printf("Index %zu -> Type: %d | Slice: \"%s\"\n", 
               i, tokens[i].type, tokens[i].slice);
    }
    printf("----------------------------------\n");

    // 4. Clean up allocated slice strings and the dynamic array itself
    for (size_t i = 0; i < count; i++) {
        if (tokens[i].slice) {
            dynarr_destroy(tokens[i].slice); // Assuming your slices were created with dynarr_create/malloc
        }
    }
    dynarr_destroy(tokens);

    printf("=== Test completed ===\n");
    return 0;
}