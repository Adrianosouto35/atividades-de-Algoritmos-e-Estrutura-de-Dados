#ifndef MISSAO_H
#define MISSAO_H

#define TAM_DESCRICAO 100

// Dados publicos de uma Missao
typedef struct Missao {
    int id;
    char descricao[TAM_DESCRICAO];
    int prioridade; // 1 = mais urgente
} Missao;

// Tipo opaco: o programa principal nao conhece a estrutura interna da lista
typedef struct Agenda Agenda;

// --- Missao (atividade 41) ---
Missao criarMissao(int id, const char* descricao, int prioridade);
void exibirMissao(const Missao* m);

// --- Agenda (atividades 42 a 48) ---
Agenda* criarAgenda(void);                      // NULL em caso de falha
int inserirInicio(Agenda* agenda, Missao m);    // 1 = sucesso, 0 = falha
int inserirFim(Agenda* agenda, Missao m);       // 1 = sucesso, 0 = falha
int buscarMissaoPorId(const Agenda* agenda, int id, Missao* resultado); // 1 = achou, 0 = ausente
int inserirDepoisDe(Agenda* agenda, int idReferencia, Missao novaMissao); // 1 = sucesso, 0 = falha
int inserirAntesDe(Agenda* agenda, int idReferencia, Missao novaMissao);  // 1 = sucesso, 0 = falha
int removerInicio(Agenda* agenda, Missao* removida); // 1 = removeu, 0 = lista vazia/nula
int removerFim(Agenda* agenda, Missao* removida);    // 1 = removeu, 0 = lista vazia/nula
int removerPorId(Agenda* agenda, int id, Missao* removida); // 1 = removeu, 0 = nenhum registro com o id
int contarMissoes(const Agenda* agenda);             // quantidade de nos (-1 se agenda nula)

// Remocoes posicionais. Retornos:
//    1 = removeu
//    0 = agenda nula ou referencia ausente
//   -1 = referencia existe, mas nao ha no para remover naquela posicao
int removerDepoisDe(Agenda* agenda, int idReferencia, Missao* removida);
int removerAntesDe(Agenda* agenda, int idReferencia, Missao* removida);
void exibirAgenda(const Agenda* agenda);

// Libera todos os nos e deixa a agenda vazia (mas ainda utilizavel).
// Pode ser chamada em lista ja vazia ou nula sem efeito colateral.
void limparAgenda(Agenda* agenda);

// Altera descricao e prioridade da missao com o id informado.
// O id e a chave de busca e nunca e alterado (novosDados.id e ignorado).
// Retornos: 1 = alterou, 0 = agenda nula ou id inexistente, -1 = dados invalidos
int atualizarMissao(Agenda* agenda, int id, Missao novosDados);

// Limpa a agenda e libera a propria estrutura (chamar ao encerrar o programa)
void destruirAgenda(Agenda* agenda);

#endif // MISSAO_H
