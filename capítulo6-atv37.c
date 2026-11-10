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
    int vida;
} Jogador;

// Estrutura para estatísticas de desempenho
typedef struct {
    long comparacoes;
    long trocas;        // Trocas diretas de posição
    long deslocamentos; // Deslocamentos de posição (Insertion)
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
    
    if (j1->pontuacao != j2->pontuacao) {
        return j1->pontuacao > j2->pontuacao;
    }
    
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
// 4. BUBBLE SORT (ATIVIDADES 35 E ANTERIOR)
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
// 5. INTERCALAÇÃO E ESTRUTURA RECURSIVA (NOVA ATIVIDADE)
// ==========================================

// Intercala dois vetores ordenados (v1 e v2) em um terceiro vetor de saída (v_saida)
Estatisticas intercalar_vetores(const int v1[], int n1, const int v2[], int n2, int v_saida[]) {
    Estatisticas est = {0, 0, 0, 0};

    // Índices independentes para as entradas e a saída
    int i = 0; // Índice para v1
    int j = 0; // Índice para v2
    int k = 0; // Índice para v_saida

    // Compara os elementos das duas entradas e copia o menor
    while (i < n1 && j < n2) {
        est.comparacoes++;
        if (v1[i] <= v2[j]) {
            v_saida[k++] = v1[i++];
        } else {
            v_saida[k++] = v2[j++];
        }
    }

    // Copia os elementos restantes de v1 (se houver)
    while (i < n1) {
        v_saida[k++] = v1[i++];
    }

    // Copia os elementos restantes de v2 (se houver)
    while (j < n2) {
        v_saida[k++] = v2[j++];
    }

    return est;
}

// Estrutura recursiva que demonstra a divisão do intervalo em duas metades
void dividir_recursivo(int v[], int baixo, int alto, int nivel) {
    if (baixo < alto) {
        // Cálculo correto do ponto médio (evita overflow de memória)
        int meio = baixo + (alto - baixo) / 2;

        printf("%*s[Nível %d] Intervalo [%d..%d] -> Dividido em: [%d..%d] e [%d..%d]\n",
               nivel * 2, "", nivel, baixo, alto, baixo, meio, meio + 1, alto);

        // --- ETAPA 1: DIVIDIR & RESOLVER (ESQUERDA) ---
        dividir_recursivo(v, baixo, meio, nivel + 1);

        // --- ETAPA 2: DIVIDIR & RESOLVER (DIREITA) ---
        dividir_recursivo(v, meio + 1, alto, nivel + 1);

        // --- ETAPA 3: COMBINAR (Intercalação a ser concluída na próxima atividade) ---
        // (Aqui ocorrerá a junção intercalada das sub-partes)
    }
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

bool validar_ordenacao_jogadores_generico(const Jogador v[], int n, FuncaoComparacaoJogador fn_comp, DirecaoOrdenacao direcao) {
    long comp_dummy = 0;
    for (int i = 0; i < n - 1; i++) {
        if (fn_comp(&v[i + 1], &v[i], direcao, &comp_dummy)) {
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

void exibir_menu_opcao(const char *campo, DirecaoOrdenacao direcao) {
    printf("\n>>> SELEÇÃO DE CONFIGURAÇÃO DE ORDENAÇÃO <<<\n");
    printf("Campo Selecionado: %s\n", campo);
    printf("Direção Selecionada: %s\n", (direcao == ORDEM_CRESCENTE) ? "CRESCENTE" : "DECRESCENTE");
    printf("-----------------------------------------------\n");
}

// ==========================================
// PROGRAMA PRINCIPAL (CENTRAL DE ORDENAÇÃO)
// ==========================================

int main() {
    // 1. DADOS ORIGINAIS DE ENTRADA (Preservados das atividades anteriores)
    const int num_elementos = 6;
    int pontuacoes_originais[] = {45, 12, 89, 34, 12, 67};

    Jogador jogadores_originais[MAX_JOGADORES] = {
        {105, "Eduardo", 1500, 80},
        {101, "Alice",   1500, 100},
        {104, "Bruno",   1950, 45},
        {102, "Bruno",   2100, 100},
        {106, "Alice",   1750, 20},
        {103, "Carla",   1800, 60}
    };

    printf("===================================================\n");
    printf("           CENTRAL DE ORDENAÇÃO DE JOGADORES       \n");
    printf("===================================================\n");

    imprimir_jogadores("ENTRADA ORIGINAL (JOGADORES BRUTOS)", jogadores_originais, MAX_JOGADORES);

    // --------------------------------------------------
    // NOVA ATIVIDADE: INTERCALAÇÃO E ESTRUTURA RECURSIVA
    // --------------------------------------------------
    printf("\n===================================================\n");
    printf(" NOVA ATIVIDADE: INTERCALAÇÃO E DIVISÃO RECURSIVA  \n");
    printf("===================================================\n");

    // PARTE 1: Intercalação de dois vetores ordenados
    int entrada_A[] = {12, 34, 67};
    int entrada_B[] = {15, 45, 89, 90};
    int tam_A = 3;
    int tam_B = 4;
    int saida_intercalada[7];

    imprimir_vetor_int("Vetor de Entrada A (Já Ordenado)", entrada_A, tam_A);
    imprimir_vetor_int("Vetor de Entrada B (Já Ordenado)", entrada_B, tam_B);

    Estatisticas est_intercalacao = intercalar_vetores(entrada_A, tam_A, entrada_B, tam_B, saida_intercalada);

    imprimir_vetor_int("Vetor Gerado pela Intercalação (Saída Completa)", saida_intercalada, tam_A + tam_B);
    printf("Métricas da Intercalação -> Comparações: %ld\n", est_intercalacao.comparacoes);
    printf("Validação do resultado intercalado: %s\n", 
           validar_ordenacao_int(saida_intercalada, tam_A + tam_B) ? "SUCESSO (Ordenado)" : "FALHA");

    // PARTE 2: Demonstração da Estrutura Recursiva de Divisão
    printf("\n--- Demonstração da Estrutura Recursiva de Divisão ---\n");
    int vetor_demonstracao[] = {45, 12, 89, 34, 12, 67};
    dividir_recursivo(vetor_demonstracao, 0, num_elementos - 1, 0);

    // --------------------------------------------------
    // VERSÕES ANTERIORES PRESERVADAS
    // --------------------------------------------------
    printf("\n===================================================\n");
    printf("             ALGORITMOS DAS ATIVIDADES ANTERIORES  \n");
    printf("===================================================\n");

    // Bubble Sort Genérico (Vida - Decrescente)
    Jogador jogadores_vida_dec[MAX_JOGADORES];
    memcpy(jogadores_vida_dec, jogadores_originais, sizeof(jogadores_originais));
    Estatisticas est_vida = bubble_sort_jogadores_generico(jogadores_vida_dec, MAX_JOGADORES, comparar_por_vida, ORDEM_DECRESCENTE);
    printf("[Bubble Sort Vida Decrescente] -> Comparações: %ld | Trocas: %ld | Passagens: %d\n", est_vida.comparacoes, est_vida.trocas, est_vida.passagens);

    // Selection Sort (Ranking)
    Jogador jogadores_sel[MAX_JOGADORES];
    memcpy(jogadores_sel, jogadores_originais, sizeof(jogadores_originais));
    Estatisticas est_sel = selection_sort_jogadores(jogadores_sel, MAX_JOGADORES);
    printf("[Selection Sort Ranking]       -> Comparações: %ld | Trocas: %ld\n", est_sel.comparacoes, est_sel.trocas);

    // Insertion Sort (Ranking)
    Jogador jogadores_ins_rank[MAX_JOGADORES];
    memcpy(jogadores_ins_rank, jogadores_originais, sizeof(jogadores_originais));
    Estatisticas est_ins_rank = insertion_sort_jogadores_ranking(jogadores_ins_rank, MAX_JOGADORES);
    printf("[Insertion Sort Ranking]       -> Comparações: %ld | Deslocamentos: %ld\n", est_ins_rank.comparacoes, est_ins_rank.deslocamentos);

    // Insertion Sort (Nome)
    Jogador jogadores_ins_nome[MAX_JOGADORES];
    memcpy(jogadores_ins_nome, jogadores_originais, sizeof(jogadores_originais));
    Estatisticas est_ins_nome = insertion_sort_jogadores_nome(jogadores_ins_nome, MAX_JOGADORES);
    printf("[Insertion Sort por Nome]      -> Comparações: %ld | Deslocamentos: %ld\n", est_ins_nome.comparacoes, est_ins_nome.deslocamentos);

    // Quick Sort (Ranking)
    Jogador jogadores_quick[MAX_JOGADORES];
    memcpy(jogadores_quick, jogadores_originais, sizeof(jogadores_originais));
    Estatisticas est_quick = quick_sort_jogadores(jogadores_quick, MAX_JOGADORES);
    printf("[Quick Sort Ranking]           -> Comparações: %ld | Trocas: %ld\n", est_quick.comparacoes, est_quick.trocas);

    return 0;
}