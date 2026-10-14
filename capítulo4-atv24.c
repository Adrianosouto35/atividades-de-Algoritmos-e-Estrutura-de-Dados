#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VALOR_BONUS 20

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

int validarEParserLinha(const char *buffer, Partida *partida) {
    char extra;
    int resultado = sscanf(buffer, "%d;%49[^;];%d",
                           &partida->identificador,
                           partida->jogador,
                           &partida->pontuacao);

    if (resultado == 3 && sscanf(buffer, "%d;%49[^;];%d %c",
                                &partida->identificador,
                                partida->jogador,
                                &partida->pontuacao,
                                &extra) != 4) {
        return 1;
    }
    return 0;
}

void listarHistorico(void) {
    FILE *arquivo = fopen("partidas.txt", "r");

    if (arquivo == NULL) {
        printf("Historico vazio ou arquivo inexistente.\n");
        return;
    }

    Partida partida;
    int registros = 0;
    int linha = 0;
    char buffer[256];

    printf("\n=== HISTORICO DE PARTIDAS ===\n");

    while (fgets(buffer, sizeof(buffer), arquivo) != NULL) {
        linha++;

        if (strchr(buffer, '\n') == NULL && !feof(arquivo)) {
            int caractere;
            while ((caractere = fgetc(arquivo)) != '\n' && caractere != EOF) {}
            printf("Registro malformado na linha %d.\n", linha);
            continue;
        }

        if (!validarEParserLinha(buffer, &partida)) {
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

void gerarRelatorioBonificado(void) {
    FILE *origem = fopen("partidas.txt", "r");
    if (origem == NULL) {
        printf("Erro: Arquivo de origem (partidas.txt) nao encontrado ou sem permissao.\n");
        return;
    }

    FILE *destino = fopen("relatorio_bonificado.txt", "w");
    if (destino == NULL) {
        printf("Erro: Nao foi possivel criar o arquivo de destino (relatorio_bonificado.txt).\n");
        fclose(origem);
        return;
    }

    Partida partida;
    int processados = 0;
    int linha = 0;
    char buffer[256];

    printf("\n=== PROCESSANDO E BONIFICANDO REGISTROS ===\n");

    while (fgets(buffer, sizeof(buffer), origem) != NULL) {
        linha++;

        if (strchr(buffer, '\n') == NULL && !feof(origem)) {
            int caractere;
            while ((caractere = fgetc(origem)) != '\n' && caractere != EOF) {}
            printf("Linha %d ignorada (excede tamanho maximo).\n", linha);
            continue;
        }

        if (validarEParserLinha(buffer, &partida)) {
            partida.pontuacao += VALOR_BONUS;

            if (fprintf(destino, "%d;%s;%d\n",
                        partida.identificador,
                        partida.jogador,
                        partida.pontuacao) > 0) {
                processados++;
            }
        } else {
            printf("Linha %d ignorada (registro malformado).\n", linha);
        }
    }

    printf("Processamento concluido!\n");
    printf("Total de registros gravados com bonus: %d\n", processados);

    fclose(origem);
    fclose(destino);
}

int salvarBinario(const Partida *partida) {
    FILE *arquivo = fopen("partidas.bin", "wb");

    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo binario para escrita.\n");
        return 0;
    }

    size_t gravados = fwrite(partida, sizeof(Partida), 1, arquivo);

    if (gravados != 1) {
        printf("Erro ao gravar estrutura no arquivo binario.\n");
        fclose(arquivo);
        return 0;
    }

    if (fclose(arquivo) != 0) {
        printf("Erro ao fechar arquivo binario de escrita.\n");
        return 0;
    }

    printf("Registro binario salvo com sucesso.\n");
    return 1;
}

void lerEExibirBinario(const Partida *original) {
    FILE *arquivo = fopen("partidas.bin", "rb");

    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo binario para leitura ou arquivo inexistente.\n");
        return;
    }

    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);

    if (tamanho == 0) {
        printf("Arquivo binario esta vazio.\n");
        fclose(arquivo);
        return;
    }

    if (tamanho % sizeof(Partida) != 0) {
        printf("Aviso: O arquivo contem registros incompletos ou corrompidos.\n");
    }

    fseek(arquivo, 0, SEEK_SET);

    Partida lida;
    size_t lidos = fread(&lida, sizeof(Partida), 1, arquivo);

    if (lidos != 1) {
        printf("Erro na leitura do registro binario (leitura incompleta).\n");
        fclose(arquivo);
        return;
    }

    printf("\n=== OBJETO RECONSTRUIDO DO BINARIO ===\n");
    printf("ID: %d | Jogador: %s | Pontuacao: %d\n",
           lida.identificador,
           lida.jogador,
           lida.pontuacao);

    if (original != NULL) {
        if (lida.identificador == original->identificador &&
            strcmp(lida.jogador, original->jogador) == 0 &&
            lida.pontuacao == original->pontuacao) {
            printf("Comparacao: O registro lido e IDENTICO ao original!\n");
        } else {
            printf("Comparacao: O registro lido e DIFERENTE do original!\n");
        }
    }

    fclose(arquivo);
}

int main(void) {
    int opcao;
    Partida ultimaPartidaSalvaBinario;
    int temPartidaBinario = 0;

    do {
        printf("\n=== GERENCIADOR DE PARTIDAS ===\n");
        printf("1 - Registrar partida em Texto\n");
        printf("2 - Listar historico Texto\n");
        printf("3 - Gerar relatorio bonificado\n");
        printf("4 - Salvar partida em Binario\n");
        printf("5 - Ler e exibir registro Binario\n");
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
        } else if (opcao == 3) {
            gerarRelatorioBonificado();
        } else if (opcao == 4) {
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

            if (salvarBinario(&partida)) {
                ultimaPartidaSalvaBinario = partida;
                temPartidaBinario = 1;
            }
        } else if (opcao == 5) {
            lerEExibirBinario(temPartidaBinario ? &ultimaPartidaSalvaBinario : NULL);
        } else if (opcao != 0) {
            printf("Opcao invalida.\n");
        }

    } while (opcao != 0);

    return 0;
}