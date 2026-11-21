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

// --- Agenda (atividades 42, 43 e 44) ---
Agenda* criarAgenda(void);                      // NULL em caso de falha
int inserirInicio(Agenda* agenda, Missao m);    // 1 = sucesso, 0 = falha
int inserirFim(Agenda* agenda, Missao m);       // 1 = sucesso, 0 = falha
int buscarMissaoPorId(const Agenda* agenda, int id, Missao* resultado); // 1 = achou, 0 = ausente
int inserirDepoisDe(Agenda* agenda, int idReferencia, Missao novaMissao); // 1 = sucesso, 0 = falha
void exibirAgenda(const Agenda* agenda);
void destruirAgenda(Agenda* agenda);

#endif // MISSAO_H
