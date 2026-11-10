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
    long trocas;        // Trocas diretas de posição (Selection / Quick / Bubble)
    long deslocamentos; // Deslocamentos de posição (Insertion)
    int passagens;      // Número de passagens realizadas pelo algoritmo (Bubble)
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
    Estatisticas est = {0, 0, 0, 0};
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
    Estatisticas est = {0, 0, 0, 0};
    for (int i = 0; i < n - 1; i++) {
        int idx_melhor = me = encontrar_indice_melhor_jogador(v, i, n, &est.comparacoes);
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
// 2. INSERTION SORT (ATIVIDADES 2 E 34)
// ==========================================

Estatisticas insertion_sort_int(int v[], int n) {
    Estatisticas est = {0, 0, 0, 0};
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

Estatisticas insertion_sort_jogadores_ranking(Jogador v[], int n) {
    Estatisticas est = {0, 0, 0, 0};
    for (int i = 1; i < n; i++) {
        Jogador chave = v[i];
        int j = i - 1;

        while (j >= 0) {
            if (!jogador_vem_antes_ranking(&v[j], &chave, &est.comparacoes)) {
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

Estatisticas insertion_sort_jogadores_nome(Jogador v[], int n) {
    Estatisticas est = {0, 0, 0, 0};
    for (int i = 1; i < n; i++) {
        Jogador chave = v[i];
        int j = i - 1;

        while (j >= 0) {
            if (!jogador_vem_antes_alfabetico(&v[j], &chave, &est.comparacoes)) {
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
    Estatisticas est = {0, 0, 0, 0};
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
    Estatisticas est = {0, 0, 0, 0};
    if (n > 0) {
        quick_sort_jogadores_recursivo(v, 0, n - 1, &est);
    }
    return est;
}

// ==========================================
// 4. BUBBLE SORT OTIMIZADO (ATIVIDADE 35 - NOVA)
// ==========================================

// Bubble Sort Otimizado para vetor numérico
Estatisticas bubble_sort_int(int v[], int n) {
    Estatisticas est = {0, 0, 0, 0};
    bool trocou;

    for (int i = 0; i < n - 1; i++) {
        trocou = false;
        est.passagens++;

        // Redução da faixa útil: (n - 1 - i) ignora os últimos elementos já ordenados
        for (int j = 0; j < n - 1 - i; j++) {
            est.comparacoes++;
            // Compara elementos vizinhos
            if (v[j] > v[j + 1]) {
                int temp = v[j];
                v[j] = v[j + 1];
                v[j + 1] = temp;
                est.trocas++;
                trocou = true;
            }
        }

        // Se nenhuma troca ocorreu na passagem inteira, o vetor já está ordenado
        if (!trocou) {
            break;
        }
    }

    return est;
}

// Bubble Sort Otimizado para estrutura de Jogadores (Ranking)
Estatisticas bubble_sort_jogadores(Jogador v[], int n) {
    Estatisticas est = {0, 0, 0, 0};
    bool trocou;

    for (int i = 0; i < n - 1; i++) {
        trocou = false;
        est.passagens++;

        // Redução da faixa útil
        for (int j = 0; j < n - 1 - i; j++) {
            // Se v[j+1] deve vir antes de v[j] no ranking, troca os vizinhos
            if (jogador_vem_antes_ranking(&v[j + 1], &v[j], &est.comparacoes)) {
                Jogador temp = v[j];
                v[j] = v[j + 1];
                v[j + 1] = temp;
                est.trocas++;
                trocou = true;
            }
        }

        // Interrompe precocemente se nenhuma troca ocorreu
        if (!trocou) {
            break;
        }
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
    // 1. DADOS ORIGINAIS DE ENTRADA (Preservados das atividades anteriores)
    const int num_elementos = 6;
    int pontuacoes_originais[] = {45, 12, 89, 34, 12, 67};

    Jogador jogadores_originais[MAX_JOGADORES] = {
        {101, "Alice", 1500},
        {102, "Bruno", 2100},
        {103, "Carla", 1800},
        {104, "Bruno", 1950}, 
        {105, "Eduardo", 1500},
        {106, "Alice", 1750}  
    };

    printf("===================================================\n");
    printf("           CENTRAL DE ORDENAÇÃO DE JOGADORES       \n");
    printf("===================================================\n");

    imprimir_jogadores("ENTRADA ORIGINAL (JOGADORES BRUTOS)", jogadores_originais, MAX_JOGADORES);

    // --------------------------------------------------
    // NOVA ATIVIDADE (35): BUBBLE SORT OTIMIZADO NOS 3 CENÁRIOS
    // --------------------------------------------------
    printf("\n===================================================\n");
    printf("       TESTES BUBBLE SORT OTIMIZADO (ATIVIDADE 35) \n");
    printf("===================================================\n");

    // Cenários de Vetores Numéricos
    int vetor_aleatorio[] = {45, 12, 89, 34, 12, 67};
    int vetor_ordenado[]  = {12, 12, 34, 45, 67, 89};
    int vetor_inverso[]   = {89, 67, 45, 34, 12, 12};

    // Teste 1: Entrada Aleatória
    Estatisticas est_bubble_aleatorio = bubble_sort_int(vetor_aleatorio, num_elementos);
    imprimir_vetor_int("Bubble Sort - Entrada Aleatória", vetor_aleatorio, num_elementos);
    printf("Métricas [Aleatório] -> Passagens: %d | Comparações: %ld | Trocas: %ld\n", 
           est_bubble_aleatorio.passagens, est_bubble_aleatorio.comparacoes, est_bubble_aleatorio.trocas);

    // Teste 2: Entrada Já Ordenada (Demostra parada precoce)
    Estatisticas est_bubble_ordenado = bubble_sort_int(vetor_ordenado, num_elementos);
    imprimir_vetor_int("Bubble Sort - Entrada Já Ordenada", vetor_ordenado, num_elementos);
    printf("Métricas [Já Ordenado] -> Passagens: %d | Comparações: %ld | Trocas: %ld\n", 
           est_bubble_ordenado.passagens, est_bubble_ordenado.comparacoes, est_bubble_ordenado.trocas);

    // Teste 3: Entrada em Ordem Inversa
    Estatisticas est_bubble_inverso = bubble_sort_int(vetor_inverso, num_elementos);
    imprimir_vetor_int("Bubble Sort - Entrada Inversa", vetor_inverso, num_elementos);
    printf("Métricas [Inverso]     -> Passagens: %d | Comparações: %ld | Trocas: %ld\n", 
           est_bubble_inverso.passagens, est_bubble_inverso.comparacoes, est_bubble_inverso.trocas);

    // Teste de Jogadores com Bubble Sort
    Jogador jogadores_bubble[MAX_JOGADORES];
    memcpy(jogadores_bubble, jogadores_originais, sizeof(jogadores_originais));
    Estatisticas est_bubble_jog = bubble_sort_jogadores(jogadores_bubble, MAX_JOGADORES);
    imprimir_jogadores("Bubble Sort - Ranking de Jogadores", jogadores_bubble, MAX_JOGADORES);
    printf("Métricas [Jogadores]  -> Passagens: %d | Comparações: %ld | Trocas: %ld\n", 
           est_bubble_jog.passagens, est_bubble_jog.comparacoes, est_bubble_jog.trocas);
    printf("Validação do resultado: %s\n", 
           validar_ordenacao_jogadores_ranking(jogadores_bubble, MAX_JOGADORES) ? "SUCESSO (Ordenado)" : "FALHA");

    // --------------------------------------------------
    // VERSÕES ANTERIORES PRESERVADAS
    // --------------------------------------------------
    printf("\n===================================================\n");
    printf("             ALGORITMOS DAS ATIVIDADES ANTERIORES  \n");
    printf("===================================================\n");

    // Selection Sort (Ranking)
    Jogador jogadores_sel[MAX_JOGADORES];
    memcpy(jogadores_sel, jogadores_originais, sizeof(jogadores_originais));
    Estatisticas est_sel = selection_sort_jogadores(jogadores_sel, MAX_JOGADORES);
    printf("\n[Selection Sort Ranking]   -> Comparações: %ld | Trocas: %ld\n", est_sel.comparacoes, est_sel.trocas);

    // Insertion Sort (Ranking)
    Jogador jogadores_ins_rank[MAX_JOGADORES];
    memcpy(jogadores_ins_rank, jogadores_originais, sizeof(jogadores_originais));
    Estatisticas est_ins_rank = insertion_sort_jogadores_ranking(jogadores_ins_rank, MAX_JOGADORES);
    printf("[Insertion Sort Ranking]   -> Comparações: %ld | Deslocamentos: %ld\n", est_ins_rank.comparacoes, est_ins_rank.deslocamentos);

    // Insertion Sort (Nome)
    Jogador jogadores_ins_nome[MAX_JOGADORES];
    memcpy(jogadores_ins_nome, jogadores_originais, sizeof(jogadores_originais));
    Estatisticas est_ins_nome = insertion_sort_jogadores_nome(jogadores_ins_nome, MAX_JOGADORES);
    printf("[Insertion Sort por Nome]  -> Comparações: %ld | Deslocamentos: %ld\n", est_ins_nome.comparacoes, est_ins_nome.deslocamentos);

    // Quick Sort (Ranking)
    Jogador jogadores_quick[MAX_JOGADORES];
    memcpy(jogadores_quick, jogadores_originais, sizeof(jogadores_originais));
    Estatisticas est_quick = quick_sort_jogadores(jogadores_quick, MAX_JOGADORES);
    printf("[Quick Sort Ranking]       -> Comparações: %ld | Trocas: %ld\n", est_quick.comparacoes, est_quick.trocas);

    return 0;
}