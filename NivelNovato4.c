#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura que representa cada comodo da mansao
typedef struct Sala {
    char nome[50];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// Funcao que cria uma nova sala dinamicamente
Sala* criarSala(const char *nome) {
    Sala *novaSala = (Sala*) malloc(sizeof(Sala));
    if (novaSala == NULL) {
        printf("Erro ao alocar memoria para a sala.\n");
        exit(1);
    }
    strcpy(novaSala->nome, nome);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

// Funcao que permite explorar as salas da mansao
void explorarSalas(Sala *salaAtual) {
    char escolha;

    // Enquanto houver uma sala valida
    while (salaAtual != NULL) {
        printf("\nVoce esta na sala: %s\n", salaAtual->nome);

        // Caso a sala nao tenha mais caminhos (folha)
        if (salaAtual->esquerda == NULL && salaAtual->direita == NULL) {
            printf("Nao ha mais caminhos a seguir. Fim da exploracao!\n");
            break;
        }

        // Exibe opcoes de direcao
        printf("Deseja ir para:\n");
        if (salaAtual->esquerda != NULL)
            printf("  (e) %s\n", salaAtual->esquerda->nome);
        if (salaAtual->direita != NULL)
            printf("  (d) %s\n", salaAtual->direita->nome);
        printf("  (s) Sair da exploracao\n");
        printf("Escolha: ");
        scanf(" %c", &escolha);

        if (escolha == 'e' && salaAtual->esquerda != NULL) {
            salaAtual = salaAtual->esquerda;
        } else if (escolha == 'd' && salaAtual->direita != NULL) {
            salaAtual = salaAtual->direita;
        } else if (escolha == 's') {
            printf("Exploracao encerrada pelo jogador.\n");
            break;
        } else {
            printf("Opcao invalida! Tente novamente.\n");
        }
    }
}

// Funcao principal: monta o mapa da mansao e inicia o jogo
int main() {
    // Criacao manual das salas
    Sala *hall = criarSala("Hall de Entrada");
    Sala *salaEstar = criarSala("Sala de Estar");
    Sala *cozinha = criarSala("Cozinha");
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *jardim = criarSala("Jardim");
    Sala *porao = criarSala("Porao");
    Sala *quarto = criarSala("Quarto Principal");

    // Montagem da arvore (estrutura da mansao)
    hall->esquerda = salaEstar;
    hall->direita = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;

    cozinha->esquerda = porao;
    cozinha->direita = quarto;

    // Inicia a exploracao
    printf("=== DETECTIVE QUEST: EXPLORACAO DA MANSAO ===\n");
    explorarSalas(hall);

    // Libera memoria (boa pratica)
    free(hall);
    free(salaEstar);
    free(cozinha);
    free(biblioteca);
    free(jardim);
    free(porao);
    free(quarto);

    return 0;
}
