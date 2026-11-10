#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_JOGADORES 6
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
    long trocas;        // Trocas diretas de posição (Selection / Quick)
    long deslocamentos; // Deslocamentos de posição (Insertion)
} Estatisticas;

// ==========================================
// FUNÇÕES AUXILIARES DE COMPARAÇÃO
// ==========================================

// Retorna true se j1 deve vir ANTES de j2 no Ranking (Maior pontuação ou Menor nome alfabético)
bool jogador_vem_antes_ranking(const Jogador *j1, const Jogador *j2, long *comparacoes) {
    (*comparacoes)++;
    
    // 1º Critério: Maior Pontuação (Ordem decrescente)
    if (j1->pontuacao != j2->pontuacao) {
        return j1->pontuacao > j2->pontuacao;
    }
    
    // 2º Critério (Desempate): Nome em ordem alfabética (Ordem crescente)
    return strcmp(j1->nome, j2->nome) < 0;
}

// Retorna true se j1 deve vir ANTES de j2 Alfabeticamente (Nome crescente ou Maior pontuação como desempate)
bool jogador_vem_antes_alfabetico(const Jogador *j1, const Jogador *j2, long *comparacoes) {
    (*comparacoes)++;
    
    int comp_nome = strcmp(j1->nome, j2->nome);
    
    // 1º Critério: Nome em ordem alfabética (Ordem crescente com strcmp)
    if (comp_nome != 0) {
        return comp_nome < 0;
    }
    
    // 2º Critério (Desempate para nomes iguais): Maior Pontuação (Ordem decrescente)
    if (j1->pontuacao != j2->pontuacao) {
        return j1->pontuacao > j2->pontuacao;
    }

    // 3º Critério (Garantia de consistência): Menor ID (Ordem crescente)
    return j1->id < j2->id;
}

// ==========================================
// 1. SELECTION SORT (ATIVIDADE 1)
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
    Estatisticas est = {0, 0, 0};
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

int encontrar_indice_melhor_jogador(const Jogador v[], int inicio, int n, long *comparacoes) {
    int idx_melhor = inicio;
    for (int j = inicio + 1; j < n; j++) {
        if (jogador_vem_antes_ranking(&v[j], &v[idx_melhor], comparacoes)) {
            idx_melhor = j;
        }
    }
    return idx_melhor;
}

Estatisticas selection_sort_jogadores(Jogador v[], int n) {
    Estatisticas est = {0, 0, 0};
    for (int i = 0; i < n - 1; i++) {
        int idx_melhor = encontrar_indice_melhor_jogador(v, i, n, &est.comparacoes);
        if (idx_melhor != i) {
            Jogador temp = v[i];
            v[i] = v[idx_melhor];
            v[idx_melhor] = temp;
            est.trocas++;
        }
    }
    return est;
}

// ==========================================
// 2. INSERTION SORT (ATIVIDADE 2 E ATIVIDADE ATUAL)
// ==========================================

Estatisticas insertion_sort_int(int v[], int n) {
    Estatisticas est = {0, 0, 0};
    for (int i = 1; i < n; i++) {
        int chave = v[i];
        int j = i - 1;

        while (j >= 0) {
            est.comparacoes++;
            if (v[j] > chave) {
                v[j + 1] = v[j];
                est.deslocamentos++;
                j--;
            } else {
                break;
            }
        }
        v[j + 1] = chave;
    }
    return est;
}

// Insertion Sort por Ranking (Pontuação)
Estatisticas insertion_sort_jogadores_ranking(Jogador v[], int n) {
    Estatisticas est = {0, 0, 0};
    for (int i = 1; i < n; i++) {
        Jogador chave = v[i]; // Estrutura temporária completa
        int j = i - 1;

        while (j >= 0) {
            if (!jogador_vem_antes_ranking(&v[j], &chave, &est.comparacoes)) {
                v[j + 1] = v[j]; // Desloca todos os campos juntos
                est.deslocamentos++;
                j--;
            } else {
                break;
            }
        }
        v[j + 1] = chave;
    }
    return est;
}

// Insertion Sort por Nome (Ordem Alfabética com Desempate)
Estatisticas insertion_sort_jogadores_nome(Jogador v[], int n) {
    Estatisticas est = {0, 0, 0};
    for (int i = 1; i < n; i++) {
        Jogador chave = v[i]; // Estrutura temporária completa guardando ID, Nome e Pontuação
        int j = i - 1;

        // Desloca elementos enquanto o elemento atual v[j] deve vir DEPOIS da chave
        while (j >= 0) {
            if (!jogador_vem_antes_alfabetico(&v[j], &chave, &est.comparacoes)) {
                v[j + 1] = v[j]; // Move a estrutura inteira abrindo espaço sem sobrescrever
                est.deslocamentos++;
                j--;
            } else {
                break;
            }
        }
        v[j + 1] = chave; // Insere a estrutura completa na posição aberta
    }
    return est;
}

// ==========================================
// 3. QUICK SORT (ATIVIDADE 3)
// ==========================================

int particionar_int(int v[], int baixo, int alto, Estatisticas *est) {
    int pivo = v[alto];
    int i = baixo - 1;

    for (int j = baixo; j < alto; j++) {
        est->comparacoes++;
        if (v[j] <= pivo) {
            i++;
            if (i != j) {
                int temp = v[i];
                v[i] = v[j];
                v[j] = temp;
                est->trocas++;
            }
        }
    }

    if ((i + 1) != alto) {
        int temp = v[i + 1];
        v[i + 1] = v[alto];
        v[alto] = temp;
        est->trocas++;
    }

    return i + 1;
}

void quick_sort_int_recursivo(int v[], int baixo, int alto, Estatisticas *est) {
    if (baixo < alto) {
        int pivo_idx = particionar_int(v, baixo, alto, est);
        quick_sort_int_recursivo(v, baixo, pivo_idx - 1, est);
        quick_sort_int_recursivo(v, pivo_idx + 1, alto, est);
    }
}

Estatisticas quick_sort_int(int v[], int n) {
    Estatisticas est = {0, 0, 0};
    if (n > 0) {
        quick_sort_int_recursivo(v, 0, n - 1, &est);
    }
    return est;
}

int particionar_jogadores(Jogador v[], int baixo, int alto, Estatisticas *est) {
    Jogador pivo = v[alto];
    int i = baixo - 1;

    for (int j = baixo; j < alto; j++) {
        if (jogador_vem_antes_ranking(&v[j], &pivo, &est->comparacoes)) {
            i++;
            if (i != j) {
                Jogador temp = v[i];
                v[i] = v[j];
                v[j] = temp;
                est->trocas++;
            }
        }
    }

    if ((i + 1) != alto) {
        Jogador temp = v[i + 1];
        v[i + 1] = v[alto];
        v[alto] = temp;
        est->trocas++;
    }

    return i + 1;
}

void quick_sort_jogadores_recursivo(Jogador v[], int baixo, int alto, Estatisticas *est) {
    if (baixo < alto) {
        int pivo_idx = particionar_jogadores(v, baixo, alto, est);
        quick_sort_jogadores_recursivo(v, baixo, pivo_idx - 1, est);
        quick_sort_jogadores_recursivo(v, pivo_idx + 1, alto, est);
    }
}

Estatisticas quick_sort_jogadores(Jogador v[], int n) {
    Estatisticas est = {0, 0, 0};
    if (n > 0) {
        quick_sort_jogadores_recursivo(v, 0, n - 1, &est);
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

bool validar_ordenacao_jogadores_ranking(const Jogador v[], int n) {
    long comp_dummy = 0;
    for (int i = 0; i < n - 1; i++) {
        if (jogador_vem_antes_ranking(&v[i + 1], &v[i], &comp_dummy)) {
            return false;
        }
    }
    return true;
}

bool validar_ordenacao_jogadores_nome(const Jogador v[], int n) {
    long comp_dummy = 0;
    for (int i = 0; i < n - 1; i++) {
        if (jogador_vem_antes_alfabetico(&v[i + 1], &v[i], &comp_dummy)) {
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
// PROGRAMA PRINCIPAL (CENTRAL DE ORDENAÇÃO)
// ==========================================

int main() {
    // 1. DADOS ORIGINAIS DE ENTRADA (Preservados com adição de nomes repetidos para teste)
    const int num_elementos = 6;
    int pontuacoes_originais[] = {45, 12, 89, 34, 12, 67};

    Jogador jogadores_originais[MAX_JOGADORES] = {
        {101, "Alice", 1500},
        {102, "Bruno", 2100},
        {103, "Carla", 1800},
        {104, "Bruno", 1950}, // NOME REPETIDO: Bruno com pontuação diferente para teste de desempate
        {105, "Eduardo", 1500},
        {106, "Alice", 1750}  // NOME REPETIDO: Alice com maior pontuação
    };

    printf("===================================================\n");
    printf("           CENTRAL DE ORDENAÇÃO DE JOGADORES       \n");
    printf("===================================================\n");

    // Exibição dos dados originais
    imprimir_jogadores("ENTRADA ORIGINAL (JOGADORES BRUTOS)", jogadores_originais, MAX_JOGADORES);

    // --------------------------------------------------
    // OPÇÃO NOVA: INSERTION SORT POR NOME (ALFABÉTICO)
    // --------------------------------------------------
    Jogador jogadores_nome[MAX_JOGADORES];
    memcpy(jogadores_nome, jogadores_originais, sizeof(jogadores_originais));

    Estatisticas est_ins_nome = insertion_sort_jogadores_nome(jogadores_nome, MAX_JOGADORES);

    imprimir_jogadores("RESULTADO: INSERTION SORT (POR NOME/ALFABÉTICO)", jogadores_nome, MAX_JOGADORES);
    printf("Estatísticas -> Comparações: %ld | Deslocamentos: %ld\n", est_ins_nome.comparacoes, est_ins_nome.deslocamentos);
    printf("Validação do resultado: %s\n", 
           validar_ordenacao_jogadores_nome(jogadores_nome, MAX_JOGADORES) ? "SUCESSO (Ordenado)" : "FALHA");

    // --------------------------------------------------
    // OPÇÃO 1: SELECTION SORT (RANKING / PONTUAÇÃO)
    // --------------------------------------------------
    Jogador jogadores_sel[MAX_JOGADORES];
    memcpy(jogadores_sel, jogadores_originais, sizeof(jogadores_originais));
    Estatisticas est_sel = selection_sort_jogadores(jogadores_sel, MAX_JOGADORES);

    imprimir_jogadores("RESULTADO: SELECTION SORT (RANKING)", jogadores_sel, MAX_JOGADORES);
    printf("Estatísticas -> Comparações: %ld | Trocas: %ld\n", est_sel.comparacoes, est_sel.trocas);

    // --------------------------------------------------
    // OPÇÃO 2: INSERTION SORT (RANKING / PONTUAÇÃO)
    // --------------------------------------------------
    Jogador jogadores_ins_rank[MAX_JOGADORES];
    memcpy(jogadores_ins_rank, jogadores_originais, sizeof(jogadores_originais));
    Estatisticas est_ins_rank = insertion_sort_jogadores_ranking(jogadores_ins_rank, MAX_JOGADORES);

    imprimir_jogadores("RESULTADO: INSERTION SORT (RANKING)", jogadores_ins_rank, MAX_JOGADORES);
    printf("Estatísticas -> Comparações: %ld | Deslocamentos: %ld\n", est_ins_rank.comparacoes, est_ins_rank.deslocamentos);

    // --------------------------------------------------
    // OPÇÃO 3: QUICK SORT (RANKING / PONTUAÇÃO)
    // --------------------------------------------------
    Jogador jogadores_quick[MAX_JOGADORES];
    memcpy(jogadores_quick, jogadores_originais, sizeof(jogadores_originais));
    Estatisticas est_quick = quick_sort_jogadores(jogadores_quick, MAX_JOGADORES);

    imprimir_jogadores("RESULTADO: QUICK SORT (RANKING)", jogadores_quick, MAX_JOGADORES);
    printf("Estatísticas -> Comparações: %ld | Trocas: %ld\n", est_quick.comparacoes, est_quick.trocas);

    return 0;
}