#include <stdio.h>
#include "missao.h"

int main(void) {
    printf("=== SISTEMA DE AGENDA DE MISSOES (ATIVIDADE 41) ===\n\n");

    Missao m1 = criarMissao(101, "Verificar integridade do casco", 2);
    Missao m2 = criarMissao(102, "Calibrar giroscopio", 1);
    Missao m3 = criarMissao(103, "Enviar relatorio de telemetria", 3);

    exibirMissao(&m1);
    exibirMissao(&m2);
    exibirMissao(&m3);

    // Protecao contra ponteiro nulo
    exibirMissao(NULL);

    return 0;
}
