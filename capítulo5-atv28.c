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

    return 0;
}