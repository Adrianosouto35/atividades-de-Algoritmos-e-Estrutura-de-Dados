#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
    long deslocamentos; // Registra quantas vezes elementos foram movidos para abrir espaço
} Estatisticas;

// ==========================================
// FUNÇÕES AUXILIARES DE COMPARAÇÃO
// ==========================================

// Retorna true se j1 deve vir ANTES de j2 no ranking (Maior pontuação ou Menor nome alfabético)
bool jogador_vem_antes(const Jogador *j1, const Jogador *j2, long *comparacoes) {
    (*comparacoes)++;
    
    // 1º Critério: Maior Pontuação
    if (j1->pontuacao != j2->pontuacao) {
        return j1->pontuacao > j2->pontuacao;
    }
    
    // 2º Critério (Desempate): Nome em ordem alfabética (crescente)
    return strcmp(j1->nome, j2->nome) < 0;
}

// ==========================================
// 1. SELECTION SORT (ATIVIDADE ANTERIOR)
// ==========================================

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

Estatisticas selection_sort_int(int v[], int n) {
    Estatisticas est = {0, 0};
    for (int i = 0; i < n - 1; i++) {
        int idx_menor = encontrar_indice_menor_int(v, i, n, &est.comparacoes);
        if (idx_menor != i) {
            int temp = v[i];
            v[i] = v[idx_menor];
            v[idx_menor] = temp;
            est.deslocamentos++; // Interpretado como troca no Selection Sort
        }
    }
    return est;
}

int encontrar_indice_melhor_jogador(const Jogador v[], int inicio, int n, long *comparacoes) {
    int idx_melhor = inicio;
    for (int j = inicio + 1; j < n; j++) {
        if (jogador_vem_antes(&v[j], &v[idx_melhor], comparacoes)) {
            idx_melhor = j;
        }
    }
    return idx_melhor;
}

Estatisticas selection_sort_jogadores(Jogador v[], int n) {
    Estatisticas est = {0, 0};
    for (int i = 0; i < n - 1; i++) {
        int idx_melhor = me = encontrar_indice_melhor_jogador(v, i, n, &est.comparacoes);
        if (idx_melhor != i) {
            Jogador temp = v[i];
            v[i] = v[idx_melhor];
            v[idx_melhor] = temp;
            est.deslocamentos++;
        }
    }
    return est;
}

// ==========================================
// 2. INSERTION SORT (NOVA ATIVIDADE)
// ==========================================

// Insertion Sort para vetor numérico (Ordem crescente)
Estatisticas insertion_sort_int(int v[], int n) {
    Estatisticas est = {0, 0};

    for (int i = 1; i < n; i++) {
        // Guardar temporariamente o valor inserido (chave)
        int chave = v[i];
        int j = i - 1;

        // Deslocar elementos da parte ordenada que são maiores que a chave
        // Respeita o limite inferior (j >= 0)
        while (j >= 0) {
            est.comparacoes++;
            if (v[j] > chave) {
                v[j + 1] = v[j]; // Desloca para abrir espaço sem sobrescrever a chave
                est.deslocamentos++;
                j--;
            } else {
                break; // Posição correta encontrada
            }
        }
        
        // Encaixa o valor na posição aberta
        v[j + 1] = chave;
    }

    return est;
}

// Insertion Sort para estrutura de Jogadores (Ranking: Pontuação decrescente, Nome crescente)
Estatisticas insertion_sort_jogadores(Jogador v[], int n) {
    Estatisticas est = {0, 0};

    for (int i = 1; i < n; i++) {
        // Guardar temporariamente a estrutura do jogador (chave)
        Jogador chave = v[i];
        int j = i - 1;

        // Desloca os jogadores que possuem menor prioridade que a chave
        // Respeita o limite inferior (j >= 0)
        while (j >= 0) {
            // Se o jogador em v[j] deve vir DEPOIS da chave, desloca v[j] para a direita
            if (!jogador_vem_antes(&v[j], &chave, &est.comparacoes)) {
                v[j + 1] = v[j]; // Deslocamento da estrutura inteira
                est.deslocamentos++;
                j--;
            } else {
                break; // Posição correta encontrada
            }
        }

        // Insere a estrutura inteira na posição liberada
        v[j + 1] = chave;
    }

    return est;
}

// ==========================================
// FUNÇÕES DE VALIDAÇÃO E EXIBIÇÃO
// ==========================================

bool validar_ordenacao_int(const int v[], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (v[i] > v[i + 1]) return false;
    }
    return true;
}

bool validar_ordenacao_jogadores(const Jogador v[], int n) {
    long comp_dummy = 0;
    for (int i = 0; i < n - 1; i++) {
        // Se v[i+1] deveria vir antes de v[i], a lista não está ordenada
        if (jogador_vem_antes(&v[i + 1], &v[i], &comp_dummy)) {
            return false;
        }
    }
    return true;
}

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
    // 1. DADOS ORIGINAIS DE ENTRADA (Preservados exatos da atividade 1)
    const int num_elementos = 6;
    int pontuacoes_originais[] = {45, 12, 89, 34, 12, 67};

    Jogador jogadores_originais[MAX_JOGADORES] = {
        {101, "Alice", 1500},
        {102, "Bruno", 2100},
        {103, "Carla", 1800},
        {104, "Daniel", 2100}, // Empate com Bruno (Bruno < Daniel)
        {105, "Eduardo", 1500} // Empate com Alice (Alice < Eduardo)
    };

    // 2. EXECUTANDO INSERTION SORT (NUMÉRICO)
    int pontuacoes_insertion[6];
    memcpy(pontuacoes_insertion, pontuacoes_originais, sizeof(pontuacoes_originais));

    Estatisticas est_ins_num = insertion_sort_int(pontuacoes_insertion, num_elementos);

    imprimir_vetor_int("Insertion Sort: Vetor Numérico Ordenado", pontuacoes_insertion, num_elementos);
    printf("Estatísticas -> Comparações: %ld | Deslocamentos: %ld\n", est_ins_num.comparacoes, est_ins_num.deslocamentos);
    printf("Validação do resultado: %s\n", validar_ordenacao_int(pontuacoes_insertion, num_elementos) ? "SUCESSO (Ordenado)" : "FALHA");

    // 3. EXECUTANDO INSERTION SORT (JOGADORES)
    Jogador jogadores_insertion[MAX_JOGADORES];
    memcpy(jogadores_insertion, jogadores_originais, sizeof(jogadores_originais));

    Estatisticas est_ins_jog = insertion_sort_jogadores(jogadores_insertion, MAX_JOGADORES);

    imprimir_jogadores("Insertion Sort: Ranking de Jogadores Ordenado", jogadores_insertion, MAX_JOGADORES);
    printf("Estatísticas -> Comparações: %ld | Deslocamentos: %ld\n", est_ins_jog.comparacoes, est_ins_jog.deslocamentos);
    printf("Validação do resultado: %s\n", validar_ordenacao_jogadores(jogadores_insertion, MAX_JOGADORES) ? "SUCESSO (Ordenado)" : "FALHA");

    return 0;
}