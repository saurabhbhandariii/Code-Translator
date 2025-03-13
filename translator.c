#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    TOKEN_NUMBER,
    TOKEN_IDENTIFIER,
    TOKEN_OPERATOR,
    TOKEN_PRINT,
    TOKEN_ASSIGN,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_FOR,
    TOKEN_WHILE,
    TOKEN_DEF,
    TOKEN_COLON,
    TOKEN_INDENT,
    TOKEN_NEWLINE,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char value[100];
} Token;

Token* lexer(const char* code, int* token_count) {
    Token* tokens = malloc(1000 * sizeof(Token));
    int i = 0, j = 0;
    while (code[i] != '\0') {
        if (isspace(code[i])) {
            if (code[i] == '\n') {
                tokens[j].type = TOKEN_NEWLINE;
                tokens[j].value[0] = '\n';
                tokens[j].value[1] = '\0';
                j++;
            }
            i++;
        } else if (isdigit(code[i])) {
            tokens[j].type = TOKEN_NUMBER;
            int k = 0;
            while (isdigit(code[i])) {
                tokens[j].value[k++] = code[i++];
            }
            tokens[j].value[k] = '\0';
            j++;
        } else if (isalpha(code[i])) {
            tokens[j].type = TOKEN_IDENTIFIER;
            int k = 0;
            while (isalnum(code[i])) {
                tokens[j].value[k++] = code[i++];
            }
            tokens[j].value[k] = '\0';
            if (strcmp(tokens[j].value, "print") == 0) {
                tokens[j].type = TOKEN_PRINT;
            } else if (strcmp(tokens[j].value, "if") == 0) {
                tokens[j].type = TOKEN_IF;
            } else if (strcmp(tokens[j].value, "else") == 0) {
                tokens[j].type = TOKEN_ELSE;
            } else if (strcmp(tokens[j].value, "for") == 0) {
                tokens[j].type = TOKEN_FOR;
            } else if (strcmp(tokens[j].value, "while") == 0) {
                tokens[j].type = TOKEN_WHILE;
            } else if (strcmp(tokens[j].value, "def") == 0) {
                tokens[j].type = TOKEN_DEF;
            }
            j++;
        } else if (code[i] == '=') {
            tokens[j].type = TOKEN_ASSIGN;
            tokens[j].value[0] = code[i++];
            tokens[j].value[1] = '\0';
            j++;
        } else if (code[i] == ':') {
            tokens[j].type = TOKEN_COLON;
            tokens[j].value[0] = code[i++];
            tokens[j].value[1] = '\0';
            j++;
        } else {
            i++;
        }
    }
    tokens[j].type = TOKEN_EOF;
    *token_count = j;
    return tokens;
}


void parser(Token* tokens, int token_count, FILE* output) {
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].type == TOKEN_PRINT) {
            fprintf(output, "console.log(%s);\n", tokens[i + 1].value);
            i++;
        } else if (tokens[i].type == TOKEN_IDENTIFIER && tokens[i + 1].type == TOKEN_ASSIGN) {
            fprintf(output, "let %s = %s;\n", tokens[i].value, tokens[i + 2].value);
            i += 2;
        } else if (tokens[i].type == TOKEN_IF) {
            fprintf(output, "if (%s) {\n", tokens[i + 1].value);
            i += 2;
        } else if (tokens[i].type == TOKEN_ELSE) {
            fprintf(output, "} else {\n");
        } else if (tokens[i].type == TOKEN_FOR) {
            fprintf(output, "for (let %s = %s; %s < %s; %s++) {\n",
                    tokens[i + 1].value, tokens[i + 3].value,
                    tokens[i + 1].value, tokens[i + 5].value,
                    tokens[i + 1].value);
            i += 6;
        } else if (tokens[i].type == TOKEN_WHILE) {
            fprintf(output, "while (%s) {\n", tokens[i + 1].value);
            i += 2;
        } else if (tokens[i].type == TOKEN_DEF) {
            fprintf(output, "function %s(%s) {\n", tokens[i + 1].value, tokens[i + 3].value);
            i += 4;
        } else if (tokens[i].type == TOKEN_NEWLINE) {
            fprintf(output, "\n");
        } else if (tokens[i].type == TOKEN_COLON) {

        }
    }
}


int main() {
    FILE* input_file = fopen("main.py", "r");
    if (!input_file) {
        perror("Error opening file");
        return 1;
    }

    fseek(input_file, 0, SEEK_END);
    long file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);
    char* python_code = malloc(file_size + 1);
    fread(python_code, 1, file_size, input_file);
    python_code[file_size] = '\0';
    fclose(input_file);

    int token_count;
    Token* tokens = lexer(python_code, &token_count);

    FILE* output_file = fopen("output.js", "w");
    if (!output_file) {
        perror("Error creating output file");
        free(python_code);
        free(tokens);
        return 1;
    }

    parser(tokens, token_count, output_file);
    fclose(output_file);

    free(python_code);
    free(tokens);

    printf("Translation complete. Check output.js for the generated JavaScript code.\n");
    return 0;
}
