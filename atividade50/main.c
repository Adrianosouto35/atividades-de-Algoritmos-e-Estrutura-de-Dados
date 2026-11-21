#include <stdio.h>
#include "missao.h"

#define ARQUIVO "agenda.txt"

// Comprova a integridade: mostra a lista e confere a quantidade esperada
static int verificar(const Agenda* agenda, int esperado) {
    exibirAgenda(agenda);
    int total = contarMissoes(agenda);
    if (total == esperado) {
        printf("[Integridade OK] %d missao(oes) na agenda.\n", total);
        return 1;
    }
    printf("[ERRO] Esperado %d, encontrado %d.\n", esperado, total);
    return 0;
}

static void salvar(const Agenda* agenda, const char* caminho) {
    int r = salvarAgenda(agenda, caminho);
    if (r == 1) {
        printf("[Sucesso] Agenda salva em '%s'.\n", caminho);
    } else if (r == 0) {
        printf("[Aviso] Agenda ou caminho nulo. Nada salvo.\n");
    } else if (r == -1) {
        printf("[Erro] Nao foi possivel abrir '%s' para escrita.\n", caminho);
    } else {
        printf("[Erro] Falha ao gravar em '%s'.\n", caminho);
    }
}

// Le o arquivo salvo e comprova: quantidade de linhas e ordem (prioridade, id)
static int conferirArquivo(const char* caminho, int esperado, int ordenado) {
    FILE* arq = fopen(caminho, "r");
    if (arq == NULL) {
        printf("[ERRO] Nao foi possivel reabrir '%s'.\n", caminho);
        return 0;
    }

    char linha[200];
    int total = 0, ok = 1;
    int idAnt = 0, prioAnt = 0;

    while (fgets(linha, sizeof(linha), arq) != NULL) {
        int id, prio;
        char desc[TAM_DESCRICAO];
        if (sscanf(linha, "%d;%d;%99[^\n]", &id, &prio, desc) != 3) {
            ok = 0;
            break;
        }
        if (ordenado && total > 0 &&
            (prio < prioAnt || (prio == prioAnt && id < idAnt))) {
            ok = 0;
        }
        idAnt = id;
        prioAnt = prio;
        total++;
    }
    fclose(arq);

    if (ok && total == esperado) {
        printf("[Arquivo OK] %d linha(s) em '%s'%s.\n", total, caminho,
               ordenado ? ", ordem (prioridade, id) confirmada" : "");
        return 1;
    }
    printf("[ERRO] Arquivo inconsistente: %d linha(s), esperado %d.\n", total, esperado);
    return 0;
}

int main(void) {
    printf("=== SISTEMA DE AGENDA DE MISSOES (ATIVIDADE 50) ===\n");

    Agenda* agenda = criarAgenda();
    if (agenda == NULL) {
        return 1;
    }

    // 1. Ordenar lista vazia e lista com um unico no
    printf("\n--- Teste 1: Lista Vazia e Unitaria ---\n");
    ordenarAgenda(agenda);
    verificar(agenda, 0);
    salvar(agenda, ARQUIVO);
    conferirArquivo(ARQUIVO, 0, 1);

    inserirFim(agenda, criarMissao(10, "Missao unica", 3));
    ordenarAgenda(agenda);
    verificar(agenda, 1);
    limparAgenda(agenda);

    // 2. Lista desordenada com empates de prioridade
    printf("\n--- Teste 2: Ordenacao com Desempate ---\n");
    Missao lote[] = {
        criarMissao(105, "Atualizar manual de bordo", 5),
        criarMissao(103, "Enviar relatorio de telemetria", 3),
        criarMissao(108, "Calibrar giroscopio", 1),
        criarMissao(102, "Verificar integridade do casco", 3),
        criarMissao(101, "Estabilizar pressao interna", 1),
        criarMissao(104, "Arquivar logs de voo", 4),
        criarMissao(107, "Revisar painel de controle", 3)
    };
    inserirLote(agenda, lote, 7);
    printf("Antes da ordenacao:");
    verificar(agenda, 7);

    ordenarAgenda(agenda);
    printf("\nDepois da ordenacao (prioridade, depois id):");
    verificar(agenda, 7); // todos os 7 registros preservados

    // 3. Ordenar de novo uma lista ja ordenada nao muda nada
    printf("\n--- Teste 3: Reordenar Lista Ja Ordenada ---\n");
    ordenarAgenda(agenda);
    verificar(agenda, 7);

    // 4. Salvar e conferir o arquivo
    printf("\n--- Teste 4: Salvar em Arquivo ---\n");
    salvar(agenda, ARQUIVO);
    conferirArquivo(ARQUIVO, 7, 1);

    // 5. Falhas de abertura e de escrita
    printf("\n--- Teste 5: Falhas ---\n");
    salvar(agenda, "pasta_inexistente/agenda.txt"); // falha ao abrir
    salvar(agenda, NULL);                           // caminho nulo
    salvar(NULL, ARQUIVO);                          // agenda nula
    salvar(agenda, "/dev/full");                    // falha de escrita (Linux)
    conferirArquivo(ARQUIVO, 7, 1);                 // arquivo anterior segue intacto

    // 6. Agenda nula
    printf("\n--- Teste 6: Agenda Nula ---\n");
    if (ordenarAgenda(NULL) == 0) {
        printf("[Protecao OK] Ordenacao em agenda nula rejeitada.\n");
    }

    destruirAgenda(agenda);
    printf("\nAgenda encerrada e memoria liberada com sucesso.\n");

    return 0;
}
