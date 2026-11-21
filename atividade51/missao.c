#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
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

int missaoValida(const Missao* m) {
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

int inserirLote(Agenda* agenda, const Missao* lote, int qtd) {
    if (agenda == NULL) {
        return 0;
    }

    // Valida o lote inteiro antes de alocar qualquer no
    if (lote == NULL || qtd <= 0 || qtd > MAX_LOTE) {
        return -1;
    }
    for (int i = 0; i < qtd; i++) {
        if (!missaoValida(&lote[i])) {
            return -1;
        }
    }

    // Monta primeiro uma cadeia separada com os novos nos, na ordem do vetor.
    // A agenda so e tocada se TODAS as alocacoes funcionarem.
    No* primeiro = NULL;
    No* ultimo = NULL;

    for (int i = 0; i < qtd; i++) {
        No* novo = criarNo(lote[i]);
        if (novo == NULL) {
            // Falha de alocacao: libera o que ja foi criado e desfaz a operacao
            while (primeiro != NULL) {
                No* temp = primeiro;
                primeiro = primeiro->prox;
                free(temp);
            }
            return -2;
        }

        if (primeiro == NULL) {
            primeiro = novo;
        } else {
            ultimo->prox = novo;
        }
        ultimo = novo;
    }

    // Conecta a cadeia pronta ao fim da agenda (percorre a lista uma unica vez)
    if (agenda->inicio == NULL) {
        agenda->inicio = primeiro;
    } else {
        No* aux = agenda->inicio;
        while (aux->prox != NULL) {
            aux = aux->prox;
        }
        aux->prox = primeiro;
    }

    return 1;
}

// Funcao auxiliar interna: criterio de ordenacao (1 se 'a' deve vir antes de 'b')
// Prioridade menor primeiro; em empate, menor id primeiro.
static int vemAntes(const No* a, const No* b) {
    if (a->dado.prioridade != b->dado.prioridade) {
        return a->dado.prioridade < b->dado.prioridade;
    }
    return a->dado.id < b->dado.id;
}

/*
 * DECISAO DE PROJETO: rearranjo de nos (e nao troca de dados).
 * - Trocar dados copiaria uma struct Missao inteira (~108 bytes) a cada troca;
 *   religar nos mexe apenas em ponteiros.
 * - Cada no continua guardando exatamente o mesmo registro, o que torna trivial
 *   provar que nenhum dado foi perdido ou duplicado.
 * Algoritmo: insertion sort sobre lista encadeada. Cada no e retirado da lista original
 * e encaixado na lista ordenada, logo depois dos nos que nao devem vir depois dele
 * (por isso e estavel). Custo O(n^2) no pior caso, suficiente para uma agenda de
 * tamanho moderado; para listas muito grandes, o merge sort seria mais indicado.
 */
int ordenarAgenda(Agenda* agenda) {
    if (agenda == NULL) {
        return 0;
    }

    No* ordenada = NULL;
    No* atual = agenda->inicio;

    while (atual != NULL) {
        No* proximo = atual->prox; // salva o proximo antes de religar o atual

        if (ordenada == NULL || vemAntes(atual, ordenada)) {
            // Entra no inicio da lista ordenada
            atual->prox = ordenada;
            ordenada = atual;
        } else {
            // Avanca ate o ultimo no que nao deve vir depois de 'atual'
            No* pos = ordenada;
            while (pos->prox != NULL && !vemAntes(atual, pos->prox)) {
                pos = pos->prox;
            }
            atual->prox = pos->prox;
            pos->prox = atual;
        }

        atual = proximo;
    }

    agenda->inicio = ordenada;
    return 1;
}

int salvarAgenda(const Agenda* agenda, const char* caminho) {
    if (agenda == NULL || caminho == NULL) {
        return 0;
    }

    FILE* arq = fopen(caminho, "w");
    if (arq == NULL) {
        return -1; // nao abriu: nao ha nada para fechar
    }

    // Percorre do inicio ao fim gravando somente os dados de cada missao
    const No* aux = agenda->inicio;
    while (aux != NULL) {
        // Copia local da descricao: quebras de linha viram espaco para nao
        // corromper o formato de uma missao por linha
        char descricao[TAM_DESCRICAO];
        strncpy(descricao, aux->dado.descricao, TAM_DESCRICAO - 1);
        descricao[TAM_DESCRICAO - 1] = '\0';
        for (char* p = descricao; *p != '\0'; p++) {
            if (*p == '\n' || *p == '\r') {
                *p = ' ';
            }
        }

        if (fprintf(arq, "%d;%d;%s\n", aux->dado.id, aux->dado.prioridade, descricao) < 0) {
            fclose(arq); // fecha tambem no caminho de erro
            return -2;
        }

        aux = aux->prox;
    }

    // fclose descarrega o buffer: erros de gravacao tambem podem aparecer aqui
    if (fclose(arq) != 0) {
        return -2;
    }

    return 1;
}

#define TAM_LINHA 256 // bem acima do maior registro valido (~123 caracteres)

// Funcao auxiliar interna: converte uma linha "id;prioridade;descricao" em Missao.
// Retorna 1 se a linha e valida e a missao e aceitavel; 0 caso contrario.
static int interpretarLinha(const char* linha, Missao* m) {
    char* fim;

    errno = 0;
    long id = strtol(linha, &fim, 10);
    if (fim == linha || *fim != ';' || errno == ERANGE || id < INT_MIN || id > INT_MAX) {
        return 0;
    }

    const char* inicioPrio = fim + 1;
    errno = 0;
    long prioridade = strtol(inicioPrio, &fim, 10);
    if (fim == inicioPrio || *fim != ';' || errno == ERANGE ||
        prioridade < INT_MIN || prioridade > INT_MAX) {
        return 0;
    }

    const char* descricao = fim + 1; // o resto da linha (pode conter ';')
    if (strlen(descricao) >= TAM_DESCRICAO) {
        return 0; // nao truncar silenciosamente um registro corrompido
    }

    *m = criarMissao((int)id, descricao, (int)prioridade);
    return missaoValida(m);
}

int carregarAgenda(Agenda* agenda, const char* caminho) {
    if (agenda == NULL || caminho == NULL) {
        return CARGA_PARAMETRO_INVALIDO;
    }

    // Politica antiduplicacao: recusa se ja houver missoes
    if (agenda->inicio != NULL) {
        return CARGA_RECUSADA_AGENDA_NAO_VAZIA;
    }

    FILE* arq = fopen(caminho, "r");
    if (arq == NULL) {
        // Arquivo inexistente nao e erro: a agenda continua vazia
        return (errno == ENOENT) ? CARGA_ARQUIVO_INEXISTENTE : CARGA_ERRO_LEITURA;
    }

    // Monta uma cadeia separada; a agenda so e alterada se o arquivo inteiro for valido
    No* primeiro = NULL;
    No* ultimo = NULL;
    int resultado = CARGA_OK;
    char linha[TAM_LINHA];

    while (fgets(linha, sizeof(linha), arq) != NULL) {
        size_t tam = strlen(linha);

        if (tam > 0 && linha[tam - 1] == '\n') {
            linha[--tam] = '\0';
            if (tam > 0 && linha[tam - 1] == '\r') {
                linha[--tam] = '\0'; // aceita fim de linha CRLF
            }
        } else if (!feof(arq)) {
            resultado = CARGA_ARQUIVO_INVALIDO; // linha maior que o buffer
            break;
        }

        if (tam == 0) {
            continue; // ignora linhas em branco
        }

        Missao m;
        if (!interpretarLinha(linha, &m)) {
            resultado = CARGA_ARQUIVO_INVALIDO; // valida ANTES de inserir
            break;
        }

        No* novo = criarNo(m);
        if (novo == NULL) {
            resultado = CARGA_SEM_MEMORIA;
            break;
        }

        // Insere no fim da cadeia: preserva a ordem gravada
        if (primeiro == NULL) {
            primeiro = novo;
        } else {
            ultimo->prox = novo;
        }
        ultimo = novo;
    }

    if (resultado == CARGA_OK && ferror(arq)) {
        resultado = CARGA_ERRO_LEITURA;
    }
    fclose(arq);

    if (resultado != CARGA_OK) {
        // Desfaz: libera a cadeia parcial; a agenda continua vazia
        while (primeiro != NULL) {
            No* temp = primeiro;
            primeiro = primeiro->prox;
            free(temp);
        }
        return resultado;
    }

    agenda->inicio = primeiro; // agenda estava vazia
    return CARGA_OK;
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
