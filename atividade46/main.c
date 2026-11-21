#include <stdio.h>
#include "missao.h"

// Comprova a integridade: mostra a lista e confere a quantidade esperada.
// Se a lista tivesse ciclo ou no perdido, a contagem nao bateria.
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

static void remocaoFim(Agenda* agenda) {
    Missao r;
    if (removerFim(agenda, &r)) {
        printf("[Removida do Fim] ");
        exibirMissao(&r);
    } else {
        printf("[Aviso] Nada a remover: a agenda esta vazia.\n");
    }
}

static void remocaoId(Agenda* agenda, int id) {
    Missao r;
    if (removerPorId(agenda, id, &r)) {
        printf("[Removida por ID] ");
        exibirMissao(&r);
    } else {
        printf("[Aviso] Nenhuma missao com ID #%d. Lista nao modificada.\n", id);
    }
}

int main(void) {
    printf("=== SISTEMA DE AGENDA DE MISSOES (ATIVIDADE 46) ===\n");

    Agenda* agenda = criarAgenda();
    if (agenda == NULL) {
        return 1;
    }

    // 1. Lista vazia
    printf("\n--- Teste 1: Lista Vazia ---\n");
    remocaoFim(agenda);
    remocaoId(agenda, 100);
    verificar(agenda, 0);

    // 2. Lista com um unico elemento
    printf("\n--- Teste 2: Um Unico Elemento (removerFim) ---\n");
    inserirFim(agenda, criarMissao(100, "Verificar integridade do casco", 2));
    verificar(agenda, 1);
    remocaoFim(agenda);
    verificar(agenda, 0); // lista volta a ser vazia

    printf("\n--- Teste 3: Um Unico Elemento (removerPorId) ---\n");
    inserirFim(agenda, criarMissao(100, "Verificar integridade do casco", 2));
    remocaoId(agenda, 999); // inexistente: nada muda
    verificar(agenda, 1);
    remocaoId(agenda, 100);
    verificar(agenda, 0);

    // Monta: 100 -> 200 -> 300 -> 400 -> 500
    inserirFim(agenda, criarMissao(100, "Verificar integridade do casco", 2));
    inserirFim(agenda, criarMissao(200, "Calibrar giroscopio", 1));
    inserirFim(agenda, criarMissao(300, "Enviar relatorio de telemetria", 3));
    inserirFim(agenda, criarMissao(400, "Arquivar logs de voo", 4));
    inserirFim(agenda, criarMissao(500, "Atualizar manual de bordo", 5));

    // 4. Remocao do fim em varios nos
    printf("\n--- Teste 4: Remocao do Fim ---\n");
    remocaoFim(agenda);
    verificar(agenda, 4); // 100 -> 200 -> 300 -> 400

    // 5. Remocao por ID: inicio, meio e fim
    printf("\n--- Teste 5: Remocao por ID no Inicio ---\n");
    remocaoId(agenda, 100);
    verificar(agenda, 3); // 200 -> 300 -> 400

    printf("\n--- Teste 6: Remocao por ID no Meio ---\n");
    remocaoId(agenda, 300);
    verificar(agenda, 2); // 200 -> 400

    printf("\n--- Teste 7: Remocao por ID no Fim ---\n");
    remocaoId(agenda, 400);
    verificar(agenda, 1); // 200

    // 8. ID inexistente
    printf("\n--- Teste 8: ID Inexistente ---\n");
    remocaoId(agenda, 999);
    verificar(agenda, 1); // inalterada

    // 9. Lista continua utilizavel apos as remocoes
    printf("\n--- Teste 9: Reuso Apos Remocoes ---\n");
    inserirFim(agenda, criarMissao(600, "Nova tarefa", 2));
    inserirInicio(agenda, criarMissao(50, "URGENTE: Revisar painel", 1));
    verificar(agenda, 3); // 50 -> 200 -> 600

    // 10. Agenda nula
    printf("\n--- Teste 10: Agenda Nula ---\n");
    if (!removerFim(NULL, NULL) && !removerPorId(NULL, 100, NULL)) {
        printf("[Protecao OK] Operacoes em agenda nula rejeitadas.\n");
    }

    destruirAgenda(agenda);
    printf("\nAgenda encerrada e memoria liberada com sucesso.\n");

    return 0;
}
