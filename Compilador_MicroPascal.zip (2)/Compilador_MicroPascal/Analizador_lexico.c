#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN 100
#define MAX_TOKENS 1000

typedef struct {
    char nome[MAX_LEN];
    char lexema[MAX_LEN];
    int linha;
    int coluna;
} Token;

typedef struct {
    char simbolo[MAX_LEN];
} TabelaSimbolos;


const char *palavrasReservadas[] = {"program", "var", "integer", "real", "begin", "end", "if", "then", "else", "while", "do"};
const char *simbolos[] = {"{", "}", ";", ",", "(", ")", "=", ">", "<", ">=", "<=", "<>", "+", "-", "*", "/"};


int ePalavraReservada(char *lexema) {
    for (int i = 0; i < sizeof(palavrasReservadas) / sizeof(palavrasReservadas[0]); i++) {
        if (strcmp(lexema, palavrasReservadas[i]) == 0)
            return 1;
    }
    return 0;
}
int eSimbolo(char *lexema) {
    for (int i = 0; i < sizeof(simbolos) / sizeof(simbolos[0]); i++) {
        if (strcmp(lexema, simbolos[i]) == 0)
            return 1;
    }
    return 0;
}


void adicionarTabelaSimbolos(TabelaSimbolos *ts, int *tamanhoTS, char *lexema) {
    for (int i = 0; i < *tamanhoTS; i++) {
        if (strcmp(ts[i].simbolo, lexema) == 0)
            return;
    }

    strcpy(ts[*tamanhoTS].simbolo, lexema);
    (*tamanhoTS)++;
}

void detectarErro(char *erro, int linha, int coluna) {
    printf("Erro lexico na linha %d, coluna %d: %s\n", linha, coluna, erro);
}

void reconhecerTokens(FILE *entrada, FILE *saida, TabelaSimbolos *ts, int *tamanhoTS) {
    char linha[MAX_LEN];
    int numeroLinha = 0;
    Token tokens[MAX_TOKENS];
    int numTokens = 0;

    while (fgets(linha, sizeof(linha), entrada)) {
        numeroLinha++;
        int i = 0;

        while (linha[i] != '\0' && linha[i] != '\n') { // Adicionando verificação para nova linha
            if (isspace(linha[i])) {
                i++;
                continue;
            }

            char lexema[MAX_LEN] = {0};
            int j = 0;

            // Análise de identificadores e palavras reservadas
            if (isalpha(linha[i])) {
                while (isalnum(linha[i])) {
                    lexema[j++] = linha[i++];
                }
                lexema[j] = '\0';
                if (ePalavraReservada(lexema)) {
                    strcpy(tokens[numTokens].nome, "PALAVRA_RESERVADA");
                } else {
                    strcpy(tokens[numTokens].nome, "ID");
                    adicionarTabelaSimbolos(ts, tamanhoTS, lexema);
                }
                strcpy(tokens[numTokens].lexema, lexema);
                tokens[numTokens].linha = numeroLinha;
                tokens[numTokens].coluna = i - j; // Corrigido
                numTokens++;
                continue;
            }

            // Análise de números
            if (isdigit(linha[i])) {
                while (isdigit(linha[i])) {
                    lexema[j++] = linha[i++];
                }
                lexema[j] = '\0';
                strcpy(tokens[numTokens].nome, "NUMERO");
                strcpy(tokens[numTokens].lexema, lexema);
                tokens[numTokens].linha = numeroLinha;
                tokens[numTokens].coluna = i - j; // Corrigido
                numTokens++;
                continue;
            }

            // Analisando caracteres como símbolos
            lexema[j++] = linha[i++];
            lexema[j] = '\0';
            if (eSimbolo(lexema)) {
                strcpy(tokens[numTokens].nome, "SIMBOLO");
                strcpy(tokens[numTokens].lexema, lexema);
                tokens[numTokens].linha = numeroLinha;
                tokens[numTokens].coluna = i - 1; // Corrigido
                numTokens++;
            } else {
                detectarErro("Caractere inválido", numeroLinha, i);
            }
        }
    }

    // Escrevendo os tokens reconhecidos no arquivo de saída
    for (int i = 0; i < numTokens; i++) {
        fprintf(saida, "<%s, %s> [linha: %d, coluna: %d]\n", tokens[i].nome, tokens[i].lexema, tokens[i].linha, tokens[i].coluna);
    }
}
