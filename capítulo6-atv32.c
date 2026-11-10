#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_JOGADORES 5
#define MAX_NOME 50

// Estrutura para representar um Jogador
typedef struct {
    int id;
    char nome[MAX_NOME];
    int pontuacao;
} Jogador;

// Estrutura para estatísticas de desempenho
typedef struct {
    long comparacoes;
    long trocas;
} Estatisticas;

// ==========================================
// 1. VETOR NUMÉRICO
// ==========================================

// Encontra o índice do menor elemento a partir de 'inicio'
int encontrar_indice_menor_int(const int v[], int inicio, int n, long *comparacoes) {
    int idx_menor = inicio;
    for (int j = inicio + 1; j < n; j++) {
        (*comparacoes)++;
        if (v[j] < v[idx_menor]) {
            idx_menor = j;
        }
    }
    return idx_menor;
}

// Selection Sort para vetores numéricos
Estatisticas selection_sort_int(int v[], int n) {
    Estatisticas est = {0, 0};
    for (int i = 0; i < n - 1; i++) {
        int idx_menor = encontrar_indice_menor_int(v, i, n, &est.comparacoes);
        if (idx_menor != i) {
            int temp = v[i];
            v[i] = v[idx_menor];
            v[idx_menor] = temp;
            est.trocas++;
        }
    }
    return est;
}

// ==========================================
// 2. ESTRUTURA DE JOGADORES (COM DESEMPATE)
// ==========================================

// Função auxiliar para comparar dois jogadores:
// Retorna < 0 se j1 deve vir ANTES de j2 (j1 tem maior pontuação ou mesmo pontos com menor nome alfabético)
int comparar_jogadores(const Jogador *j1, const Jogador *j2, long *comparacoes) {
    (*comparacoes)++;
    
    // 1º Critério: Maior Pontuação (Ordem decrescente)
    if (j1->pontuacao != j2->pontuacao) {
        return j2->pontuacao - j1->pontuacao; 
    }
    
    // 2º Critério (Desempate): Nome (Ordem alfabética / crescente)
    return strcmp(j1->nome, j2->nome);
}

// Encontra o índice do melhor jogador a partir de 'inicio'
int encontrar_indice_melhor_jogador(const Jogador v[], int inicio, int n, long *comparacoes) {
    int idx_melhor = inicio;
    for (int j = inicio + 1; j < n; j++) {
        if (comparar_jogadores(&v[j], &v[idx_melhor], comparacoes) < 0) {
            idx_melhor = j;
        }
    }
    return idx_melhor;
}

// Selection Sort para a estrutura inteira de Jogadores
Estatisticas selection_sort_jogadores(Jogador v[], int n) {
    Estatisticas est = {0, 0};
    for (int i = 0; i < n - 1; i++) {
        int idx_melhor = encontrar_indice_melhor_jogador(v, i, n, &est.comparacoes);
        if (idx_melhor != i) {
            // Troca da estrutura inteira
            Jogador temp = v[i];
            v[i] = v[idx_melhor];
            v[idx_melhor] = temp;
            est.trocas++;
        }
    }
    return est;
}

// ==========================================
// FUNÇÕES AUXILIARES DE EXIBIÇÃO
// ==========================================

void imprimir_vetor_int(const char *titulo, const int v[], int n) {
    printf("\n--- %s ---\n", titulo);
    for (int i = 0; i < n; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");
}

void imprimir_jogadores(const char *titulo, const Jogador v[], int n) {
    printf("\n--- %s ---\n", titulo);
    printf("%-5s | %-20s | %-10s\n", "ID", "Nome", "Pontos");
    printf("-----------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%-5d | %-20s | %-10d\n", v[i].id, v[i].nome, v[i].pontuacao);
    }
}

// ==========================================
// PROGRAMA PRINCIPAL
// ==========================================

int main() {
    // 1. DADOS ORIGINAIS DE ENTRADA (Preservados)
    const int num_elementos = 6;
    int pontuacoes_originais[] = {45, 12, 89, 34, 12, 67};

    Jogador jogadores_originais[MAX_JOGADORES] = {
        {101, "Alice", 1500},
        {102, "Bruno", 2100},
        {103, "Carla", 1800},
        {104, "Daniel", 2100}, // Empate na pontuação com Bruno (Nome desempata: Bruno < Daniel)
        {105, "Eduardo", 1500} // Empate na pontuação com Alice (Nome desempata: Alice < Eduardo)
    };

    // 2. ORDENAÇÃO DO VETOR NUMÉRICO
    // Criação da cópia de trabalho para preservar o original
    int pontuacoes_trabalho[6];
    memcpy(pontuacoes_trabalho, pontuacoes_originais, sizeof(pontuacoes_originais));

    Estatisticas est_num = selection_sort_int(pontuacoes_trabalho, num_elementos);

    imprimir_vetor_int("Vetor Numérico Original", pontuacoes_originais, num_elementos);
    imprimir_vetor_int("Vetor Numérico Ordenado", pontuacoes_trabalho, num_elementos);
    printf("Estatísticas (Numérico) -> Comparações: %ld | Trocas: %ld\n", est_num.comparacoes, est_num.trocas);

    // 3. ORDENAÇÃO DA ESTRUTURA DE JOGADORES
    // Criação da cópia de trabalho para preservar o original
    Jogador jogadores_trabalho[MAX_JOGADORES];
    memcpy(jogadores_trabalho, jogadores_originais, sizeof(jogadores_originais));

    Estatisticas est_jog = selection_sort_jogadores(jogadores_trabalho, MAX_JOGADORES);

    imprimir_jogadores("Tabela Original de Jogadores", jogadores_originais, MAX_JOGADORES);
    imprimir_jogadores("Ranking de Jogadores (Ordenado)", jogadores_trabalho, MAX_JOGADORES);
    printf("Estatísticas (Jogadores) -> Comparações: %ld | Trocas: %ld\n", est_jog.comparacoes, est_jog.trocas);

    return 0;
}