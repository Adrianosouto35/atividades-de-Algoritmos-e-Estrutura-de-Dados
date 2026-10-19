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
        printf("\n--- Rastreio da Busca Binaria Iterativa (Chave: %d, N: %d) ---\n", chave, tamanho);
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

long fatorial_recursivo(int n, int *chamadas) {
    (*chamadas)++;
    if (n <= 1) {
        return 1;
    }
    return n * fatorial_recursivo(n - 1, chamadas);
}

long fatorial_iterativo(int n, int *operacoes) {
    long resultado = 1;
    for (int i = 2; i <= n; i++) {
        (*operacoes)++;
        resultado *= i;
    }
    return resultado;
}

long soma_vetor_recursiva(int vetor[], int tamanho, int indice, Contadores *cnt, int *chamadas) {
    (*chamadas)++;
    
    cnt->comparacoes++;
    if (indice >= tamanho) {
        return 0;
    }
    
    cnt->acessos++;
    return vetor[indice] + soma_vetor_recursiva(vetor, tamanho, indice + 1, cnt, chamadas);
}

long soma_vetor_iterativa(int vetor[], int tamanho, Contadores *cnt) {
    long soma = 0;
    for (int i = 0; i < tamanho; i++) {
        cnt->acessos++;
        cnt->comparacoes++;
        soma += vetor[i];
    }
    return soma;
}

long fibonacci_ingenuo(int n, int *chamadas) {
    (*chamadas)++;
    if (n <= 0) return 0;
    if (n == 1) return 1;
    return fibonacci_ingenuo(n - 1, chamadas) + fibonacci_ingenuo(n - 2, chamadas);
}

int busca_binaria_recursiva(int vetor[], int inicio, int fim, int chave, Contadores *cnt, int *chamadas) {
    (*chamadas)++;
    
    cnt->comparacoes++;
    if (inicio > fim) {
        return -1;
    }

    int meio = inicio + (fim - inicio) / 2;

    cnt->acessos++;
    cnt->comparacoes++;
    if (vetor[meio] == chave) {
        return meio;
    }

    cnt->comparacoes++;
    if (vetor[meio] > chave) {
        return busca_binaria_recursiva(vetor, inicio, meio - 1, chave, cnt, chamadas);
    } else {
        return busca_binaria_recursiva(vetor, meio + 1, fim, chave, cnt, chamadas);
    }
}

void executar_atv1_2() {
    int tamanhos[] = {100, 1000, 10000};
    int num_testes = sizeof(tamanhos) / sizeof(tamanhos[0]);

    printf("\n========================================================================================================================\n");
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
}

void executar_atv3() {
    printf("\n========================================================================================\n");
    printf("                    COMPARACAO DIRETA: BUSCA LINEAR VS BUSCA BINARIA                     \n");
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
           "Cenario", "Busca Linear (A/C)", "Busca Binaria (A/C)", "Pos Linear", "Pos Binaria");
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
    printf("DEMONSTRACAO EM VETOR DESORDENADO (Apenas Busca Linear e Aplicavel Com Seguranca):\n");
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
    printf(" - Busca Linear : Retorno = Posicao %d | Acessos = %ld | Comparacoes = %ld\n", 
           pos_lin_des, cnt_lin_des.acessos, cnt_lin_des.comparacoes);
    printf(" - Busca Binaria: Retorno = %d (Recusada devido a falta de ordenacao)\n", pos_bin_des);
}

void executar_atv4() {
    printf("\n========================================================================================\n");
    printf("                        DEMONSTRACAO DE RECURSAO VS ITERACAO                             \n");
    printf("========================================================================================\n");

    int n_fat = 10;
    int chamadas_fat = 0;
    int ops_fat_iter = 0;
    long res_fat_rec = fatorial_recursivo(n_fat, &chamadas_fat);
    long res_fat_iter = fatorial_iterativo(n_fat, &ops_fat_iter);

    printf("1. Fatorial Numerico (N = %d):\n", n_fat);
    printf("   - Recursivo: Resultado = %ld | Chamadas de Funcao (Profundidade) = %d\n", res_fat_rec, chamadas_fat);
    printf("   - Iterativo: Resultado = %ld | Iteracoes do Laco = %d\n", res_fat_iter, ops_fat_iter);

    int n_soma = 100;
    int *vetor_soma = (int *)malloc(n_soma * sizeof(int));
    for (int i = 0; i < n_soma; i++) {
        vetor_soma[i] = i + 1;
    }

    Contadores cnt_soma_rec;
    zerar_contadores(&cnt_soma_rec);
    int chamadas_soma = 0;
    long res_soma_rec = soma_vetor_recursiva(vetor_soma, n_soma, 0, &cnt_soma_rec, &chamadas_soma);

    Contadores cnt_soma_iter;
    zerar_contadores(&cnt_soma_iter);
    long res_soma_iter = soma_vetor_iterativa(vetor_soma, n_soma, &cnt_soma_iter);

    printf("\n2. Soma em Vetor (N = %d elementos):\n", n_soma);
    printf("   - Recursiva: Resultado = %ld | Chamadas = %d | Acessos = %ld | Comparacoes = %ld\n", 
           res_soma_rec, chamadas_soma, cnt_soma_rec.acessos, cnt_soma_rec.comparacoes);
    printf("   - Iterativa: Resultado = %ld | Iteracoes = %d | Acessos = %ld | Comparacoes = %ld\n", 
           res_soma_iter, n_soma, cnt_soma_iter.acessos, cnt_soma_iter.comparacoes);

    free(vetor_soma);
}

void executar_rastreio_binaria_iterativa() {
    int vetor[] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32};
    int tamanho = sizeof(vetor) / sizeof(vetor[0]);
    int chave;

    printf("\n============================================================\n");
    printf("       RASTREIO DA BUSCA BINARIA ITERATIVA\n");
    printf("============================================================\n");
    printf("Vetor ordenado: {2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32}\n");
    printf("Digite o valor que deseja procurar: ");

    if (scanf("%d", &chave) != 1) {
        int caractere;
        while ((caractere = getchar()) != '\n' && caractere != EOF) {}
        printf("Entrada invalida. Informe um numero inteiro.\n");
        return;
    }

    Contadores cnt;
    zerar_contadores(&cnt);
    int posicao = busca_binaria(vetor, tamanho, chave, &cnt, 1);

    if (posicao >= 0) {
        printf("\nValor encontrado na posicao %d.\n", posicao);
    } else {
        printf("\nValor nao encontrado; retorno = %d.\n", posicao);
    }
    printf("Acessos: %ld | Comparacoes: %ld\n", cnt.acessos, cnt.comparacoes);
}

void executar_atv5() {
    printf("\n========================================================================================\n");
    printf("              RECURSAO RAMIFICADA (FIBONACCI) E BUSCA BINARIA RECURSIVA                  \n");
    printf("========================================================================================\n");

    int n_fib;
    do {
        printf("Digite N para Fibonacci (0 a 30): ");
        if (scanf("%d", &n_fib) != 1) {
            int caractere;
            while ((caractere = getchar()) != '\n' && caractere != EOF) {}
            n_fib = -1;
            printf("Entrada invalida. Digite um numero inteiro entre 0 e 30.\n");
            continue;
        }
        if (n_fib < 0 || n_fib > 30) {
            printf("Valor fora do limite. Escolha um numero entre 0 e 30.\n");
        }
    } while (n_fib < 0 || n_fib > 30);

    int chamadas_fib = 0;
    long res_fib = fibonacci_ingenuo(n_fib, &chamadas_fib);

    printf("1. Fibonacci Ingenuo Ramificado (N = %d, limite maximo seguro N <= 30):\n", n_fib);
    printf("   - Resultado: Fib(%d) = %ld\n", n_fib, res_fib);
    printf("   - Total de Chamadas Recursivas: %d (Crescimento Exponencial O(2^n))\n", chamadas_fib);

    int n_bin_rec = 1000;
    int *vetor_bin = (int *)malloc(n_bin_rec * sizeof(int));
    for (int i = 0; i < n_bin_rec; i++) {
        vetor_bin[i] = (i + 1) * 2;
    }

    if (!esta_ordenado(vetor_bin, n_bin_rec)) {
        printf("Erro: Vetor desordenado. A busca binaria exige vetor ordenado.\n");
        free(vetor_bin);
        return;
    }

    int chave_pres = vetor_bin[n_bin_rec - 2];
    int chave_aus = -1;

    Contadores cnt_pres, cnt_aus;
    zerar_contadores(&cnt_pres);
    zerar_contadores(&cnt_aus);

    int chamadas_pres = 0, chamadas_aus = 0;

    int pos_pres = busca_binaria_recursiva(vetor_bin, 0, n_bin_rec - 1, chave_pres, &cnt_pres, &chamadas_pres);
    int pos_aus = busca_binaria_recursiva(vetor_bin, 0, n_bin_rec - 1, chave_aus, &cnt_aus, &chamadas_aus);

    printf("\n2. Busca Binaria Recursiva (Vetor Ordenado N = %d):\n", n_bin_rec);
    printf("   - Chave Presente (%d): Pos = %d | Chamadas = %d | Acessos = %ld | Comparacoes = %ld\n",
           chave_pres, pos_pres, chamadas_pres, cnt_pres.acessos, cnt_pres.comparacoes);
    printf("   - Chave Ausente  (%d): Pos = %d | Chamadas = %d | Acessos = %ld | Comparacoes = %ld\n",
           chave_aus, pos_aus, chamadas_aus, cnt_aus.acessos, cnt_aus.comparacoes);

    free(vetor_bin);
}

int main() {
    int opcao;

    do {
        printf("\n========================================================================================\n");
        printf("                         MENU DE EXPERIMENTOS - CAPITULO 5                              \n");
        printf("========================================================================================\n");
        printf("1. Atividades 1 e 2: Medicao O(1), O(n), O(n^2) e Busca Binaria Iterativa O(log n)\n");
        printf("2. Atividade 3: Comparacao Direta (Busca Linear vs Busca Binaria e Pre-condicoes)\n");
        printf("3. Atividade 4: Introducao a Recursao vs Iteracao (Fatorial e Soma de Vetor)\n");
        printf("4. Atividade 5: Fibonacci Ramificado O(2^n) e Busca Binaria Recursiva O(log n)\n");
        printf("5. Rastrear Busca Binaria Iterativa (inicio, meio e fim)\n");
        printf("6. Executar Todos os Experimentos em Sequencia\n");
        printf("0. Sair\n");
        printf("========================================================================================\n");
        printf("Escolha uma opcao: ");
        
        if (scanf("%d", &opcao) != 1) {
            break;
        }

        switch (opcao) {
            case 1:
                executar_atv1_2();
                break;
            case 2:
                executar_atv3();
                break;
            case 3:
                executar_atv4();
                break;
            case 4:
                executar_atv5();
                break;
            case 5:
                executar_rastreio_binaria_iterativa();
                break;
            case 6:
                executar_atv1_2();
                executar_atv3();
                executar_atv4();
                executar_atv5();
                executar_rastreio_binaria_iterativa();
                break;
            case 0:
                printf("\nEncerrando o programa do Capitulo 5...\n");
                break;
            default:
                printf("\nOpcao invalida! Tente novamente.\n");
        }
    } while (opcao != 0);

    return 0;
}

/*
 ========================================================================================
                            PRE-CONDICOES DOS ALGORITMOS DE BUSCA
 ========================================================================================
 1. BUSCA LINEAR - O(n):
    - Pre-condicao: NENHUMA.
    - Requisito dos dados: Funciona em qualquer estrutura de dados sequencial, independente 
      de estar ordenada ou desordenada.
    - Mecanismo: Percorre o vetor elemento por elemento a partir do primeiro ate encontrar 
      a chave ou alcancar o fim do vetor.
    - Analise de Custo: No melhor caso (inicio), realiza O(1) comparacoes. No pior caso (fim ou 
      ausente), realiza O(n) comparacoes.

 2. BUSCA BINARIA - O(log n):
    - Pre-condicao: O VETOR DEVE ESTAR PREVIAMENTE ORDENADO.
    - Requisito dos dados: Acesso direto/aleatorio rapido aos elementos atraves do indice.
    - Mecanismo: Aplica a estrategia de divisao e conquista, comparando a chave com o elemento 
      do meio e descartando metade do espaco de busca a cada iteracao.
    - Analise de Custo: Se aplicada em vetores desordenados, a premissa de descartar metades 
      e violada, gerando retornos incorretos ou falsos negativos. Quando a pre-condicao e 
      atendida, reduz dramaticamente o custo no pior caso para O(log n).

 ========================================================================================
                        COMPARACAO ENTRE RECURSAO E ITERACAO
 ========================================================================================
 1. FATORIAL NUMERICO:
    - Caso-base: Alcancado quando n <= 1, interrompendo as chamadas e retornando 1.
    - Progresso: Cada chamada reduz o parametro para 'n - 1', aproximando-se do caso-base.
    - Custo: A versao recursiva gera N chamadas empilhadas no Call Stack (Stack Overflow para N grande), 
      enquanto a versao iterativa executa um simples laco reutilizando as mesmas variaveis.

 2. SOMA DE VETOR:
    - Caso-base: Alcancado quando o indice chega em 'tamanho' (posicao invalida/alem do vetor), 
      retornando 0 e garantindo a ausencia de acessos fora dos limites do vetor.
    - Progresso: O indice incrementa (+1) a cada etapa, processando elemento por elemento.
    - Custo e Resultados: Ambos produzem o exato mesmo resultado correto de soma e realizam o 
      mesmo numero de acessos e comparacoes operacionais. Contudo, a recursao consome memoria O(n) 
      no Stack para manter os frames de chamada, enquanto a iteracao utiliza memoria O(1).

 ========================================================================================
                 PAPEL DOS CASOS-BASE E RECURSAO RAMIFICADA VS DIVISAO E CONQUISTA
 ========================================================================================
 1. PAPEL DOS CASOS-BASE NAS FUNCOES RECURSIVAS:
    - O caso-base e a condicao de parada indispensavel para qualquer algoritmo recursivo.
    - Ele impede que a funcao continue se chamando infinitamente, evitando o estouro de pilha 
      (Stack Overflow).
    - No Fibonacci: Os casos-base sao (n <= 0) retornando 0 e (n == 1) retornando 1.
    - Na Busca Binaria Recursiva: O caso-base principal de falha ocorre quando (inicio > fim), 
      indicando que o intervalo de busca esvaziou e o elemento nao esta presente (retorna -1). 
      O caso-base de sucesso ocorre quando vetor[meio] == chave (retorna a posicao meio).

 2. FIBONACCI INGENUO (RECURSAO RAMIFICADA):
    - Mecanismo: Cada chamada se ramifica em duas novas chamadas: fib(n-1) + fib(n-2).
    - Custo de Chamadas: A arvore de recursao gera um numero exponencial de chamadas O(2^n).
    - Entrada Limitada: E indispensavel limitar o valor de N (ex.: N <= 30) para evitar o 
      travamento da execucao, pois Fib(40) exige mais de 300 milhoes de chamadas recursivas.

 3. BUSCA BINARIA RECURSIVA (DIVISAO E CONQUISTA):
    - Mecanismo: Passa os limites 'inicio' e 'fim' como parametros. Em cada chamada, calcula 
      o ponto medio e realiza APENAS UMA nova chamada recursiva para metade do vetor (esquerda ou direita).
    - Custo de Chamadas: Por dividir o problema pela metade sem ramificacao dupla, realiza 
      no maximo O(log n) chamadas recursivas. Para N = 1000, exige cerca de 10 chamadas no pior caso.
 ========================================================================================
*/