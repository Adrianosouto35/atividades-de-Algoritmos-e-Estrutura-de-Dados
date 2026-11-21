#include <stdio.h>
#include "missao.h"

// Funcao auxiliar do teste: executa a insercao e mostra o resultado ao usuario
static void testarInserirDepoisDe(Agenda* agenda, int idRef, Missao nova) {
    if (inserirDepoisDe(agenda, idRef, nova)) {
        printf("[Sucesso] Missao #%d inserida depois da missao #%d.\n", nova.id, idRef);
    } else {
        printf("[Aviso] Referencia #%d inexistente. Lista nao modificada.\n", idRef);
    }
}

int main(void) {
    printf("=== SISTEMA DE AGENDA DE MISSOES (ATIVIDADE 44) ===\n");

    Agenda* agenda = criarAgenda();
    if (agenda == NULL) {
        return 1;
    }

    // 1. Busca e insercao em lista vazia
    printf("\n--- Teste 1: Lista Vazia ---\n");
    Missao achada;
    if (!buscarMissaoPorId(agenda, 100, &achada)) {
        printf("[Busca] Missao #100 ausente.\n");
    }
    testarInserirDepoisDe(agenda, 100, criarMissao(101, "Tarefa orfa", 2));
    exibirAgenda(agenda);

    // Monta a lista base: 100 -> 200 -> 300
    inserirFim(agenda, criarMissao(100, "Verificar integridade do casco", 2));
    inserirFim(agenda, criarMissao(200, "Calibrar giroscopio", 1));
    inserirFim(agenda, criarMissao(300, "Enviar relatorio de telemetria", 3));

    // 2. Busca por identificador
    printf("\n--- Teste 2: Busca por ID ---\n");
    if (buscarMissaoPorId(agenda, 200, &achada)) {
        printf("[Busca] Encontrada: ");
        exibirMissao(&achada);
    }

    // 3. Referencia no meio da lista
    printf("\n--- Teste 3: Referencia no Meio ---\n");
    testarInserirDepoisDe(agenda, 100, criarMissao(150, "Revisar sensores do casco", 2));
    exibirAgenda(agenda); // Esperado: 100 -> 150 -> 200 -> 300

    // 4. Referencia no ultimo no
    printf("\n--- Teste 4: Referencia no Ultimo No ---\n");
    testarInserirDepoisDe(agenda, 300, criarMissao(400, "Arquivar logs de voo", 4));
    exibirAgenda(agenda); // Esperado: 100 -> 150 -> 200 -> 300 -> 400

    // 5. Referencia inexistente
    printf("\n--- Teste 5: Referencia Inexistente ---\n");
    testarInserirDepoisDe(agenda, 999, criarMissao(500, "Tarefa descartada", 5));
    exibirAgenda(agenda); // Esperado: sem alteracao

    // 6. Agenda nula
    printf("\n--- Teste 6: Agenda Nula ---\n");
    if (!inserirDepoisDe(NULL, 100, criarMissao(600, "Invalida", 1))) {
        printf("[Protecao OK] Insercao em agenda nula rejeitada.\n");
    }

    destruirAgenda(agenda);
    printf("\nAgenda encerrada e memoria liberada com sucesso.\n");

    return 0;
}
