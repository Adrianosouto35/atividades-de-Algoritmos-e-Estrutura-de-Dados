#include <stdio.h>


long long encontrarParesComSoma(int vet[], int tamanho, int alvo) {
    long long comparacoes = 0;

    printf("Pares cuja soma é igual a %d:\n", alvo);
    int paresEncontrados = 0;

    for (int i = 0; i < tamanho - 1; i++) {
        for (int j = i + 1; j < tamanho; j++) {
            comparacoes++; // Contador do núcleo do algoritmo

            if (vet[i] + vet[j] == alvo) {
                printf("  -> Índices (%d, %d) | Valores [%d + %d = %d]\n", 
                       i, j, vet[i], vet[j], alvo);
                paresEncontrados++;
            }
        }
    }

    if (paresEncontrados == 0) {
        printf("  -> Nenhum par encontrado.\n");
    }

    return comparacoes;
}

int main() {
    // Teste de funcionalidade individual
    int vetorExemplo[] = {2, 7, 11, 15, 3, 6, 8, 1};
    int tamanhoExemplo = sizeof(vetorExemplo) / sizeof(vetorExemplo[0]);
    int alvoExemplo = 9;

    printf("=== TESTE DE FUNCIONALIDADE ===\n");
    long long compExemplo = encontrarParesComSoma(vetorExemplo, tamanhoExemplo, alvoExemplo);
    printf("Comparações realizadas para N = %d: %lld\n\n", tamanhoExemplo, compExemplo);

    // Teste com tamanhos crescentes de entrada
    printf("=== ANÁLISE DE DESEMPENHO E COMPORTAMENTO O(n²) ===\n");
    int tamanhosTestes[] = {10, 100, 1000, 10000};
    int numTestes = sizeof(tamanhosTestes) / sizeof(tamanhosTestes[0]);

    printf("%-10s | %-20s | %-25s\n", "Tamanho (N)", "Comparações Exatas", "Fórmula [N*(N-1)/2]");
    printf("--------------------------------------------------------------\n");

    for (int k = 0; k < numTestes; k++) {
        int n = tamanhosTestes[k];
        
        // Criando um vetor dummy de tamanho N
        int vet[n];
        for (int i = 0; i < n; i++) {
            vet[i] = i + 1;
        }

        // Medição do número de comparações
        long long comparacoes = 0;
        for (int i = 0; i < n - 1; i++) {
            for (int j = i + 1; j < n; j++) {
                comparacoes++;
            }
        }

        long long teoretico = (long long)n * (n - 1) / 2;
        printf("%-10d | %-20lld | %-25lld\n", n, comparacoes, teoretico);
    }

    return 0;
}