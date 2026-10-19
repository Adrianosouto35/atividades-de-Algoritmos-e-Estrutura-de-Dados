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

int main() {
    int tamanhos[] = {100, 1000, 10000};
    int num_testes = sizeof(tamanhos) / sizeof(tamanhos[0]);

    printf("========================================================================================\n");
    printf("%-10s | %-18s | %-20s | %-25s\n", 
           "N", "Acesso Direto O(1)", "Busca Maior O(n)", "Busca Pares O(n^2)");
    printf("           | (Acessos/Comparações) | (Acessos/Comparações)  | (Acessos/Comparações)\n");
    printf("========================================================================================\n");

    for (int t = 0; t < num_testes; t++) {
        int n = tamanhos[t];

        int *vetor = (int *)malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            vetor[i] = i + 1;
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

        printf("%-10d | A:%-3ld C:%-10ld | A:%-6ld C:%-11ld | A:%-11ld C:%-11ld\n", 
               n, 
               cnt_direto.acessos, cnt_direto.comparacoes,
               cnt_maior.acessos, cnt_maior.comparacoes,
               cnt_pares.acessos, cnt_pares.comparacoes);

        free(vetor);
    }

    printf("========================================================================================\n");

    return 0;
}