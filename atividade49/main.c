#include <stdio.h>
#include "missao.h"

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

// Importa um lote mostrando o tamanho antes e depois
static void importar(Agenda* agenda, const Missao* lote, int qtd) {
    printf("Tamanho antes da importacao: %d\n", contarMissoes(agenda));

    int r = inserirLote(agenda, lote, qtd);
    if (r == 1) {
        printf("[Sucesso] Lote de %d missao(oes) importado.\n", qtd);
    } else if (r == 0) {
        printf("[Aviso] Agenda nula. Nada importado.\n");
    } else if (r == -1) {
        printf("[Aviso] Lote invalido (quantidade ou dados). Nada importado.\n");
    } else {
        printf("[Erro] Falha de alocacao. Agenda mantida como estava.\n");
    }

    printf("Tamanho depois da importacao: %d\n", contarMissoes(agenda));
}

int main(void) {
    printf("=== SISTEMA DE AGENDA DE MISSOES (ATIVIDADE 49) ===\n");

    Agenda* agenda = criarAgenda();
    if (agenda == NULL) {
        return 1;
    }

    // 1. Contagem em lista vazia deve devolver zero
    printf("\n--- Teste 1: Contagem em Lista Vazia ---\n");
    printf("contarMissoes = %d (esperado 0)\n", contarMissoes(agenda));
    printf("contarMissoes(NULL) = %d (agenda inexistente)\n", contarMissoes(NULL));

    // 2. Importacao em lista vazia (ordem do vetor preservada)
    printf("\n--- Teste 2: Importar em Lista Vazia ---\n");
    Missao lote1[] = {
        criarMissao(101, "Verificar integridade do casco", 2),
        criarMissao(102, "Calibrar giroscopio", 1),
        criarMissao(103, "Enviar relatorio de telemetria", 3)
    };
    importar(agenda, lote1, 3);
    verificar(agenda, 3); // Esperado: 101 -> 102 -> 103

    // 3. Importacao em lista com nos: lote entra no fim, na ordem original
    printf("\n--- Teste 3: Importar em Lista com Nos ---\n");
    Missao lote2[] = {
        criarMissao(201, "Arquivar logs de voo", 4),
        criarMissao(202, "Atualizar manual de bordo", 5)
    };
    importar(agenda, lote2, 2);
    verificar(agenda, 5); // Esperado: 101 -> 102 -> 103 -> 201 -> 202

    // 4. Lote de uma unica missao
    printf("\n--- Teste 4: Lote Unitario ---\n");
    Missao lote3[] = { criarMissao(301, "Revisar painel de controle", 2) };
    importar(agenda, lote3, 1);
    verificar(agenda, 6);

    // 5. Quantidade invalida: nada deve mudar
    printf("\n--- Teste 5: Quantidade Invalida ---\n");
    importar(agenda, lote1, 0);
    importar(agenda, lote1, -4);
    importar(agenda, lote1, MAX_LOTE + 1);
    importar(agenda, NULL, 3);
    verificar(agenda, 6);

    // 6. Lote com missao invalida no meio: atomico, nenhuma missao entra
    printf("\n--- Teste 6: Lote com Missao Invalida ---\n");
    Missao loteRuim[] = {
        criarMissao(401, "Valida", 1),
        criarMissao(402, "", 2),          // descricao vazia
        criarMissao(403, "Tambem valida", 3)
    };
    importar(agenda, loteRuim, 3);
    verificar(agenda, 6);

    // 7. Agenda nula
    printf("\n--- Teste 7: Agenda Nula ---\n");
    importar(NULL, lote1, 3);

    // 8. Limpeza e reimportacao
    printf("\n--- Teste 8: Limpar e Reimportar ---\n");
    limparAgenda(agenda);
    verificar(agenda, 0);
    importar(agenda, lote2, 2);
    verificar(agenda, 2);

    destruirAgenda(agenda);
    printf("\nAgenda encerrada e memoria liberada com sucesso.\n");

    return 0;
}
