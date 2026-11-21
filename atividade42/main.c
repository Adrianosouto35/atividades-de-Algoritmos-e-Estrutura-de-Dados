#include <stdio.h>
#include "missao.h"

int main(void) {
    printf("=== SISTEMA DE AGENDA DE MISSOES (ATIVIDADE 42) ===\n");

    Agenda* agenda = criarAgenda();
    if (agenda == NULL) {
        return 1;
    }

    // Agenda recem-criada deve estar vazia
    exibirAgenda(agenda);

    // Cada insercao no inicio passa a missao para o primeiro lugar
    inserirInicio(agenda, criarMissao(103, "Enviar relatorio de telemetria", 3));
    exibirAgenda(agenda);

    inserirInicio(agenda, criarMissao(102, "Calibrar giroscopio", 1));
    inserirInicio(agenda, criarMissao(101, "Verificar integridade do casco", 2));
    exibirAgenda(agenda); // Esperado: 101 -> 102 -> 103

    // Protecao contra agenda nula
    if (!inserirInicio(NULL, criarMissao(999, "Invalida", 1))) {
        printf("[Protecao OK] Insercao em agenda nula rejeitada.\n");
    }

    destruirAgenda(agenda);
    printf("\nAgenda encerrada e memoria liberada com sucesso.\n");

    return 0;
}
