#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int identificador;
    char jogador[50];
    int pontuacao;
} Partida;

int salvarPartidas(const Partida partidas[], int quantidade) {
    FILE *arquivo = fopen("partidas.txt", "a");

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

void listarHistorico(void) {
    FILE *arquivo = fopen("partidas.txt", "r");

    if (arquivo == NULL) {
        printf("Historico vazio ou arquivo inexistente.\n");
        return;
    }

    Partida partida;
    int registros = 0;
    int resultado;
    int linha = 0;
    char buffer[256];

    printf("\n=== HISTORICO DE PARTIDAS ===\n");

    while (fgets(buffer, sizeof(buffer), arquivo) != NULL) {
        linha++;

        if (strchr(buffer, '\n') == NULL && !feof(arquivo)) {
            int caractere;
            while ((caractere = fgetc(arquivo)) != '\n' && caractere != EOF) {
            }

            printf("Registro malformado na linha %d.\n", linha);
            continue;
        }

        resultado = sscanf(buffer, "%d;%49[^;];%d",
                           &partida.identificador,
                           partida.jogador,
                           &partida.pontuacao);

        char extra;
        if (resultado != 3 ||
            sscanf(buffer, "%d;%49[^;];%d %c",
                   &partida.identificador,
                   partida.jogador,
                   &partida.pontuacao,
                   &extra) == 4) {
            printf("Registro malformado na linha %d.\n", linha);
            continue;
        }

        printf("ID: %d | Jogador: %s | Pontuacao: %d\n",
               partida.identificador,
               partida.jogador,
               partida.pontuacao);

        registros++;
    }

    if (ferror(arquivo)) {
        printf("Erro durante a leitura do historico.\n");
    }

    if (registros == 0) {
        printf("Historico vazio: nenhum registro valido encontrado.\n");
    }

    printf("Total de registros validos: %d\n", registros);

    fclose(arquivo);
}

int main(void) {
    int opcao;

    do {
        printf("\n=== GERENCIADOR DE PARTIDAS ===\n");
        printf("1 - Registrar partida\n");
        printf("2 - Listar historico\n");
        printf("0 - Sair\n");
        printf("Opcao: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Opcao invalida.\n");
            return 1;
        }

        if (opcao == 1) {
            Partida partida;

            printf("Identificador: ");
            if (scanf("%d", &partida.identificador) != 1) {
                printf("Identificador invalido.\n");
                return 1;
            }

            printf("Nome do jogador (sem espacos): ");
            if (scanf("%49s", partida.jogador) != 1) {
                printf("Nome invalido.\n");
                return 1;
            }

            printf("Pontuacao: ");
            if (scanf("%d", &partida.pontuacao) != 1) {
                printf("Pontuacao invalida.\n");
                return 1;
            }

            salvarPartidas(&partida, 1);
        } else if (opcao == 2) {
            listarHistorico();
        } else if (opcao != 0) {
            printf("Opcao invalida.\n");
        }

    } while (opcao != 0);

    return 0;
}