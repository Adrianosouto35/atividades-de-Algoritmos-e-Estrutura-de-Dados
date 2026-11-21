#include <stdio.h>
#include "missao.h"

int main(void) {
    printf("=== SISTEMA DE AGENDA DE MISSOES (ATIVIDADE 45) ===\n");

    Agenda* agenda = criarAgenda();
    if (agenda == NULL) {
        return 1;
    }

    Missao concluida;

    // 1. Tentativa de remocao em lista vazia
    printf("\n--- Teste 1: Lista Vazia ---\n");
    if (!removerInicio(agenda, &concluida)) {
        printf("[Protecao OK] Impossivel remover: a agenda esta vazia.\n");
    }
    if (!inserirAntesDe(agenda, 102, criarMissao(100, "Tarefa orfa", 1))) {
        printf("[Protecao OK] Impossivel inserir antes de #102: a agenda esta vazia.\n");
    }

    // 2. Lista com APENAS UM NO
    printf("\n--- Teste 2: Lista com Um Unico No ---\n");
    inserirInicio(agenda, criarMissao(102, "Verificar integridade do casco", 2));
    exibirAgenda(agenda);

    // Inserindo antes do unico no (altera o inicio)
    if (inserirAntesDe(agenda, 102, criarMissao(100, "URGENTE: Estabilizar pressao interna", 1))) {
        printf("[Sucesso] Missao #100 inserida antes do primeiro no (#102).\n");
    }
    exibirAgenda(agenda); // Esperado: 100 -> 102

    // 3. VARIOS NOS (insercao no meio e referencia inexistente)
    printf("\n--- Teste 3: Lista com Varios Nos ---\n");
    inserirFim(agenda, criarMissao(103, "Enviar relatorio de telemetria", 3)); // 100 -> 102 -> 103

    if (inserirAntesDe(agenda, 102, criarMissao(101, "URGENTE: Calibrar giroscopio", 1))) {
        printf("[Sucesso] Missao #101 inserida antes da #102.\n");
    }

    if (!inserirAntesDe(agenda, 999, criarMissao(104, "Tarefa descartada", 2))) {
        printf("[Aviso] Referencia #999 inexistente. Lista nao modificada.\n");
    }
    exibirAgenda(agenda); // Esperado: 100 -> 101 -> 102 -> 103

    // 4. Remocao do primeiro no ate esvaziar a lista
    printf("\n--- Teste 4: Remocao do Primeiro No ---\n");
    while (removerInicio(agenda, &concluida)) {
        printf("[Concluida e Removida] ");
        exibirMissao(&concluida);
    }

    // Confirma que a agenda ficou vazia
    exibirAgenda(agenda);

    // 5. Agenda nula
    printf("\n--- Teste 5: Agenda Nula ---\n");
    if (!removerInicio(NULL, &concluida) &&
        !inserirAntesDe(NULL, 100, criarMissao(1, "Invalida", 1))) {
        printf("[Protecao OK] Operacoes em agenda nula rejeitadas.\n");
    }

    destruirAgenda(agenda);
    printf("\nAgenda encerrada e memoria liberada com sucesso.\n");

    return 0;
}
