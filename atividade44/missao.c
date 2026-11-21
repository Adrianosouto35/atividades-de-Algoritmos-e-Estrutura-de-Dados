#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "missao.h"

// Estrutura interna do No (encapsulada no .c)
typedef struct No {
    Missao dado;
    struct No* prox;
} No;

// Estrutura interna da Agenda (encapsulada no .c)
struct Agenda {
    No* inicio;
};

// ---------------- Missao ----------------

Missao criarMissao(int id, const char* descricao, int prioridade) {
    Missao m;

    m.id = id;
    m.prioridade = prioridade;

    if (descricao != NULL) {
        strncpy(m.descricao, descricao, TAM_DESCRICAO - 1);
        m.descricao[TAM_DESCRICAO - 1] = '\0';
    } else {
        m.descricao[0] = '\0';
    }

    return m;
}

void exibirMissao(const Missao* m) {
    if (m == NULL) {
        printf("[Aviso] Missao inexistente.\n");
        return;
    }

    printf("Missao #%d | Prioridade: %d | Descricao: %s\n",
           m->id, m->prioridade, m->descricao);
}

// ---------------- Agenda ----------------

// Funcao auxiliar interna: aloca, valida e preenche um no
static No* criarNo(Missao m) {
    No* novo = (No*)malloc(sizeof(No));
    if (novo == NULL) {
        printf("Erro critico: falha ao alocar memoria para o novo no.\n");
        return NULL;
    }

    novo->dado = m; // copia todos os dados da missao para dentro do no
    novo->prox = NULL;

    return novo;
}

// Funcao auxiliar interna: devolve o primeiro no com o id informado (ou NULL)
static No* buscarNoPorId(const Agenda* agenda, int id) {
    if (agenda == NULL) {
        return NULL;
    }

    No* aux = agenda->inicio;
    while (aux != NULL) {
        if (aux->dado.id == id) {
            return aux; // primeira correspondencia
        }
        aux = aux->prox;
    }

    return NULL; // ausente
}

Agenda* criarAgenda(void) {
    Agenda* agenda = (Agenda*)malloc(sizeof(Agenda));
    if (agenda == NULL) {
        printf("Erro critico: falha ao alocar memoria para a agenda.\n");
        return NULL;
    }

    agenda->inicio = NULL; // lista vazia
    return agenda;
}

int inserirInicio(Agenda* agenda, Missao m) {
    if (agenda == NULL) {
        return 0;
    }

    No* novo = criarNo(m);
    if (novo == NULL) {
        return 0; // falha de alocacao: lista permanece inalterada
    }

    novo->prox = agenda->inicio; // conecta antes do antigo primeiro no
    agenda->inicio = novo;       // atualiza o inicio

    return 1;
}

int inserirFim(Agenda* agenda, Missao m) {
    if (agenda == NULL) {
        return 0;
    }

    No* novo = criarNo(m); // novo->prox ja e NULL
    if (novo == NULL) {
        return 0; // falha de alocacao: lista permanece inalterada
    }

    // Caso 1: lista vazia, o novo no passa a ser o primeiro
    if (agenda->inicio == NULL) {
        agenda->inicio = novo;
        return 1;
    }

    // Caso 2: lista com nos, localiza o ultimo (aquele cujo prox e NULL)
    No* aux = agenda->inicio;
    while (aux->prox != NULL) {
        aux = aux->prox;
    }
    aux->prox = novo; // o ultimo no passa a apontar para o novo

    return 1;
}

int buscarMissaoPorId(const Agenda* agenda, int id, Missao* resultado) {
    No* encontrado = buscarNoPorId(agenda, id);
    if (encontrado == NULL) {
        return 0; // ausente
    }

    if (resultado != NULL) {
        *resultado = encontrado->dado; // devolve uma copia, sem expor o no
    }
    return 1;
}

int inserirDepoisDe(Agenda* agenda, int idReferencia, Missao novaMissao) {
    if (agenda == NULL) {
        return 0;
    }

    // Reutiliza a busca: so segue se a referencia existir
    No* referencia = buscarNoPorId(agenda, idReferencia);
    if (referencia == NULL) {
        return 0; // lista permanece inalterada
    }

    // Fluxo seguro: o no so e alocado depois de confirmar a referencia
    No* novo = criarNo(novaMissao);
    if (novo == NULL) {
        return 0;
    }

    novo->prox = referencia->prox; // liga ao antigo sucessor (NULL se for o ultimo)
    referencia->prox = novo;       // referencia passa a apontar para o novo no

    return 1;
}

void exibirAgenda(const Agenda* agenda) {
    if (agenda == NULL) {
        printf("[Erro] Agenda nao inicializada.\n");
        return;
    }

    if (agenda->inicio == NULL) {
        printf("\n[Agenda Vazia] Nenhuma missao cadastrada no momento.\n");
        return;
    }

    printf("\n=== ORDEM ATUAL DAS MISSOES ===\n");
    const No* aux = agenda->inicio; // ponteiro auxiliar: 'inicio' nao e alterado
    while (aux != NULL) {
        exibirMissao(&aux->dado);
        aux = aux->prox;
    }
    printf("===============================\n");
}

void destruirAgenda(Agenda* agenda) {
    if (agenda == NULL) {
        return;
    }

    No* atual = agenda->inicio;
    while (atual != NULL) {
        No* temp = atual;
        atual = atual->prox;
        free(temp);
    }
    free(agenda);
}
