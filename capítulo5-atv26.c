#include <stdio.h>
#include <stdlib.h>

// Estrutura para manter os contadores independentes
typedef struct {
    long acessos;
    long comparacoes;
} Contadores;

// 1. Acesso Direto Instrumentado
int acesso_direto(int vetor[], int tamanho, int indice, Contadores *cnt) {
    cnt->comparacoes++; // Validação: 0 <= indice
    cnt->comparacoes++; // Validação: indice < tamanho
    
    if (indice >= 0 && indice < tamanho) {
        cnt->acessos++; // Acesso efetivo à posição
        return vetor[indice];
    } else {
        printf("Erro: Índice fora dos limites!\n");
        return -1;
    }
}

// 2. Busca do Maior Instrumentada
int buscar_maior(int vetor[], int tamanho, Contadores *cnt) {
    if (tamanho <= 0) return -1;

    cnt->acessos++; // Acesso para inicializar o maior com vetor[0]
    int maior = vetor[0];

    for (int i = 1; i < tamanho; i++) {
        cnt->acessos++;     // Acesso ao elemento vetor[i]
        cnt->comparacoes++; // Comparação entre vetor[i] e maior
        
        if (vetor[i] > maior) {
            maior = vetor[i];
        }
    }

    return maior;
}

int main() {
    int tamanhos[] = {100, 1000, 10000};
    int num_testes = sizeof(tamanhos) / sizeof(tamanhos[0]);

    printf("%-12s | %-22s | %-22s\n", "Tamanho (N)", "Acesso Direto (A/C)", "Busca Maior (A/C)");
    printf("---------------------------------------------------------------\n");

    for (int t = 0; t < num_testes; t++) {
        int n = tamanhos[t];

        // Alocação dinâmica do vetor de teste
        int *vetor = (int *)malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            vetor[i] = i; // Preenche o vetor
        }

        // Contadores independentes para Acesso Direto
        Contadores cnt_direto = {0, 0};
        acesso_direto(vetor, n, n / 2, &cnt_direto);

        // Contadores independentes para Busca do Maior
        Contadores cnt_maior = {0, 0};
        buscar_maior(vetor, n, &cnt_maior);

        printf("%-12d | A:%-4ld C:%-12ld | A:%-6ld C:%-10ld\n", 
               n, 
               cnt_direto.acessos, cnt_direto.comparacoes, 
               cnt_maior.acessos, cnt_maior.comparacoes);

        free(vetor); // Liberação de memória
    }

    return 0;
}