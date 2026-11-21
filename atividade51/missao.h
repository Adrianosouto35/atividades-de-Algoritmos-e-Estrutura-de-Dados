#ifndef MISSAO_H
#define MISSAO_H

/* ========================================================================
 *  Modulo MISSAO / AGENDA
 *  Agenda de missoes implementada como lista simplesmente encadeada.
 *  O tipo Agenda e opaco: a estrutura dos nos fica escondida em missao.c e
 *  o programa principal so conhece as funcoes declaradas aqui.
 * ======================================================================== */

#define TAM_DESCRICAO 100
#define MAX_LOTE 1000 // maximo de missoes aceitas em uma importacao

/* Dados publicos de uma missao. Prioridade 1 = mais urgente. */
typedef struct Missao {
    int id;                       /* chave de busca da missao */
    char descricao[TAM_DESCRICAO];
    int prioridade;
} Missao;

/* Tipo opaco: a estrutura interna da lista nao e visivel fora do modulo. */
typedef struct Agenda Agenda;

/* Resultados possiveis de carregarAgenda. */
typedef enum ResultadoCarga {
    CARGA_OK = 1,                     /* missoes carregadas (arquivo vazio tambem e OK) */
    CARGA_ARQUIVO_INEXISTENTE = 2,    /* arquivo nao existe: agenda segue vazia, sem erro */
    CARGA_PARAMETRO_INVALIDO = 0,     /* agenda ou caminho nulo */
    CARGA_RECUSADA_AGENDA_NAO_VAZIA = -1, /* agenda ja tem missoes: nada e carregado */
    CARGA_ARQUIVO_INVALIDO = -2,      /* alguma linha invalida: nada e carregado */
    CARGA_SEM_MEMORIA = -3,           /* falha de alocacao: nada e carregado */
    CARGA_ERRO_LEITURA = -4           /* falha ao abrir (exceto inexistente) ou ler */
} ResultadoCarga;

/* ------------------------------------------------------------------------
 *  Missao
 * ------------------------------------------------------------------------ */

/* Cria uma Missao preenchida. A descricao e copiada com seguranca (truncada em
 * TAM_DESCRICAO - 1 caracteres); descricao NULL gera descricao vazia. */
Missao criarMissao(int id, const char* descricao, int prioridade);

/* Informa se os dados sao aceitaveis: descricao nao vazia e prioridade >= 1.
 * Retorna 1 se valida, 0 se invalida ou se m for NULL. */
int missaoValida(const Missao* m);

/* Imprime uma missao na saida padrao. Aceita NULL (imprime um aviso). */
void exibirMissao(const Missao* m);

/* ------------------------------------------------------------------------
 *  Ciclo de vida da agenda
 * ------------------------------------------------------------------------ */

/* Cria uma agenda vazia. Retorna NULL se faltar memoria. */
Agenda* criarAgenda(void);

/* Libera todos os nos e deixa a agenda vazia, mas ainda utilizavel.
 * Pode ser chamada em agenda ja vazia ou nula sem efeito colateral. */
void limparAgenda(Agenda* agenda);

/* Limpa a agenda e libera a propria estrutura. Chamar ao encerrar o programa.
 * Aceita NULL. Depois da chamada o ponteiro nao deve mais ser usado. */
void destruirAgenda(Agenda* agenda);

/* ------------------------------------------------------------------------
 *  Insercao
 *  Retorno comum: 1 = inserido; 0 = falha (agenda nula, referencia inexistente
 *  ou falta de memoria). Em caso de falha a agenda permanece inalterada.
 * ------------------------------------------------------------------------ */

/* Insere a missao antes do primeiro no. */
int inserirInicio(Agenda* agenda, Missao m);

/* Insere a missao depois do ultimo no. */
int inserirFim(Agenda* agenda, Missao m);

/* Insere imediatamente antes da primeira missao cujo id seja idReferencia. */
int inserirAntesDe(Agenda* agenda, int idReferencia, Missao novaMissao);

/* Insere imediatamente depois da primeira missao cujo id seja idReferencia. */
int inserirDepoisDe(Agenda* agenda, int idReferencia, Missao novaMissao);

/* Importa um vetor de 'qtd' missoes no FIM da agenda, preservando a ordem do vetor
 * (lote[0] fica logo depois da ultima missao ja cadastrada). Operacao atomica:
 * ou todas entram, ou nenhuma.
 * Retornos: 1 = importado; 0 = agenda nula;
 *          -1 = lote invalido (NULL, qtd fora de 1..MAX_LOTE ou missao invalida);
 *          -2 = falha de alocacao (agenda permanece como estava). */
int inserirLote(Agenda* agenda, const Missao* lote, int qtd);

/* ------------------------------------------------------------------------
 *  Remocao
 *  Quando 'removida' nao e NULL, recebe uma copia dos dados da missao removida.
 * ------------------------------------------------------------------------ */

/* Remove o primeiro no. Retorna 1 = removeu; 0 = agenda nula ou vazia. */
int removerInicio(Agenda* agenda, Missao* removida);

/* Remove o ultimo no. Retorna 1 = removeu; 0 = agenda nula ou vazia. */
int removerFim(Agenda* agenda, Missao* removida);

/* Remove a primeira missao com o id informado.
 * Retorna 1 = removeu; 0 = agenda nula ou nenhum registro com esse id. */
int removerPorId(Agenda* agenda, int id, Missao* removida);

/* Remove a missao imediatamente depois da referencia (primeira com idReferencia).
 * Retorna 1 = removeu; 0 = agenda nula ou referencia ausente;
 *        -1 = a referencia e o ultimo no (nada depois dela). */
int removerDepoisDe(Agenda* agenda, int idReferencia, Missao* removida);

/* Remove a missao imediatamente antes da referencia (primeira com idReferencia).
 * Retorna 1 = removeu; 0 = agenda nula ou referencia ausente;
 *        -1 = a referencia e o primeiro no (nada antes dela). */
int removerAntesDe(Agenda* agenda, int idReferencia, Missao* removida);

/* ------------------------------------------------------------------------
 *  Consulta e alteracao
 * ------------------------------------------------------------------------ */

/* Busca a primeira missao com o id informado. Se resultado nao for NULL, recebe uma
 * copia dos dados. Retorna 1 = encontrou; 0 = ausente ou agenda nula. */
int buscarMissaoPorId(const Agenda* agenda, int id, Missao* resultado);

/* Quantidade de missoes pendentes: 0 para agenda vazia, -1 se a agenda for nula.
 * Apenas percorre a lista; nao modifica o encadeamento. */
int contarMissoes(const Agenda* agenda);

/* Altera descricao e prioridade da missao com o id informado. O id e a chave de
 * busca e nunca e alterado (novosDados.id e ignorado).
 * Retorna 1 = alterou; 0 = agenda nula ou id inexistente; -1 = dados invalidos
 * (a validacao ocorre antes de qualquer alteracao). */
int atualizarMissao(Agenda* agenda, int id, Missao novosDados);

/* Imprime todas as missoes na ordem atual; informa quando a agenda esta vazia. */
void exibirAgenda(const Agenda* agenda);

/* ------------------------------------------------------------------------
 *  Ordenacao e persistencia
 * ------------------------------------------------------------------------ */

/* Ordena por prioridade crescente; desempate pelo menor id; empate total mantem a
 * ordem anterior (estavel). Apenas religa nos: nenhum registro e criado, copiado ou
 * perdido. Retorna 1 = ok (inclui lista vazia ou unitaria); 0 = agenda nula. */
int ordenarAgenda(Agenda* agenda);

/* Grava as missoes, na ordem atual, em arquivo de texto (uma por linha):
 *     id;prioridade;descricao
 * Somente dados sao gravados, nunca enderecos. O arquivo e sobrescrito.
 * Retorna 1 = salvo; 0 = agenda ou caminho nulo; -1 = falha ao abrir;
 *        -2 = falha durante a escrita ou ao fechar. */
int salvarAgenda(const Agenda* agenda, const char* caminho);

/* Reconstroi a agenda a partir de um arquivo gravado por salvarAgenda, na mesma ordem.
 *
 * POLITICA ANTIDUPLICACAO: o carregamento e RECUSADO se a agenda ja tiver missoes
 * (CARGA_RECUSADA_AGENDA_NAO_VAZIA), em vez de limpa-la, para nao descartar dados sem
 * o usuario pedir. Para substituir o conteudo, chame limparAgenda antes. Assim,
 * carregamentos sucessivos nunca duplicam missoes.
 *
 * Cada linha e validada antes de ser inserida. A operacao e atomica: se qualquer linha
 * for invalida (ou faltar memoria), nada e inserido e a agenda continua vazia.
 * Linhas em branco sao ignoradas; fim de linha CRLF e aceito.
 * Se o arquivo nao existir, a agenda continua vazia (CARGA_ARQUIVO_INEXISTENTE).
 * Veja ResultadoCarga para todos os retornos. */
int carregarAgenda(Agenda* agenda, const char* caminho);

#endif // MISSAO_H
