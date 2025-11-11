#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXNOME 64
#define MAXPISTA 128
#define HASH_SIZE 101   // numero primo razoavel para tabela hash

// =========================
// ESTRUTURAS
// =========================

// No da arvore binaria que representa cada sala da mansao
typedef struct Sala {
    char nome[MAXNOME];
    char pista[MAXPISTA]; // string vazia se nao houver pista
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// No da BST que armazena pistas coletadas (ordenadas)
typedef struct PistaNode {
    char pista[MAXPISTA];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

// No da lista encadeada usada em cada bucket da hash (chave: pista, valor: suspeito)
typedef struct HashNode {
    char pista[MAXPISTA];
    char suspeito[MAXNOME];
    struct HashNode *proximo;
} HashNode;

// Tabela hash (array de ponteiros para HashNode)
typedef struct HashTable {
    HashNode *buckets[HASH_SIZE];
} HashTable;

// =========================
// FUNCOES AUXILIARES
// =========================

// Converte string para minusculas (resultado em buffer, que deve ter espaco suficiente)
void strToLower(const char *src, char *buffer) {
    int i;
    for (i = 0; src[i] != '\0' && i < MAXPISTA-1; ++i)
        buffer[i] = (char) tolower((unsigned char) src[i]);
    buffer[i] = '\0';
}

// Calcula hash simples a partir da string (usando djb2 mod HASH_SIZE)
unsigned int hashFunc(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + (unsigned char)c; /* hash * 33 + c */
    return (unsigned int)(hash % HASH_SIZE);
}

// =========================
// FUNCOES PARA SALAS
// =========================

// criarSala() – cria dinamicamente um comodo (nome, pista)
Sala* criarSala(const char *nome, const char *pista) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("Erro: memoria insuficiente para criar sala.\n");
        exit(1);
    }
    strncpy(nova->nome, nome, MAXNOME-1);
    nova->nome[MAXNOME-1] = '\0';
    if (pista != NULL)
        strncpy(nova->pista, pista, MAXPISTA-1);
    else
        nova->pista[0] = '\0';
    nova->pista[MAXPISTA-1] = '\0';
    nova->esquerda = nova->direita = NULL;
    return nova;
}

// =========================
// FUNCOES PARA BST DE PISTAS
// =========================

// inserirPista() – insere a pista coletada na BST (ignora duplicatas e pistas vazias)
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (pista == NULL || strlen(pista) == 0) return raiz; // ignora
    if (raiz == NULL) {
        PistaNode *novo = (PistaNode*) malloc(sizeof(PistaNode));
        if (novo == NULL) { printf("Erro: memoria insuficiente (pista).\n"); exit(1); }
        strncpy(novo->pista, pista, MAXPISTA-1);
        novo->pista[MAXPISTA-1] = '\0';
        novo->esquerda = novo->direita = NULL;
        return novo;
    }
    int cmp = strcmp(pista, raiz->pista);
    if (cmp < 0) raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (cmp > 0) raiz->direita = inserirPista(raiz->direita, pista);
    // se igual, nao insere duplicata
    return raiz;
}

// exibirPistasInOrder - imprime as pistas em ordem alfabetica
void exibirPistasInOrder(PistaNode *raiz) {
    if (raiz == NULL) return;
    exibirPistasInOrder(raiz->esquerda);
    printf("- %s\n", raiz->pista);
    exibirPistasInOrder(raiz->direita);
}

// liberar memoria da BST de pistas
void liberarPistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

// =========================
// FUNCOES PARA TABELA HASH
// =========================

// inicializa a hash (todos os buckets com NULL)
void inicializarHash(HashTable *ht) {
    for (int i = 0; i < HASH_SIZE; ++i) ht->buckets[i] = NULL;
}

// inserirNaHash() – insere associacao pista -> suspeito
// se a pista ja existir, sobrescreve o suspeito
void inserirNaHash(HashTable *ht, const char *pista, const char *suspeito) {
    if (pista == NULL || strlen(pista) == 0) return;
    unsigned int h = hashFunc(pista);
    HashNode *cur = ht->buckets[h];
    while (cur != NULL) {
        if (strcmp(cur->pista, pista) == 0) {
            // atualiza suspeito
            strncpy(cur->suspeito, suspeito, MAXNOME-1);
            cur->suspeito[MAXNOME-1] = '\0';
            return;
        }
        cur = cur->proximo;
    }
    // nao existe: cria novo no no inicio da lista
    HashNode *novo = (HashNode*) malloc(sizeof(HashNode));
    if (novo == NULL) { printf("Erro: memoria insuficiente (hash node).\n"); exit(1); }
    strncpy(novo->pista, pista, MAXPISTA-1);
    novo->pista[MAXPISTA-1] = '\0';
    strncpy(novo->suspeito, suspeito, MAXNOME-1);
    novo->suspeito[MAXNOME-1] = '\0';
    novo->proximo = ht->buckets[h];
    ht->buckets[h] = novo;
}

// encontrarSuspeito() – consulta o suspeito correspondente a uma pista
// retorna NULL se nao encontrar
const char* encontrarSuspeito(HashTable *ht, const char *pista) {
    if (pista == NULL || strlen(pista) == 0) return NULL;
    unsigned int h = hashFunc(pista);
    HashNode *cur = ht->buckets[h];
    while (cur != NULL) {
        if (strcmp(cur->pista, pista) == 0)
            return cur->suspeito;
        cur = cur->proximo;
    }
    return NULL;
}

// libera memoria da hash table
void liberarHash(HashTable *ht) {
    for (int i = 0; i < HASH_SIZE; ++i) {
        HashNode *cur = ht->buckets[i];
        while (cur != NULL) {
            HashNode *tmp = cur;
            cur = cur->proximo;
            free(tmp);
        }
        ht->buckets[i] = NULL;
    }
}

// =========================
// EXPLORACAO DA MANSAO
// =========================

// explorarSalas() – navega pela arvore e ativa o sistema de pistas
// entrada: salaAtual (ponteiro para Sala), raizPistas (ponteiro para ponteiro da BST), ht (tabela hash com relacao pista->suspeito)
// ao visitar, exibe pista (se existir) e adiciona na BST automaticamente
void explorarSalas(Sala *salaAtual, PistaNode **raizPistas, HashTable *ht) {
    char escolha;
    while (salaAtual != NULL) {
        printf("\nVocê esta na sala: %s\n", salaAtual->nome);
        if (strlen(salaAtual->pista) > 0) {
            printf("Voce encontrou uma pista: \"%s\"\n", salaAtual->pista);
            *raizPistas = inserirPista(*raizPistas, salaAtual->pista);
            // opcional: mostrar suspeito ligado a esta pista
            const char *sus = encontrarSuspeito(ht, salaAtual->pista);
            if (sus != NULL) {
                printf("  (essa pista esta ligada ao suspeito: %s)\n", sus);
            } else {
                printf("  (nenhum suspeito associado a esta pista no momento)\n");
            }
        } else {
            printf("Nao ha pista nesta sala.\n");
        }

        // opcoes
        printf("\nEscolha seu proximo caminho:\n");
        if (salaAtual->esquerda != NULL) printf("  (e) Esquerda -> %s\n", salaAtual->esquerda->nome);
        if (salaAtual->direita != NULL)  printf("  (d) Direita  -> %s\n", salaAtual->direita->nome);
        printf("  (s) Sair e ir ao julgamento\n");
        printf("Opcao: ");
        scanf(" %c", &escolha);
        // limpar resto da linha
        while (getchar() != '\n');

        if (escolha == 'e' && salaAtual->esquerda != NULL) salaAtual = salaAtual->esquerda;
        else if (escolha == 'd' && salaAtual->direita != NULL) salaAtual = salaAtual->direita;
        else if (escolha == 's') {
            printf("\nExploracao encerrada. Seguindo para o julgamento...\n");
            break;
        } else {
            printf("Opcao invalida ou caminho inexistente. Tente novamente.\n");
        }
    }
}

// =========================
// AVALIACAO FINAL
// =========================

// Funcao recursiva que conta quantas pistas da BST apontam para 'suspeitoAlvo'
// Para cada pista no no, busca na hash quem eh o suspeito e compara (case-insensitive)
int contarPistasParaSuspeitoRec(PistaNode *raiz, HashTable *ht, const char *suspeitoAlvo) {
    if (raiz == NULL) return 0;
    int total = 0;
    // esquerda
    total += contarPistasParaSuspeitoRec(raiz->esquerda, ht, suspeitoAlvo);
    // atual
    const char *sus = encontrarSuspeito(ht, raiz->pista);
    if (sus != NULL) {
        char susLower[MAXNOME], alvoLower[MAXNOME];
        strToLower(sus, susLower);
        strToLower(suspeitoAlvo, alvoLower);
        if (strcmp(susLower, alvoLower) == 0) total++;
    }
    // direita
    total += contarPistasParaSuspeitoRec(raiz->direita, ht, suspeitoAlvo);
    return total;
}

// verificarSuspeitoFinal() – conduz a fase de julgamento final
// requisito: ao menos 2 pistas devem apontar para o suspeito acusado
void verificarSuspeitoFinal(PistaNode *raizPistas, HashTable *ht) {
    if (raizPistas == NULL) {
        printf("\nNenhuma pista foi coletada. Nao ha como acusar alguem com base em evidencias.\n");
        return;
    }

    printf("\n===== PISTAS COLETADAS =====\n");
    exibirPistasInOrder(raizPistas);

    char acusado[MAXNOME];
    printf("\nDigite o nome do suspeito que deseja acusar: ");
    if (fgets(acusado, MAXNOME, stdin) == NULL) {
        printf("Entrada invalida.\n");
        return;
    }
    // remover newline
    acusado[strcspn(acusado, "\n")] = '\0';

    if (strlen(acusado) == 0) {
        printf("Nenhum nome informado. Acusacao abortada.\n");
        return;
    }

    int cont = contarPistasParaSuspeitoRec(raizPistas, ht, acusado);
    printf("\nPistas que apontam para %s: %d\n", acusado, cont);

    if (cont >= 2) {
        printf("\nResultado: Ha evidencias suficientes! %s eh considerado(a) culpado(a).\n", acusado);
    } else {
        printf("\nResultado: Nao ha evidencias suficientes para sustentar a acusacao contra %s.\n", acusado);
    }
}

// =========================
// FUNCAO PRINCIPAL (main)
// =========================

int main() {
    // Montagem manual do mapa da mansao (arvore binaria de Salas)
    Sala *hall = criarSala("Hall de Entrada", "pegadas_no_chao");
    Sala *salaEstar = criarSala("Sala de Estar", "livro_aberto_sobre_enigmas");
    Sala *cozinha = criarSala("Cozinha", "faca_limpa_na_mesa");
    Sala *biblioteca = criarSala("Biblioteca", "mapa_rasgado_com_x");
    Sala *jardim = criarSala("Jardim", "vaso_quebrado");
    Sala *porao = criarSala("Porao", "cofre_trancado_com_simbolos");
    Sala *quarto = criarSala("Quarto Principal", "retrato_antigo_quebrado");

    // ligar as salas
    hall->esquerda = salaEstar;
    hall->direita  = cozinha;
    salaEstar->esquerda = biblioteca;
    salaEstar->direita  = jardim;
    cozinha->esquerda = porao;
    cozinha->direita  = quarto;

    // Criar e popular a tabela hash pista -> suspeito
    HashTable ht;
    inicializarHash(&ht);
    // A associacao e definida aqui (simplificacao: pistas estaticas associadas a suspeitos)
    inserirNaHash(&ht, "pegadas_no_chao", "Sr. Silva");
    inserirNaHash(&ht, "livro_aberto_sobre_enigmas", "Sra. Almeida");
    inserirNaHash(&ht, "faca_limpa_na_mesa", "Sr. Silva");
    inserirNaHash(&ht, "mapa_rasgado_com_x", "Dr. Costa");
    inserirNaHash(&ht, "vaso_quebrado", "Sra. Almeida");
    inserirNaHash(&ht, "cofre_trancado_com_simbolos", "Dr. Costa");
    inserirNaHash(&ht, "retrato_antigo_quebrado", "Sr. Oliveira");

    // BST de pistas coletadas (inicialmente vazia)
    PistaNode *raizPistas = NULL;

    // Inicio do jogo
    printf("=== DETECTIVE QUEST: JULGAMENTO FINAL (COLETA, ASSOCIACAO E ACUSACAO) ===\n");
    printf("Instrucoes: em cada sala escolha 'e' para esquerda, 'd' para direita ou 's' para sair.\n");

    explorarSalas(hall, &raizPistas, &ht);

    // Fase de julgamento: exibe pistas coletadas e pede o acusado
    verificarSuspeitoFinal(raizPistas, &ht);

    // Liberar memoria (boas praticas)
    liberarPistas(raizPistas);
    liberarHash(&ht);
    free(hall); free(salaEstar); free(cozinha); free(biblioteca); free(jardim); free(porao); free(quarto);

    printf("\nFim do jogo. Obrigado por jogar, detetive!\n");
    return 0;
}
