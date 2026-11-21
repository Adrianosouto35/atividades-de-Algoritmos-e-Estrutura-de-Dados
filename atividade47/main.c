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

static void tratarResultado(int r, const char* posicao, int idRef, const Missao* removida) {
    if (r == 1) {
        printf("[Removida %s #%d] ", posicao, idRef);
        exibirMissao(removida);
    } else if (r == 0) {
        printf("[Aviso] Referencia #%d ausente. Lista nao modificada.\n", idRef);
    } else {
        printf("[Aviso] Nao ha missao %s da #%d. Lista nao modificada.\n", posicao, idRef);
    }
}

static void remocaoDepois(Agenda* agenda, int idRef) {
    Missao r;
    tratarResultado(removerDepoisDe(agenda, idRef, &r), "depois", idRef, &r);
}

static void remocaoAntes(Agenda* agenda, int idRef) {
    Missao r;
    tratarResultado(removerAntesDe(agenda, idRef, &r), "antes", idRef, &r);
}

// Reconstroi a lista base: 100 -> 200 -> 300 -> 400
static void montarBase(Agenda* agenda) {
    inserirFim(agenda, criarMissao(100, "Verificar integridade do casco", 2));
    inserirFim(agenda, criarMissao(200, "Calibrar giroscopio", 1));
    inserirFim(agenda, criarMissao(300, "Enviar relatorio de telemetria", 3));
    inserirFim(agenda, criarMissao(400, "Arquivar logs de voo", 4));
}

static void esvaziar(Agenda* agenda) {
    while (removerInicio(agenda, NULL)) {
    }
}

int main(void) {
    printf("=== SISTEMA DE AGENDA DE MISSOES (ATIVIDADE 47) ===\n");

    Agenda* agenda = criarAgenda();
    if (agenda == NULL) {
        return 1;
    }

    // 1. Lista vazia
    printf("\n--- Teste 1: Lista Vazia ---\n");
    remocaoDepois(agenda, 100);
    remocaoAntes(agenda, 100);
    verificar(agenda, 0);

    // 2. Um unico no: nao ha nada antes nem depois
    printf("\n--- Teste 2: Um Unico No ---\n");
    inserirFim(agenda, criarMissao(100, "Verificar integridade do casco", 2));
    remocaoDepois(agenda, 100);
    remocaoAntes(agenda, 100);
    verificar(agenda, 1);
    esvaziar(agenda);

    // Varios nos
    montarBase(agenda);
    printf("\n--- Lista base ---");
    verificar(agenda, 4);

    // 3. Referencia ausente
    printf("\n--- Teste 3: Referencia Ausente ---\n");
    remocaoDepois(agenda, 999);
    remocaoAntes(agenda, 999);
    verificar(agenda, 4);

    // 4. Depois do ultimo no / antes do primeiro no
    printf("\n--- Teste 4: Limites da Lista ---\n");
    remocaoDepois(agenda, 400); // 400 e o ultimo
    remocaoAntes(agenda, 100);  // 100 e o primeiro
    verificar(agenda, 4);

    // 5. Remover depois: no meio e no ultimo no
    printf("\n--- Teste 5: Remover Depois ---\n");
    remocaoDepois(agenda, 100); // remove 200
    verificar(agenda, 3);       // 100 -> 300 -> 400
    remocaoDepois(agenda, 300); // remove 400 (o ultimo)
    verificar(agenda, 2);       // 100 -> 300
    esvaziar(agenda);

    // 6. Remover antes: o removido e o primeiro (atualiza o inicio)
    montarBase(agenda);
    printf("\n--- Teste 6: Remover Antes (removido e o primeiro) ---\n");
    remocaoAntes(agenda, 200); // remove 100, inicio passa a ser 200
    verificar(agenda, 3);      // 200 -> 300 -> 400

    // 7. Remover antes: removido no meio da lista
    printf("\n--- Teste 7: Remover Antes (removido no meio) ---\n");
    remocaoAntes(agenda, 400); // remove 300
    verificar(agenda, 2);      // 200 -> 400
    remocaoAntes(agenda, 400); // remove 200 (agora o primeiro)
    verificar(agenda, 1);      // 400

    // 8. Reuso apos as remocoes
    printf("\n--- Teste 8: Reuso Apos Remocoes ---\n");
    inserirInicio(agenda, criarMissao(50, "URGENTE: Revisar painel", 1));
    inserirFim(agenda, criarMissao(500, "Atualizar manual de bordo", 5));
    verificar(agenda, 3); // 50 -> 400 -> 500

    // 9. Agenda nula
    printf("\n--- Teste 9: Agenda Nula ---\n");
    if (removerDepoisDe(NULL, 100, NULL) == 0 && removerAntesDe(NULL, 100, NULL) == 0) {
        printf("[Protecao OK] Operacoes em agenda nula rejeitadas.\n");
    }

    destruirAgenda(agenda);
    printf("\nAgenda encerrada e memoria liberada com sucesso.\n");

    return 0;
}
