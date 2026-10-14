#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VALOR_BONUS 20 // Regra de bonificacao da Atividade 23

typedef struct {
    int identificador;
    char jogador[50];
    int pontuacao;
} Partida;

// ============================================================================
// ATIVIDADE 21 & 22: Escrita em arquivo (Modo Append)
// ============================================================================
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

// Auxiliar para validar registros malformados (reutilizado nas Atividades 22 e 23)
int validarEParserLinha(const char *buffer, Partida *partida) {
    char extra;
    int resultado = sscanf(buffer, "%d;%49[^;];%d",
                           &partida->identificador,
                           partida->jogador,
                           &partida->pontuacao);

    // Deve ler exatamente 3 campos e NAO ter caracteres extras no final da linha
    if (resultado == 3 && sscanf(buffer, "%d;%49[^;];%d %c",
                                &partida->identificador,
                                partida->jogador,
                                &partida->pontuacao,
                                &extra) != 4) {
        return 1; // Registro valido
    }
    return 0; // Registro malformado
}

// ============================================================================
// ATIVIDADE 22: Leitura e listagem do historico
// ============================================================================
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

// ============================================================================
// ATIVIDADE 23: Transformação entre arquivos e Bonificação
// ============================================================================
void gerarRelatorioBonificado(void) {
    // Requisito 1 & 2: Verificar retorno de fopen com mensagens especificas
    FILE *origem = fopen("partidas.txt", "r");
    if (origem == NULL) {
        printf("Erro: Arquivo de origem (partidas.txt) nao encontrado ou sem permissao.\n");
        return;
    }

    FILE *destino = fopen("relatorio_bonificado.txt", "w");
    if (destino == NULL) {
        printf("Erro: Nao foi possivel criar o arquivo de destino (relatorio_bonificado.txt).\n");
        // Requisito 5: Fechar origem se o destino falhar
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

        // Tratamento de linha que excede o buffer
        if (strchr(buffer, '\n') == NULL && !feof(origem)) {
            int caractere;
            while ((caractere = fgetc(origem)) != '\n' && caractere != EOF) {}
            printf("Linha %d ignorada (excede tamanho maximo).\n", linha);
            continue;
        }

        // Requisito 3: Processar apenas quando a leitura for completa
        if (validarEParserLinha(buffer, &partida)) {
            // Aplicacao da regra de bonificacao
            partida.pontuacao += VALOR_BONUS;

            // Requisito 4: Escrever no arquivo destino
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

    // Requisito 6: Apresentar a quantidade de registros processados
    printf("Processamento concluido!\n");
    printf("Total de registros gravados com bonus: %d\n", processados);

    // Requisito 5: Fechar ambos os arquivos no caminho de sucesso
    fclose(origem);
    fclose(destino);
}

// ============================================================================
// MENU PRINCIPAL (Integrando todas as Atividades)
// ============================================================================
int main(void) {
    int opcao;

    do {
        printf("\n=== GERENCIADOR DE PARTIDAS ===\n");
        printf("1 - Registrar partida (Atv 21/22)\n");
        printf("2 - Listar historico (Atv 22)\n");
        printf("3 - Gerar relatorio bonificado (Atv 23)\n");
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
        } else if (opcao != 0) {
            printf("Opcao invalida.\n");
        }

    } while (opcao != 0);

    return 0;
}