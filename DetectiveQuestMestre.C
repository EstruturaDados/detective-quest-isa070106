#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAM_NOME 64
#define TAM_PISTA 128
#define HASH_SIZE 31   // tamanho da tabela hash (primo pequeno)

/* ==========================
   ESTRUTURAS
   ========================== */

// Nó da árvore binária que representa cada cômodo da mansão
typedef struct sala {
    char nome[TAM_NOME];
    char pista[TAM_PISTA];    // pista associada à sala (string vazia se não houver)
    struct sala *esq;
    struct sala *dir;
} Sala;

// Nó da BST que armazena pistas coletadas (ordem alfabética)
typedef struct pistaNode {
    char conteudo[TAM_PISTA];
    struct pistaNode *esq;
    struct pistaNode *dir;
} PistaNode;

// Nó para lista encadeada na tabela hash (colisões via encadeamento)
typedef struct HashNode {
    char pista[TAM_PISTA];
    char suspeito[TAM_NOME];
    struct HashNode *next;
} HashNode;

/* ==========================
   ASSINATURAS DE FUNÇÕES
   ========================== */

Sala* criarSala(const char *nome, const char *pista);
/* Cria dinamicamente um cômodo com nome e pista (pista pode ser NULL). */

void explorarSalas(Sala *inicio, PistaNode **raizPistas, HashNode *tabela[]);
 /* Navega interativamente pela árvore da mansão, coleta pistas automaticamente
    e insere cada pista encontrada na BST de pistas. */

PistaNode* inserirPista(PistaNode *raiz, const char *conteudo);
/* Insere uma pista na BST em ordem alfabética. */

void exibirPistas(PistaNode *raiz);
/* Exibe todas as pistas coletadas em ordem (in-order traversal). */

unsigned int hash_func(const char *str);
/* Função de dispersão simples para a tabela hash. */

void inserirNaHash(HashNode *tabela[], const char *pista, const char *suspeito);
/* Insere a associação pista -> suspeito na tabela hash (encadeamento). */

const char* encontrarSuspeito(HashNode *tabela[], const char *pista);
/* Retorna o nome do suspeito associado a uma pista ou NULL se não achar. */

int contarPistasParaSuspeito(PistaNode *raiz, HashNode *tabela[], const char *suspeito);
/* Percorre a BST de pistas e conta quantas pistas apontam para 'suspeito'. */

void verificarSuspeitoFinal(PistaNode *raiz, HashNode *tabela[], const char *acusado);
/* Verifica se há pelo menos 2 pistas que apontam para o suspeito acusado e imprime resultado. */

/* ==========================
   IMPLEMENTAÇÃO
   ========================== */

/* criarSala()
   Cria dinamicamente um nó Sala com nome e pista opcional.
*/
Sala* criarSala(const char *nome, const char *pista) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (!s) {
        printf("Erro ao alocar memória para sala.\n");
        exit(1);
    }
    strncpy(s->nome, nome, TAM_NOME-1); s->nome[TAM_NOME-1] = '\0';
    if (pista != NULL) {
        strncpy(s->pista, pista, TAM_PISTA-1); s->pista[TAM_PISTA-1] = '\0';
    } else {
        s->pista[0] = '\0';
    }
    s->esq = s->dir = NULL;
    return s;
}

/* inserirPista()
   Insere a pista na BST ordenada alfabeticamente.
   Duplicatas são permitidas (cada entrada representa uma coleta).
*/
PistaNode* inserirPista(PistaNode *raiz, const char *conteudo) {
    if (raiz == NULL) {
        PistaNode *no = (PistaNode*) malloc(sizeof(PistaNode));
        if (!no) { printf("Erro de memória ao inserir pista.\n"); exit(1); }
        strncpy(no->conteudo, conteudo, TAM_PISTA-1); no->conteudo[TAM_PISTA-1] = '\0';
        no->esq = no->dir = NULL;
        return no;
    }
    if (strcmp(conteudo, raiz->conteudo) < 0)
        raiz->esq = inserirPista(raiz->esq, conteudo);
    else
        raiz->dir = inserirPista(raiz->dir, conteudo);

    return raiz;
}

/* exibirPistas()
   Imprime as pistas em ordem alfabética (in-order).
*/
void exibirPistas(PistaNode *raiz) {
    if (!raiz) return;
    exibirPistas(raiz->esq);
    printf("- %s\n", raiz->conteudo);
    exibirPistas(raiz->dir);
}

/* hash_func()
   Função de dispersão simples: soma ASCII dos caracteres % HASH_SIZE.
*/
unsigned int hash_func(const char *str) {
    unsigned int sum = 0;
    for (; *str; ++str) sum = (sum * 31u) + (unsigned char)(*str);
    return sum % HASH_SIZE;
}

/* inserirNaHash()
   Insere (pista -> suspeito) na tabela hash; permite múltiplas pistas.
*/
void inserirNaHash(HashNode *tabela[], const char *pista, const char *suspeito) {
    unsigned int idx = hash_func(pista);
    HashNode *novo = (HashNode*) malloc(sizeof(HashNode));
    if (!novo) { printf("Erro de memória na hash.\n"); exit(1); }
    strncpy(novo->pista, pista, TAM_PISTA-1); novo->pista[TAM_PISTA-1] = '\0';
    strncpy(novo->suspeito, suspeito, TAM_NOME-1); novo->suspeito[TAM_NOME-1] = '\0';
    novo->next = tabela[idx];
    tabela[idx] = novo;
}

/* encontrarSuspeito()
   Procura na tabela hash a pista e retorna o suspeito associado (primeira ocorrência).
   Retorna NULL se não encontrar.
*/
const char* encontrarSuspeito(HashNode *tabela[], const char *pista) {
    unsigned int idx = hash_func(pista);
    for (HashNode *h = tabela[idx]; h != NULL; h = h->next) {
        if (strcmp(h->pista, pista) == 0)
            return h->suspeito;
    }
    return NULL;
}

/* contarPistasParaSuspeito()
   Percorre a BST de pistas e conta quantas pistas coletadas apontam para 'suspeito'.
*/
int contarPistasParaSuspeito(PistaNode *raiz, HashNode *tabela[], const char *suspeito) {
    if (!raiz) return 0;
    int count = 0;
    const char *s = encontrarSuspeito(tabela, raiz->conteudo);
    if (s != NULL && strcmp(s, suspeito) == 0) count++;
    count += contarPistasParaSuspeito(raiz->esq, tabela, suspeito);
    count += contarPistasParaSuspeito(raiz->dir, tabela, suspeito);
    return count;
}

/* verificarSuspeitoFinal()
   Verifica se há pelo menos 2 pistas apontando para o acusado e imprime o veredito.
*/
void verificarSuspeitoFinal(PistaNode *raiz, HashNode *tabela[], const char *acusado) {
    int total = contarPistasParaSuspeito(raiz, tabela, acusado);
    printf("\n--- JULGAMENTO FINAL ---\n");
    printf("Você acusou: %s\n", acusado);
    printf("Pistas que apontam para %s: %d\n", acusado, total);
    if (total >= 2) {
        printf("Resultado: Há evidências suficientes. Acusação CONVALIDADA.\n");
    } else {
        printf("Resultado: Evidências insuficientes. Acusação REJEITADA.\n");
    }
}

/* explorarSalas()
   Navega interativamente pela árvore de salas; ao entrar em uma sala,
   exibe a pista (se houver) e a adiciona na BST de pistas coletadas.
*/
void explorarSalas(Sala *inicio, PistaNode **raizPistas, HashNode *tabela[]) {
    Sala *atual = inicio;
    char opcao;
    while (atual != NULL) {
        printf("\nVocê está em: %s\n", atual->nome);
        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada: \"%s\"\n", atual->pista);
            *raizPistas = inserirPista(*raizPistas, atual->pista);
        }

        // opções
        printf("\nEscolha um caminho:\n");
        if (atual->esq) printf("  (e) Ir para a esquerda -> %s\n", atual->esq->nome);
        if (atual->dir) printf("  (d) Ir para a direita  -> %s\n", atual->dir->nome);
        printf("  (s) Sair da exploração\n");
        printf("Digite sua opção: ");
        if (scanf(" %c", &opcao) != 1) { opcao = 's'; }
        // limpar resto da linha
        int c; while ((c = getchar()) != '\n' && c != EOF);

        if (opcao == 'e') {
            if (atual->esq) atual = atual->esq;
            else printf("Não há caminho à esquerda!\n");
        } else if (opcao == 'd') {
            if (atual->dir) atual = atual->dir;
            else printf("Não há caminho à direita!\n");
        } else if (opcao == 's') {
            printf("Exploração encerrada pelo jogador.\n");
            return;
        } else {
            printf("Opção inválida. Use e, d ou s.\n");
        }
    }
}

/* ==========================
   FUNÇÃO MAIN: monta tudo e dirige o jogo
   ========================== */
int main() {
    // 1) Monta o mapa da mansão (árvore binária fixa)
    Sala *hall = criarSala("Hall de Entrada", NULL);

    hall->esq = criarSala("Sala de Estar", "Pegadas no tapete");
    hall->dir = criarSala("Cozinha", "Faca com sangue");

    hall->esq->esq = criarSala("Jardim", "Pegadas que levam ao portão");
    hall->esq->dir = criarSala("Biblioteca", "Livro rasgado");

    hall->dir->esq = criarSala("Despensa", "Pano manchado");
    hall->dir->dir = criarSala("Porão", "Porta trancada por dentro");

    // 2) Prepara a tabela hash (vazia)
    HashNode *tabela[HASH_SIZE];
    for (int i = 0; i < HASH_SIZE; ++i) tabela[i] = NULL;

    // 3) Insere associações pista -> suspeito (definidas pelo jogo)
    inserirNaHash(tabela, "Pegadas no tapete", "Sr. Oliveira");
    inserirNaHash(tabela, "Pegadas que levam ao portão", "Sr. Oliveira");
    inserirNaHash(tabela, "Faca com sangue", "Sra. Martins");
    inserirNaHash(tabela, "Pano manchado", "Sra. Martins");
    inserirNaHash(tabela, "Livro rasgado", "Sra. Silva");
    inserirNaHash(tabela, "Porta trancada por dentro", "Desconhecido"); // pista ambígua

    // 4) BST vazia para coletar pistas encontradas
    PistaNode *pistasColetadas = NULL;

    printf("\n=== DETECTIVE QUEST — NIVEL MESTRE ===\n");

    // 5) Exploração (interativa)
    explorarSalas(hall, &pistasColetadas, tabela);

    // 6) Ao final, mostra todas as pistas coletadas em ordem
    printf("\n=== PISTAS COLETADAS (ordem alfabética) ===\n");
    if (pistasColetadas == NULL) {
        printf("Nenhuma pista coletada.\n");
    } else {
        exibirPistas(pistasColetadas);
    }

    // 7) Pergunta ao jogador quem ele acusa
    char acusado[TAM_NOME];
    printf("\nQuem você acusa? Digite o nome do suspeito: ");
    if (!fgets(acusado, TAM_NOME, stdin)) acusado[0]='\0';
    // remover newline e espaços no final
    size_t L = strlen(acusado);
    if (L > 0 && acusado[L-1] == '\n') acusado[L-1] = '\0';

    // 8) Verifica se há ao menos 2 pistas que apontam para o acusado
    verificarSuspeitoFinal(pistasColetadas, tabela, acusado);

    printf("\nObrigado por jogar — investigação encerrada.\n");
    return 0;
}
