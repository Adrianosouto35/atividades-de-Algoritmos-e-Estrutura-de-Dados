#include <stdio.h>
#include "missao.h"

int main(void) {
    printf("=== SISTEMA DE AGENDA DE MISSOES (ATIVIDADE 43) ===\n");

    Agenda* agenda = criarAgenda();
    if (agenda == NULL) {
        return 1;
    }

    // 1. Lista vazia: exibicao informa que nao ha missoes
    printf("\n--- Teste 1: Lista Vazia ---\n");
    exibirAgenda(agenda);

    // 2. Insercao no fim em lista vazia (o novo no vira o primeiro)
    printf("\n--- Teste 2: Fim em Lista Vazia ---\n");
    inserirFim(agenda, criarMissao(102, "Verificar integridade do casco", 2));
    exibirAgenda(agenda); // Esperado: 102

    // 3. Insercao no fim em lista com nos
    printf("\n--- Teste 3: Fim em Lista com Nos ---\n");
    inserirFim(agenda, criarMissao(103, "Enviar relatorio de telemetria", 3));
    exibirAgenda(agenda); // Esperado: 102 -> 103

    // 4. Sequencia mista: inicio e fim intercalados
    printf("\n--- Teste 4: Sequencia Mista ---\n");
    inserirInicio(agenda, criarMissao(101, "URGENTE: Calibrar giroscopio", 1));
    inserirFim(agenda, criarMissao(104, "Arquivar logs de voo", 4));
    inserirInicio(agenda, criarMissao(100, "URGENTE: Estabilizar pressao interna", 1));
    inserirFim(agenda, criarMissao(105, "Atualizar manual de bordo", 5));
    exibirAgenda(agenda); // Esperado: 100 -> 101 -> 102 -> 103 -> 104 -> 105

    // 5. Protecao contra agenda nula
    if (!inserirFim(NULL, criarMissao(999, "Invalida", 1))) {
        printf("\n[Protecao OK] Insercao no fim em agenda nula rejeitada.\n");
    }

    destruirAgenda(agenda);
    printf("\nAgenda encerrada e memoria liberada com sucesso.\n");

    return 0;
}
