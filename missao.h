#ifndef MISSAO_H
#define MISSAO_H

#define TAM_DESCRICAO 100

// Dados publicos de uma Missao
typedef struct Missao {
    int id;
    char descricao[TAM_DESCRICAO];
    int prioridade; // 1 = mais urgente
} Missao;

// Cria uma Missao ja preenchida (a descricao e copiada com seguranca)
Missao criarMissao(int id, const char* descricao, int prioridade);

// Exibe uma Missao na saida padrao
void exibirMissao(const Missao* m);

#endif // MISSAO_H
