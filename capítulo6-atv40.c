#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_JOGADORES 6
#define MAX_NOME 50

// Estrutura para representar um Jogador
typedef struct {
    int id;
    char nome[MAX_NOME];
    int pontuacao;
    int vida;
} Jogador;

// Estrutura para estatísticas de desempenho
typedef struct {
    long comparacoes;
    long trocas;        // Trocas diretas de posição (Selection / Quick / Bubble)
    long deslocamentos; // Deslocamentos de posição (Insertion / Merge)
    int passagens;      // Número de passagens realizadas (Bubble)
} Estatisticas;

// Enumeração para a direção da ordenação
typedef enum {
    ORDEM_CRESCENTE,
    ORDEM_DECRESCENTE
} DirecaoOrdenacao;

// Ponteiro de função de comparação flexível
typedef bool (*FuncaoComparacaoJogador)(const Jogador*, const Jogador*, DirecaoOrdenacao, long*);

// ==========================================
// FUNÇÕES AUXILIARES DE COMPARAÇÃO
// ==========================================

bool jogador_vem_antes_ranking(const Jogador *j1, const Jogador *j2, long *comparacoes) {
    if (comparacoes) (*comparacoes)++;
    
    // 1º Critério: Maior Pontuação (Ordem decrescente)
    if (j1->pontuacao != j2->pontuacao) {
        return j1->pontuacao > j2->pontuacao;
    }
    
    // 2º Critério (Desempate): Nome em ordem alfabética (Ordem crescente)
    return strcmp(j1->nome, j2->nome) < 0;
}

bool jogador_vem_antes_alfabetico(const Jogador *j1, const Jogador *j2, long *comparacoes) {
    if (comparacoes) (*comparacoes)++;
    
    int comp_nome = strcmp(j1->nome, j2->nome);
    
    if (comp_nome != 0) {
        return comp_nome < 0;
    }
    
    if (j1->pontuacao != j2->pontuacao) {
        return j1->pontuacao > j2->pontuacao;
    }

    return j1->id < j2->id;
}

bool comparar_por_id(const Jogador *j1, const Jogador *j2, DirecaoOrdenacao direcao, long *comparacoes) {
    if (comparacoes) (*comparacoes)++;
    if (j1->id == j2->id) return false;

    if (direcao == ORDEM_CRESCENTE) {
        return j1->id < j2->id;
    } else {
        return j1->id > j2->id;
    }
}

bool comparar_por_vida(const Jogador *j1, const Jogador *j2, DirecaoOrdenacao direcao, long *comparacoes) {
    if (comparacoes) (*comparacoes)++;
    
    if (j1->vida != j2->vida) {
        if (direcao == ORDEM_CRESCENTE) {
            return j1->vida < j2->vida;
        } else {
            return j1->vida > j2->vida;
        }
    }

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
// 2. INSERTION SORT (ATIVIDADES 2, 34)
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
// 3. BUBBLE SORT (ATIVIDADES 35 E EDICAO FLEXIVEL)
// ==========================================

Estatisticas bubble_sort_int(int v[], int n) {
    Estatisticas est = {0, 0, 0, 0};
    bool trocou;

    for (int i = 0; i < n - 1; i++) {
        trocou = false;
        est.passagens++;

        for (int j = 0; j < n - 1 - i; j++) {
            est.comparacoes++;
            if (v[j] > v[j + 1]) {
                int temp = v[j];
                v[j] = v[j + 1];
                v[j + 1] = temp;
                est.trocas++;
                trocou = true;
            }
        }

        if (!trocou) break;
    }

    return est;
}

Estatisticas bubble_sort_jogadores_generico(Jogador v[], int n, FuncaoComparacaoJogador fn_comp, DirecaoOrdenacao direcao) {
    Estatisticas est = {0, 0, 0, 0};
    bool trocou;

    for (int i = 0; i < n - 1; i++) {
        trocou = false;
        est.passagens++;

        for (int j = 0; j < n - 1 - i; j++) {
            if (fn_comp(&v[j + 1], &v[j], direcao, &est.comparacoes)) {
                Jogador temp = v[j];
                v[j] = v[j + 1];
                v[j + 1] = temp;
                est.trocas++;
                trocou = true;
            }
        }

        if (!trocou) break;
    }

    return est;
}

// ==========================================
// 4. MERGE SORT
// ==========================================

void intercalar_int(int v[], int baixo, int meio, int alto, Estatisticas *est) {
    int n1 = meio - baixo + 1;
    int n2 = alto - meio;

    int *E = (int *)malloc(n1 * sizeof(int));
    int *D = (int *)malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++) E[i] = v[baixo + i];
    for (int j = 0; j < n2; j++) D[j] = v[meio + 1 + j];

    int i = 0, j = 0, k = baixo;

    while (i < n1 && j < n2) {
        est->comparacoes++;
        if (E[i] <= D[j]) {
            v[k++] = E[i++];
        } else {
            v[k++] = D[j++];
        }
        est->deslocamentos++;
    }

    while (i < n1) {
        v[k++] = E[i++];
        est->deslocamentos++;
    }
    while (j < n2) {
        v[k++] = D[j++];
        est->deslocamentos++;
    }

    free(E);
    free(D);
}

void merge_sort_int_recursivo(int v[], int baixo, int alto, Estatisticas *est) {
    if (baixo < alto) {
        int meio = baixo + (alto - baixo) / 2;
        merge_sort_int_recursivo(v, baixo, meio, est);
        merge_sort_int_recursivo(v, meio + 1, alto, est);
        intercalar_int(v, baixo, meio, alto, est);
    }
}

Estatisticas merge_sort_int(int v[], int n) {
    Estatisticas est = {0, 0, 0, 0};
    if (n > 1) {
        merge_sort_int_recursivo(v, 0, n - 1, &est);
    }
    return est;
}

void intercalar_jogadores(Jogador v[], int baixo, int meio, int alto, Estatisticas *est) {
    int n1 = meio - baixo + 1;
    int n2 = alto - meio;

    Jogador *E = (Jogador *)malloc(n1 * sizeof(Jogador));
    Jogador *D = (Jogador *)malloc(n2 * sizeof(Jogador));

    for (int i = 0; i < n1; i++) E[i] = v[baixo + i];
    for (int j = 0; j < n2; j++) D[j] = v[meio + 1 + j];

    int i = 0, j = 0, k = baixo;

    while (i < n1 && j < n2) {
        if (jogador_vem_antes_ranking(&E[i], &D[j], &est->comparacoes)) {
            v[k++] = E[i++];
        } else {
            v[k++] = D[j++];
        }
        est->deslocamentos++;
    }

    while (i < n1) {
        v[k++] = E[i++];
        est->deslocamentos++;
    }
    while (j < n2) {
        v[k++] = D[j++];
        est->deslocamentos++;
    }

    free(E);
    free(D);
}

void merge_sort_jogadores_recursivo(Jogador v[], int baixo, int alto, Estatisticas *est) {
    if (baixo < alto) {
        int meio = baixo + (alto - baixo) / 2;
        merge_sort_jogadores_recursivo(v, baixo, meio, est);
        merge_sort_jogadores_recursivo(v, meio + 1, alto, est);
        intercalar_jogadores(v, baixo, meio, alto, est);
    }
}

Estatisticas merge_sort_jogadores(Jogador v[], int n) {
    Estatisticas est = {0, 0, 0, 0};
    if (n > 1) {
        merge_sort_jogadores_recursivo(v, 0, n - 1, &est);
    }
    return est;
}

// ==========================================
// 5. QUICK SORT COM PARTICIONAMENTO E PIVÔ ALEATÓRIO (NOVA ATIVIDADE)
// ==========================================

// Particionamento In-Place Padrão (Lomuto)
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

// Particionamento Pseudoaleatório para Inteiros
int particionar_randomico_int(int v[], int baixo, int alto, Estatisticas *est) {
    // Escolhe um pivô aleatório estritamente dentro do subintervalo [baixo..alto]
    int idx_rand = baixo + rand() % (alto - baixo + 1);

    // Coloca o pivô sorteado na posição final esperada pela partição (alto)
    if (idx_rand != alto) {
        int temp = v[idx_rand];
        v[idx_rand] = v[alto];
        v[alto] = temp;
        est->trocas++;
    }

    return particionar_int(v, baixo, alto, est);
}

void quick_sort_randomico_int_recursivo(int v[], int baixo, int alto, Estatisticas *est) {
    if (baixo < alto) {
        int pivo_idx = particionar_randomico_int(v, baixo, alto, est);
        quick_sort_randomico_int_recursivo(v, baixo, pivo_idx - 1, est);
        quick_sort_randomico_int_recursivo(v, pivo_idx + 1, alto, est);
    }
}

Estatisticas quick_sort_randomico_int(int v[], int n) {
    Estatisticas est = {0, 0, 0, 0};
    if (n > 0) {
        quick_sort_randomico_int_recursivo(v, 0, n - 1, &est);
    }
    return est;
}

// Particionamento In-Place Padrão para Jogadores
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

// Particionamento Pseudoaleatório para Estruturas de Jogadores
int particionar_randomico_jogadores(Jogador v[], int baixo, int alto, Estatisticas *est) {
    // Escolhe um pivô aleatório estritamente dentro do subintervalo
    int idx_rand = baixo + rand() % (alto - baixo + 1);

    // Troca o pivô aleatório com a última posição (alto) para alinhamento com a estrutura
    if (idx_rand != alto) {
        Jogador temp = v[idx_rand];
        v[idx_rand] = v[alto];
        v[alto] = temp;
        est->trocas++;
    }

    return particionar_jogadores(v, baixo, alto, est);
}

void quick_sort_randomico_jogadores_recursivo(Jogador v[], int baixo, int alto, Estatisticas *est) {
    if (baixo < alto) {
        int pivo_idx = particionar_randomico_jogadores(v, baixo, alto, est);
        quick_sort_randomico_jogadores_recursivo(v, baixo, pivo_idx - 1, est);
        quick_sort_randomico_jogadores_recursivo(v, pivo_idx + 1, alto, est);
    }
}

Estatisticas quick_sort_randomico_jogadores(Jogador v[], int n) {
    Estatisticas est = {0, 0, 0, 0};
    if (n > 0) {
        quick_sort_randomico_jogadores_recursivo(v, 0, n - 1, &est);
    }
    return est;
}

// Quick Sort Clássico (Pivô Fixo) para Jogadores
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

void imprimir_vetor_int(const char *titulo, const int v[], int n) {
    printf("\n--- %s ---\n", titulo);
    for (int i = 0; i < n; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");
}

void imprimir_jogadores(const char *titulo, const Jogador v[], int n) {
    printf("\n--- %s ---\n", titulo);
    printf("%-5s | %-15s | %-8s | %-6s\n", "ID", "Nome", "Pontos", "Vida");
    printf("-----------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%-5d | %-15s | %-8d | %-6d\n", v[i].id, v[i].nome, v[i].pontuacao, v[i].vida);
    }
}

// Relatório Comparativo Final Unificado
void exibir_relatorio_comparativo(Jogador entrada_base[], int n) {
    printf("\n=========================================================================\n");
    printf("        RELATÓRIO COMPARATIVO FINAL DE MÉTRICAS (MESMA ENTRADA)          \n");
    printf("=========================================================================\n");
    printf("%-28s | %-12s | %-10s | %-13s\n", "Algoritmo", "Comparações", "Trocas", "Deslocamentos");
    printf("-------------------------------------------------------------------------\n");

    // 1. Selection Sort
    Jogador c_sel[MAX_JOGADORES]; memcpy(c_sel, entrada_base, sizeof(Jogador) * n);
    Estatisticas e_sel = selection_sort_jogadores(c_sel, n);
    printf("%-28s | %-12ld | %-10ld | %-13s\n", "Selection Sort", e_sel.comparacoes, e_sel.trocas, "-");

    // 2. Insertion Sort (Ranking)
    Jogador c_ins[MAX_JOGADORES]; memcpy(c_ins, entrada_base, sizeof(Jogador) * n);
    Estatisticas e_ins = insertion_sort_jogadores_ranking(c_ins, n);
    printf("%-28s | %-12ld | %-10s | %-13ld\n", "Insertion Sort (Ranking)", e_ins.comparacoes, "-", e_ins.deslocamentos);

    // 3. Insertion Sort (Nome)
    Jogador c_nome[MAX_JOGADORES]; memcpy(c_nome, entrada_base, sizeof(Jogador) * n);
    Estatisticas e_nome = insertion_sort_jogadores_nome(c_nome, n);
    printf("%-28s | %-12ld | %-10s | %-13ld\n", "Insertion Sort (Nome)", e_nome.comparacoes, "-", e_nome.deslocamentos);

    // 4. Bubble Sort Otimizado
    Jogador c_bub[MAX_JOGADORES]; memcpy(c_bub, entrada_base, sizeof(Jogador) * n);
    Estatisticas e_bub = bubble_sort_jogadores_generico(c_bub, n, comparar_por_vida, ORDEM_DECRESCENTE);
    printf("%-28s | %-12ld | %-10ld | %-13s\n", "Bubble Sort (Vida Decr.)", e_bub.comparacoes, e_bub.trocas, "-");

    // 5. Merge Sort
    Jogador c_mer[MAX_JOGADORES]; memcpy(c_mer, entrada_base, sizeof(Jogador) * n);
    Estatisticas e_mer = merge_sort_jogadores(c_mer, n);
    printf("%-28s | %-12ld | %-10s | %-13ld\n", "Merge Sort", e_mer.comparacoes, "-", e_mer.deslocamentos);

    // 6. Quick Sort (Pivô Fixo)
    Jogador c_qfk[MAX_JOGADORES]; memcpy(c_qfk, entrada_base, sizeof(Jogador) * n);
    Estatisticas e_qfk = quick_sort_jogadores(c_qfk, n);
    printf("%-28s | %-12ld | %-10ld | %-13s\n", "Quick Sort (Pivô Fixo)", e_qfk.comparacoes, e_qfk.trocas, "-");

    // 7. Quick Sort (Pivô Randomico)
    Jogador c_qrnd[MAX_JOGADORES]; memcpy(c_qrnd, entrada_base, sizeof(Jogador) * n);
    Estatisticas e_qrnd = quick_sort_randomico_jogadores(c_qrnd, n);
    printf("%-28s | %-12ld | %-10ld | %-13s\n", "Quick Sort (Pivô Randômico)", e_qrnd.comparacoes, e_qrnd.trocas, "-");
    printf("=========================================================================\n");
}

// ==========================================
// PROGRAMA PRINCIPAL COM MENU INTERATIVO
// ==========================================

int main() {
    // Inicialização ÚNICA do gerador pseudoaleatório no programa
    srand((unsigned int)time(NULL));

    Jogador jogadores_originais[MAX_JOGADORES] = {
        {105, "Eduardo", 1500, 80},
        {101, "Alice",   1500, 100},
        {104, "Bruno",   1950, 45},
        {102, "Bruno",   2100, 100},
        {106, "Alice",   1750, 20},
        {103, "Carla",   1800, 60}
    };

    int opcao = -1;

    while (opcao != 0) {
        printf("\n===================================================\n");
        printf("    CENTRAL DE ORDENAÇÃO DE JOGADORES (MENU)       \n");
        printf("===================================================\n");
        printf("1. Exibir Entrada Original\n");
        printf("2. Executar Quick Sort com Pivô Randômico (Nova Atividade)\n");
        printf("3. Executar Outros Algoritmos (Selection, Insertion, Bubble, Merge)\n");
        printf("4. Gerar Relatório Comparativo Unificado (Todas as Métricas)\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        if (scanf("%d", &opcao) != 1) break;

        switch (opcao) {
            case 1:
                imprimir_jogadores("ENTRADA ORIGINAL PRESERVADA", jogadores_originais, MAX_JOGADORES);
                break;
            case 2: {
                // Quick Sort Randômico sobre uma CÓPIA limpa
                Jogador copia[MAX_JOGADORES];
                memcpy(copia, jogadores_originais, sizeof(jogadores_originais));

                Estatisticas est = quick_sort_randomico_jogadores(copia, MAX_JOGADORES);
                imprimir_jogadores("RESULTADO: QUICK SORT COM PIVÔ RANDÔMICO", copia, MAX_JOGADORES);
                printf("Métricas -> Comparações: %ld | Trocas: %ld\n", est.comparacoes, est.trocas);
                printf("Validação: %s\n", validar_ordenacao_jogadores_ranking(copia, MAX_JOGADORES) ? "SUCESSO" : "FALHA");
                break;
            }
            case 3: {
                Jogador copia[MAX_JOGADORES];
                memcpy(copia, jogadores_originais, sizeof(jogadores_originais));

                Estatisticas est = merge_sort_jogadores(copia, MAX_JOGADORES);
                imprimir_jogadores("RESULTADO: MERGE SORT", copia, MAX_JOGADORES);
                printf("Métricas -> Comparações: %ld | Deslocamentos: %ld\n", est.comparacoes, est.deslocamentos);
                break;
            }
            case 4:
                exibir_relatorio_comparativo(jogadores_originais, MAX_JOGADORES);
                break;
            case 0:
                printf("\nEncerrando a Central de Ordenação...\n");
                break;
            default:
                printf("\nOpção inválida! Tente novamente.\n");
        }
    }

    return 0;
}