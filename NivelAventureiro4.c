#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================
// ESTRUTURAS DE DADOS
// ============================

// Estrutura para representar uma sala da mansao
typedef struct Sala {
    char nome[50];
    char pista[100];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// Estrutura para representar um no da arvore de pistas (BST)
typedef struct PistaNode {
    char pista[100];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

// ============================
// FUNCOES PARA SALAS
// ============================

// Cria dinamicamente uma sala com nome e pista
Sala* criarSala(const char *nome, const char *pista) {
    Sala *novaSala = (Sala*) malloc(sizeof(Sala));
    if (novaSala == NULL) {
        printf("Erro ao alocar memoria para a sala.\n");
        exit(1);
    }
    strcpy(novaSala->nome, nome);
    strcpy(novaSala->pista, pista);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

// ============================
// FUNCOES PARA A ARVORE DE PISTAS (BST)
// ============================

// Insere uma pista na BST em ordem alfabetica
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (pista == NULL || strlen(pista) == 0)
        return raiz; // ignora pistas vazias

    if (raiz == NULL) {
        PistaNode *novo = (PistaNode*) malloc(sizeof(PistaNode));
        strcpy(novo->pista, pista);
        novo->esquerda = NULL;
        novo->direita = NULL;
        return novo;
    }

    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->direita = inserirPista(raiz->direita, pista);

    return raiz;
}

// Exibe as pistas em ordem alfabetica (in-order)
void exibirPistas(PistaNode *raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda);
        printf("- %s\n", raiz->pista);
        exibirPistas(raiz->direita);
    }
}

// Libera memoria da arvore de pistas
void liberarPistas(PistaNode *raiz) {
    if (raiz != NULL) {
        liberarPistas(raiz->esquerda);
        liberarPistas(raiz->direita);
        free(raiz);
    }
}

// ============================
// FUNCAO DE EXPLORACAO
// ============================

// Permite explorar as salas da mansao e coletar pistas
void explorarSalasComPistas(Sala *salaAtual, PistaNode **raizPistas) {
    char escolha;

    while (salaAtual != NULL) {
        printf("\nVoce esta na sala: %s\n", salaAtual->nome);

        // Coleta de pista, se houver
        if (strlen(salaAtual->pista) > 0) {
            printf("Voce encontrou uma pista: \"%s\"\n", salaAtual->pista);
            *raizPistas = inserirPista(*raizPistas, salaAtual->pista);
        } else {
            printf("Nao ha pistas nesta sala.\n");
        }

        // Exibe opcoes de navegacao
        printf("\nEscolha seu proximo caminho:\n");
        if (salaAtual->esquerda != NULL)
            printf("  (e) Ir para %s\n", salaAtual->esquerda->nome);
        if (salaAtual->direita != NULL)
            printf("  (d) Ir para %s\n", salaAtual->direita->nome);
        printf("  (s) Sair da exploracao\n");
        printf("Opcao: ");
        scanf(" %c", &escolha);

        if (escolha == 'e' && salaAtual->esquerda != NULL) {
            salaAtual = salaAtual->esquerda;
        } else if (escolha == 'd' && salaAtual->direita != NULL) {
            salaAtual = salaAtual->direita;
        } else if (escolha == 's') {
            printf("\nEncerrando exploracao...\n");
            break;
        } else {
            printf("Opcao invalida! Tente novamente.\n");
        }
    }
}

// ============================
// FUNCAO PRINCIPAL
// ============================

int main() {
    // Criacao manual do mapa da mansao (arvore binaria)
    Sala *hall = criarSala("Hall de Entrada", "Pegadas misteriosas no chao");
    Sala *salaEstar = criarSala("Sala de Estar", "Um livro aberto sobre enigmas");
    Sala *cozinha = criarSala("Cozinha", "Uma faca limpa sobre a mesa");
    Sala *biblioteca = criarSala("Biblioteca", "Um mapa rasgado com uma marca X");
    Sala *jardim = criarSala("Jardim", "");
    Sala *porao = criarSala("Porao", "Um cofre trancado com simbolos estranhos");
    Sala *quarto = criarSala("Quarto Principal", "Um retrato antigo quebrado");

    // Conexao entre as salas
    hall->esquerda = salaEstar;
    hall->direita = cozinha;
    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;
    cozinha->esquerda = porao;
    cozinha->direita = quarto;

    // Arvore de pistas (inicialmente vazia)
    PistaNode *raizPistas = NULL;

    // Inicio do jogo
    printf("=== DETECTIVE QUEST: COLETA DE PISTAS ===\n");
    explorarSalasComPistas(hall, &raizPistas);

    // Exibe pistas coletadas
    printf("\n===== PISTAS COLETADAS (ORDEM ALFABETICA) =====\n");
    if (raizPistas == NULL)
        printf("Nenhuma pista foi coletada.\n");
    else
        exibirPistas(raizPistas);

    // Libera memoria
    free(hall);
    free(salaEstar);
    free(cozinha);
    free(biblioteca);
    free(jardim);
    free(porao);
    free(quarto);
    liberarPistas(raizPistas);

    printf("\nFim da investigacao. Ate a proxima, detetive!\n");
    return 0;
}
