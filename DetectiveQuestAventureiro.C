#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_NOME 50
#define TAM_PISTA 100

// ==========================================================
// ESTRUTURAS
// ==========================================================

// Estrutura de um cômodo da mansão (Árvore Binária)
typedef struct sala {
    char nome[TAM_NOME];
    char pista[TAM_PISTA];     // pista opcional do cômodo
    struct sala *esq;
    struct sala *dir;
} Sala;

// Estrutura da árvore BST que guarda as pistas coletadas
typedef struct pistaNode {
    char conteudo[TAM_PISTA];
    struct pistaNode *esq;
    struct pistaNode *dir;
} PistaNode;


// ==========================================================
// FUNÇÃO: criarSala
// Cria dinamicamente um cômodo com nome e pista opcional
// ==========================================================
Sala* criarSala(const char *nome, const char *pista) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) {
        printf("Erro ao alocar memória para sala.\n");
        exit(1);
    }

    strcpy(nova->nome, nome);
    if (pista != NULL)
        strcpy(nova->pista, pista);
    else
        strcpy(nova->pista, "");

    nova->esq = NULL;
    nova->dir = NULL;
    return nova;
}


// ==========================================================
// FUNÇÃO: inserirPista
// Insere uma pista na BST de forma ordenada (alfabética)
// ==========================================================
PistaNode* inserirPista(PistaNode *raiz, const char *conteudo) {
    if (raiz == NULL) {
        PistaNode *novo = (PistaNode*) malloc(sizeof(PistaNode));
        if (!novo) {
            printf("Erro de memória ao inserir pista.\n");
            exit(1);
        }
        strcpy(novo->conteudo, conteudo);
        novo->esq = novo->dir = NULL;
        return novo;
    }

    if (strcmp(conteudo, raiz->conteudo) < 0)
        raiz->esq = inserirPista(raiz->esq, conteudo);
    else
        raiz->dir = inserirPista(raiz->dir, conteudo);

    return raiz;
}


// ==========================================================
// FUNÇÃO: exibirPistas
// Exibe todas as pistas em ordem alfabética (in-ordem)
// ==========================================================
void exibirPistas(PistaNode *raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esq);
        printf("- %s\n", raiz->conteudo);
        exibirPistas(raiz->dir);
    }
}


// ==========================================================
// FUNÇÃO: explorarSalasComPistas
// Controla a navegação do jogador e coleta automática de pistas
// ==========================================================
void explorarSalasComPistas(Sala *atual, PistaNode **pistas) {
    char opcao;

    while (atual != NULL) {

        printf("\nVocê está em: %s\n", atual->nome);

        // Coleta automática da pista (se existir)
        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada: \"%s\"\n", atual->pista);
            *pistas = inserirPista(*pistas, atual->pista);
        }

        // Opções de navegação
        printf("\nEscolha um caminho:\n");
        if (atual->esq != NULL)
            printf("  (e) Ir para a esquerda  -> %s\n", atual->esq->nome);
        if (atual->dir != NULL)
            printf("  (d) Ir para a direita   -> %s\n", atual->dir->nome);
        printf("  (s) Sair da exploração\n");

        printf("Digite sua opção: ");
        scanf(" %c", &opcao);

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
                printf("\nExploração encerrada pelo jogador.\n");
                return;

            default:
                printf("Opção inválida! Use e, d ou s.\n");
        }
    }
}


// ==========================================================
// FUNÇÃO PRINCIPAL
// Monta o mapa da mansão com pistas em alguns cômodos
// ==========================================================
int main() {

    // ------------------------------------------------------
    // Mapa da mansão (ÁRVORE BINÁRIA FIXA)
    // ------------------------------------------------------
    Sala *hall = criarSala("Hall de Entrada", NULL);

    hall->esq = criarSala("Sala de Estar", "Pegadas suspeitas");
    hall->dir = criarSala("Cozinha", "Faca fora do lugar");

    hall->esq->esq = criarSala("Jardim", "Terra revirada");
    hall->esq->dir = criarSala("Biblioteca", "Livro arrancado da estante");

    hall->dir->esq = criarSala("Despensa", NULL);
    hall->dir->dir = criarSala("Porão", "Porta trancada por dentro");


    // BST das pistas coletadas
    PistaNode *pistasColetadas = NULL;


    printf("\n=== DETECTIVE QUEST — NIVEL AVENTUREIRO ===\n");

    // Inicia exploração
    explorarSalasComPistas(hall, &pistasColetadas);

    // Exibe pistas ao final
    printf("\n=== PISTAS COLETADAS ===\n");
    if (pistasColetadas == NULL)
        printf("Nenhuma pista foi coletada.\n");
    else
        exibirPistas(pistasColetadas);

    return 0;
}
