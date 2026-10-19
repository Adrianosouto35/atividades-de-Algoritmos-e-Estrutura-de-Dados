#include <stdio.h>
#include <stdlib.h>

typedef struct {
    long acessos;
    long comparacoes;
} Contadores;

void zerar_contadores(Contadores *cnt) {
    cnt->acessos = 0;
    cnt->comparacoes = 0;
}

int acesso_direto(int vetor[], int tamanho, int indice, Contadores *cnt) {
    cnt->comparacoes++;
    cnt->comparacoes++;
    
    if (indice >= 0 && indice < tamanho) {
        cnt->acessos++;
        return vetor[indice];
    } else {
        return -1;
    }
}

int buscar_maior(int vetor[], int tamanho, Contadores *cnt) {
    if (tamanho <= 0) return -1;

    cnt->acessos++;
    int maior = vetor[0];

    for (int i = 1; i < tamanho; i++) {
        cnt->acessos++;
        cnt->comparacoes++;
        
        if (vetor[i] > maior) {
            maior = vetor[i];
        }
    }

    return maior;
}

int buscar_pares_soma(int vetor[], int tamanho, int alvo, Contadores *cnt) {
    int pares_encontrados = 0;

    for (int i = 0; i < tamanho - 1; i++) {
        for (int j = i + 1; j < tamanho; j++) {
            cnt->acessos += 2;
            cnt->comparacoes++;

            if (vetor[i] + vetor[j] == alvo) {
                pares_encontrados++;
            }
        }
    }

    return pares_encontrados;
}

int esta_ordenado(int vetor[], int tamanho) {
    for (int i = 0; i < tamanho - 1; i++) {
        if (vetor[i] > vetor[i + 1]) {
            return 0;
        }
    }
    return 1;
}

int busca_binaria(int vetor[], int tamanho, int chave, Contadores *cnt, int exibir_passos) {
    if (!esta_ordenado(vetor, tamanho)) {
        return -2;
    }

    int inicio = 0;
    int fim = tamanho - 1;

    if (exibir_passos) {
        printf("\n--- Rastreio da Busca Binaria (Chave: %d, N: %d) ---\n", chave, tamanho);
        printf("%-10s | %-10s | %-10s | %-10s\n", "Inicio", "Meio", "Fim", "Valor[Meio]");
        printf("---------------------------------------------------\n");
    }

    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;

        cnt->acessos++;
        cnt->comparacoes++;

        if (exibir_passos) {
            printf("%-10d | %-10d | %-10d | %-10d\n", inicio, meio, fim, vetor[meio]);
        }

        if (vetor[meio] == chave) {
            return meio;
        }

        cnt->comparacoes++;
        if (vetor[meio] < chave) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }

    return -1;
}

int busca_linear(int vetor[], int tamanho, int chave, Contadores *cnt) {
    for (int i = 0; i < tamanho; i++) {
        cnt->acessos++;
        cnt->comparacoes++;
        if (vetor[i] == chave) {
            return i;
        }
    }
    return -1;
}

int main() {
    int tamanhos[] = {100, 1000, 10000};
    int num_testes = sizeof(tamanhos) / sizeof(tamanhos[0]);

    printf("========================================================================================================================\n");
    printf("%-10s | %-15s | %-18s | %-22s | %-22s | %-20s\n", 
           "N", "Acesso Direto", "Busca Maior O(n)", "Busca Pares O(n^2)", "Busca Binaria (Presente)", "Busca Binaria (Ausente)");
    printf("           | O(1) (A/C)      | (A/C)              | (A/C)                  | O(log n) (A/C)         | O(log n) (A/C)\n");
    printf("========================================================================================================================\n");

    for (int t = 0; t < num_testes; t++) {
        int n = tamanhos[t];

        int *vetor = (int *)malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            vetor[i] = (i + 1) * 2;
        }

        Contadores cnt_direto;
        zerar_contadores(&cnt_direto);
        acesso_direto(vetor, n, n / 2, &cnt_direto);

        Contadores cnt_maior;
        zerar_contadores(&cnt_maior);
        buscar_maior(vetor, n, &cnt_maior);

        Contadores cnt_pares;
        zerar_contadores(&cnt_pares);
        buscar_pares_soma(vetor, n, n, &cnt_pares);

        Contadores cnt_binaria_pres;
        zerar_contadores(&cnt_binaria_pres);
        int chave_presente = vetor[n - 2];
        busca_binaria(vetor, n, chave_presente, &cnt_binaria_pres, 0);

        Contadores cnt_binaria_aus;
        zerar_contadores(&cnt_binaria_aus);
        int chave_ausente = -1;
        busca_binaria(vetor, n, chave_ausente, &cnt_binaria_aus, 0);

        printf("%-10d | A:%-3ld C:%-7ld | A:%-6ld C:%-9ld | A:%-10ld C:%-9ld | A:%-6ld C:%-13ld | A:%-6ld C:%-11ld\n", 
               n, 
               cnt_direto.acessos, cnt_direto.comparacoes,
               cnt_maior.acessos, cnt_maior.comparacoes,
               cnt_pares.acessos, cnt_pares.comparacoes,
               cnt_binaria_pres.acessos, cnt_binaria_pres.comparacoes,
               cnt_binaria_aus.acessos, cnt_binaria_aus.comparacoes);

        free(vetor);
    }

    printf("========================================================================================================================\n");

    int n_demonstracao = 16;
    int *vetor_demo = (int *)malloc(n_demonstracao * sizeof(int));
    for (int i = 0; i < n_demonstracao; i++) {
        vetor_demo[i] = (i + 1) * 5;
    }

    Contadores cnt_demo_pres;
    zerar_contadores(&cnt_demo_pres);
    busca_binaria(vetor_demo, n_demonstracao, 65, &cnt_demo_pres, 1);

    Contadores cnt_demo_aus;
    zerar_contadores(&cnt_demo_aus);
    busca_binaria(vetor_demo, n_demonstracao, 12, &cnt_demo_aus, 1);

    free(vetor_demo);

    printf("\n========================================================================================\n");
    printf("                    COMPARAÇÃO DIRETA: BUSCA LINEAR VS BUSCA BINÁRIA                     \n");
    printf("========================================================================================\n");

    int n_comp = 1000;
    int *vetor_ordenado = (int *)malloc(n_comp * sizeof(int));
    for (int i = 0; i < n_comp; i++) {
        vetor_ordenado[i] = (i + 1) * 2;
    }

    int posicoes_teste[4] = {0, n_comp / 2, n_comp - 1, -1};
    char *nomes_casos[4] = {"Inicio (2)", "Meio (1000)", "Fim (2000)", "Ausente (-1)"};

    printf("Vetor Ordenado de N = %d elementos:\n", n_comp);
    printf("%-15s | %-20s | %-20s | %-12s | %-12s\n", 
           "Cenário", "Busca Linear (A/C)", "Busca Binaria (A/C)", "Pos Linear", "Pos Binaria");
    printf("----------------------------------------------------------------------------------------\n");

    for (int i = 0; i < 4; i++) {
        int chave = (posicoes_teste[i] != -1) ? vetor_ordenado[posicoes_teste[i]] : -1;

        Contadores cnt_lin;
        zerar_contadores(&cnt_lin);
        int pos_lin = busca_linear(vetor_ordenado, n_comp, chave, &cnt_lin);

        Contadores cnt_bin;
        zerar_contadores(&cnt_bin);
        int pos_bin = busca_binaria(vetor_ordenado, n_comp, chave, &cnt_bin, 0);

        printf("%-15s | A:%-5ld C:%-11ld | A:%-5ld C:%-11ld | %-12d | %-12d\n",
               nomes_casos[i],
               cnt_lin.acessos, cnt_lin.comparacoes,
               cnt_bin.acessos, cnt_bin.comparacoes,
               pos_lin, pos_bin);
    }
    free(vetor_ordenado);

    printf("\n----------------------------------------------------------------------------------------\n");
    printf("DEMONSTRAÇÃO EM VETOR DESORDENADO (Apenas Busca Linear é Aplicável Com Segurança):\n");
    printf("----------------------------------------------------------------------------------------\n");

    int vetor_desordenado[] = {45, 12, 89, 3, 27, 99, 1, 54, 18, 72};
    int n_des = sizeof(vetor_desordenado) / sizeof(vetor_desordenado[0]);
    int chave_des = 27;

    Contadores cnt_lin_des;
    zerar_contadores(&cnt_lin_des);
    int pos_lin_des = busca_linear(vetor_desordenado, n_des, chave_des, &cnt_lin_des);

    Contadores cnt_bin_des;
    zerar_contadores(&cnt_bin_des);
    int pos_bin_des = busca_binaria(vetor_desordenado, n_des, chave_des, &cnt_bin_des, 0);

    printf("Chave procurada: %d no vetor {45, 12, 89, 3, 27, 99, 1, 54, 18, 72}\n", chave_des);
    printf("Resultados:\n");
    printf(" - Busca Linear : Retorno = Posição %d | Acessos = %ld | Comparações = %ld\n", 
           pos_lin_des, cnt_lin_des.acessos, cnt_lin_des.comparacoes);
    printf(" - Busca Binária: Retorno = %d (Recusada devido à falta de ordenação)\n", pos_bin_des);

    return 0;
}

/*
 ========================================================================================
                            PRÉ-CONDIÇÕES DOS ALGORITMOS DE BUSCA
 ========================================================================================
 1. BUSCA LINEAR - O(n):
    - Pré-condição: NENHUMA.
    - Requisito dos dados: Funciona em qualquer estrutura de dados sequencial, independente 
      de estar ordenada ou desordenada.
    - Mecanismo: Percorre o vetor elemento por elemento a partir do primeiro até encontrar 
      a chave ou alcançar o fim do vetor.
    - Análise de Custo: No melhor caso (início), realiza O(1) comparações. No pior caso (fim ou 
      ausente), realiza O(n) comparações.

 2. BUSCA BINÁRIA - O(log n):
    - Pré-condição: O VETOR DEVE ESTAR PREVIAMENTE ORDENADO.
    - Requisito dos dados: Acesso direto/aleatório rápido aos elementos através do índice.
    - Mecanismo: Aplica a estratégia de divisão e conquista, comparando a chave com o elemento 
      do meio e descartando metade do espaço de busca a cada iteração.
    - Análise de Custo: Se aplicada em vetores desordenados, a premissa de descartar metades 
      é violada, gerando retornos incorretos ou falsos negativos. Quando a pré-condição é 
      atendida, reduz dramaticamente o custo no pior caso para O(log n).
 ========================================================================================
*/