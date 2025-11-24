#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_NOME 50

// -----------------------------------------------------------
// Estrutura Sala: representa cada cômodo da mansão
// -----------------------------------------------------------
typedef struct sala {
    char nome[TAM_NOME];
    struct sala *esq;   // Caminho à esquerda
    struct sala *dir;   // Caminho à direita
} Sala;

// -----------------------------------------------------------
// criarSala()
// Cria dinamicamente um nó da árvore representando uma sala
// -----------------------------------------------------------
Sala* criarSala(const char *nome) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("Erro ao alocar memória.\n");
        exit(1);
    }

    strcpy(nova->nome, nome);
    nova->esq = NULL;
    nova->dir = NULL;

    return nova;
}

// -----------------------------------------------------------
// explorarSalas()
// Permite ao jogador navegar interativamente pela mansão
// Começa no Hall de Entrada e segue até um nó-folha
// -----------------------------------------------------------
void explorarSalas(Sala *atual) {
    char opcao;

    while (atual != NULL) {

        // Exibe sala atual
        printf("\nVocê está em: %s\n", atual->nome);

        // Verifica se chegou ao final (nó-folha)
        if (atual->esq == NULL && atual->dir == NULL) {
            printf("Você chegou ao fim do caminho! Exploração encerrada.\n");
            return;
        }

        // Opções disponíveis
        printf("Escolha um caminho:\n");
        if (atual->esq != NULL)
            printf("  (e) Ir para a esquerda -> %s\n", atual->esq->nome);
        if (atual->dir != NULL)
            printf("  (d) Ir para a direita  -> %s\n", atual->dir->nome);
        printf("  (s) Sair da exploração\n");

        printf("Digite sua escolha: ");
        scanf(" %c", &opcao);

        // Processa escolha do jogador
        switch (opcao) {
            case 'e':
                if (atual->esq != NULL)
                    atual = atual->esq;
                else
                    printf("Não existe caminho à esquerda!\n");
                break;

            case 'd':
                if (atual->dir != NULL)
                    atual = atual->dir;
                else
                    printf("Não existe caminho à direita!\n");
                break;

            case 's':
                printf("Exploração encerrada pelo jogador.\n");
                return;

            default:
                printf("Opção inválida! Digite e, d ou s.\n");
        }
    }
}

// -----------------------------------------------------------
// main()
// Monta a árvore binária da mansão e inicia a exploração
// -----------------------------------------------------------
int main() {

    // Criação do mapa da mansão (árvore binária estática)
    Sala *hall = criarSala("Hall de Entrada");

    hall->esq = criarSala("Sala de Estar");
    hall->dir = criarSala("Cozinha");

    hall->esq->esq = criarSala("Jardim");
    hall->esq->dir = criarSala("Biblioteca");

    hall->dir->esq = criarSala("Despensa");
    hall->dir->dir = criarSala("Porão");

    printf("=== DETECTIVE QUEST: EXPLORANDO A MANSÃO ===\n");

    // Começa a exploração pelo Hall
    explorarSalas(hall);

    return 0;
}
