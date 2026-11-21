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

int inserirAntesDe(Agenda* agenda, int idReferencia, Missao novaMissao) {
    if (agenda == NULL || agenda->inicio == NULL) {
        return 0; // lista inexistente ou vazia: nada a modificar
    }

    // Mantem ponteiros anterior e atual durante a busca
    No* anterior = NULL;
    No* atual = agenda->inicio;

    while (atual != NULL && atual->dado.id != idReferencia) {
        anterior = atual;
        atual = atual->prox;
    }

    // Referencia nao encontrada: lista permanece inalterada (nada foi alocado)
    if (atual == NULL) {
        return 0;
    }

    No* novo = criarNo(novaMissao);
    if (novo == NULL) {
        return 0;
    }

    novo->prox = atual;

    if (anterior == NULL) {
        // Caso especial: insercao antes do primeiro no altera o inicio
        agenda->inicio = novo;
    } else {
        // Caso geral: insercao entre anterior e atual
        anterior->prox = novo;
    }

    return 1;
}

int removerInicio(Agenda* agenda, Missao* removida) {
    if (agenda == NULL || agenda->inicio == NULL) {
        return 0; // impede remocao em lista vazia
    }

    No* alvo = agenda->inicio;

    // Devolve os dados removidos antes de liberar a memoria
    if (removida != NULL) {
        *removida = alvo->dado;
    }

    // Atualiza o inicio ANTES de chamar free
    agenda->inicio = alvo->prox;
    free(alvo);

    return 1;
}

int removerFim(Agenda* agenda, Missao* removida) {
    if (agenda == NULL || agenda->inicio == NULL) {
        return 0; // lista inexistente ou vazia
    }

    // Localiza o ultimo no e seu antecessor
    No* anterior = NULL;
    No* atual = agenda->inicio;
    while (atual->prox != NULL) {
        anterior = atual;
        atual = atual->prox;
    }

    if (removida != NULL) {
        *removida = atual->dado; // devolve os dados antes de liberar
    }

    if (anterior == NULL) {
        // Unico elemento: a lista passa a ser vazia
        agenda->inicio = NULL;
    } else {
        // O antecessor passa a ser o novo ultimo no
        anterior->prox = NULL;
    }

    free(atual); // libera exatamente o no retirado
    return 1;
}

int removerPorId(Agenda* agenda, int id, Missao* removida) {
    if (agenda == NULL || agenda->inicio == NULL) {
        return 0; // lista inexistente ou vazia: nenhum registro
    }

    // Busca a primeira correspondencia mantendo o anterior
    No* anterior = NULL;
    No* atual = agenda->inicio;
    while (atual != NULL && atual->dado.id != id) {
        anterior = atual;
        atual = atual->prox;
    }

    if (atual == NULL) {
        return 0; // nenhum registro corresponde ao criterio; lista inalterada
    }

    if (removida != NULL) {
        *removida = atual->dado;
    }

    if (anterior == NULL) {
        // Remocao no inicio: o inicio passa para o segundo no
        agenda->inicio = atual->prox;
    } else {
        // Religa os vizinhos, pulando o no removido
        anterior->prox = atual->prox;
    }

    free(atual); // libera exatamente o no retirado
    return 1;
}

int contarMissoes(const Agenda* agenda) {
    if (agenda == NULL) {
        return -1;
    }

    int total = 0;
    const No* aux = agenda->inicio;
    while (aux != NULL) {
        total++;
        aux = aux->prox;
    }
    return total;
}

int removerDepoisDe(Agenda* agenda, int idReferencia, Missao* removida) {
    if (agenda == NULL) {
        return 0;
    }

    No* referencia = buscarNoPorId(agenda, idReferencia);
    if (referencia == NULL) {
        return 0; // referencia ausente (inclui lista vazia)
    }

    No* alvo = referencia->prox;
    if (alvo == NULL) {
        return -1; // referencia e o ultimo no: nada depois dela
    }

    if (removida != NULL) {
        *removida = alvo->dado; // devolve os dados antes de liberar
    }

    referencia->prox = alvo->prox; // preserva o restante do encadeamento
    free(alvo);

    return 1;
}

int removerAntesDe(Agenda* agenda, int idReferencia, Missao* removida) {
    if (agenda == NULL || agenda->inicio == NULL) {
        return 0; // lista inexistente ou vazia: referencia ausente
    }

    // Acompanha: atual (referencia), anterior (alvo) e antesAnterior
    No* antesAnterior = NULL;
    No* anterior = NULL;
    No* atual = agenda->inicio;

    while (atual != NULL && atual->dado.id != idReferencia) {
        antesAnterior = anterior;
        anterior = atual;
        atual = atual->prox;
    }

    if (atual == NULL) {
        return 0; // referencia ausente
    }

    if (anterior == NULL) {
        return -1; // referencia e o primeiro no: nada antes dela
    }

    if (removida != NULL) {
        *removida = anterior->dado; // devolve os dados antes de liberar
    }

    if (antesAnterior == NULL) {
        // O removido e o primeiro no: o inicio passa a ser a referencia
        agenda->inicio = atual;
    } else {
        // Religa o no antes do removido diretamente a referencia
        antesAnterior->prox = atual;
    }

    free(anterior);

    return 1;
}

// Funcao auxiliar interna: valida os dados de uma missao
static int missaoValida(const Missao* m) {
    if (m == NULL) {
        return 0;
    }
    if (m->descricao[0] == '\0') {
        return 0; // descricao vazia
    }
    if (m->prioridade < 1) {
        return 0; // prioridade deve ser 1 (mais urgente) ou maior
    }
    return 1;
}

void limparAgenda(Agenda* agenda) {
    if (agenda == NULL) {
        return;
    }

    No* atual = agenda->inicio;
    while (atual != NULL) {
        No* proximo = atual->prox; // salva o proximo ANTES de liberar o atual
        free(atual);
        atual = proximo;
    }

    agenda->inicio = NULL; // lista vazia e consistente apos a limpeza
}

int atualizarMissao(Agenda* agenda, int id, Missao novosDados) {
    if (agenda == NULL) {
        return 0;
    }

    // Valida antes de qualquer alteracao
    if (!missaoValida(&novosDados)) {
        return -1;
    }

    No* alvo = buscarNoPorId(agenda, id);
    if (alvo == NULL) {
        return 0; // id inexistente: nada alterado
    }

    // Atualiza apenas os campos permitidos; o id (chave) e preservado
    strncpy(alvo->dado.descricao, novosDados.descricao, TAM_DESCRICAO - 1);
    alvo->dado.descricao[TAM_DESCRICAO - 1] = '\0';
    alvo->dado.prioridade = novosDados.prioridade;

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

    limparAgenda(agenda); // descarta todas as missoes restantes
    free(agenda);
}
