#include <stdio.h>
#include <string.h>
#include "missao.h"

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
