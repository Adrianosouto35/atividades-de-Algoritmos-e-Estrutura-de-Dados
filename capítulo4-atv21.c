
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int identificador;
    char jogador[50];
    int pontuacao;
} Partida;

int salvarPartidas(const Partida partidas[], int quantidade) {
    FILE *arquivo = fopen("partidas.txt", "w");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return 0;
    }

    for (int i = 0; i < quantidade; i++) {
        if (fprintf(arquivo, "%d;%s;%d\n",
                    partidas[i].identificador,
                    partidas[i].jogador,
                    partidas[i].pontuacao) < 0) {
            printf("Erro ao gravar os dados.\n");
            fclose(arquivo);
            return 0;
        }
    }

    if (fclose(arquivo) != 0) {
        printf("Erro ao fechar o arquivo de escrita.\n");
        return 0;
    }

    printf("Partidas gravadas com sucesso.\n");
    return 1;
}

void lerPartidas(void) {
    FILE *arquivo = fopen("partidas.txt", "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para leitura.\n");
        return;
    }

    Partida partida;
    int quantidade = 0;

    printf("\n=== PARTIDAS RECUPERADAS DO ARQUIVO ===\n");

    while (fscanf(arquivo, "%d;%49[^;];%d",
                  &partida.identificador,
                  partida.jogador,
                  &partida.pontuacao) == 3) {
        printf("ID: %d | Jogador: %s | Pontuacao: %d\n",
               partida.identificador,
               partida.jogador,
               partida.pontuacao);

        quantidade++;
    }

    if (quantidade == 0) {
        printf("Nenhuma partida encontrada no arquivo.\n");
    }

    printf("Total de partidas recuperadas: %d\n", quantidade);

    if (fclose(arquivo) != 0) {
        printf("Erro ao fechar o arquivo de leitura.\n");
    }
}

int main(void) {
    Partida partidas[] = {
        {1, "Adriano", 150},
        {2, "Carlos Silva", 230},
        {3, "Joao Pedro", 180}
    };

    int quantidade = sizeof(partidas) / sizeof(partidas[0]);

    if (salvarPartidas(partidas, quantidade)) {
        lerPartidas();
    }

    return 0;
}
