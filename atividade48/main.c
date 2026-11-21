#include <stdio.h>
#include <string.h>
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

static void tentarAtualizar(Agenda* agenda, int id, Missao novos) {
    int r = atualizarMissao(agenda, id, novos);
    if (r == 1) {
        printf("[Sucesso] Missao #%d atualizada.\n", id);
    } else if (r == 0) {
        printf("[Aviso] Missao #%d inexistente. Nada alterado.\n", id);
    } else {
        printf("[Aviso] Dados invalidos para a missao #%d. Nada alterado.\n", id);
    }
}

static void montarBase(Agenda* agenda) {
    inserirFim(agenda, criarMissao(100, "Verificar integridade do casco", 2));
    inserirFim(agenda, criarMissao(200, "Calibrar giroscopio", 1));
    inserirFim(agenda, criarMissao(300, "Enviar relatorio de telemetria", 3));
}

int main(void) {
    printf("=== SISTEMA DE AGENDA DE MISSOES (ATIVIDADE 48) ===\n");

    Agenda* agenda = criarAgenda();
    if (agenda == NULL) {
        return 1;
    }

    // 1. Limpeza em lista ja vazia (nao deve causar erro)
    printf("\n--- Teste 1: Limpar Lista Vazia ---\n");
    limparAgenda(agenda);
    limparAgenda(NULL); // agenda nula tambem e aceita
    verificar(agenda, 0);

    // 2. Atualizacao em lista vazia
    printf("\n--- Teste 2: Atualizar em Lista Vazia ---\n");
    tentarAtualizar(agenda, 100, criarMissao(100, "Qualquer", 1));

    montarBase(agenda);
    printf("\n--- Lista base ---");
    verificar(agenda, 3);

    // 3. Atualizacao valida: o id enviado em novos dados e ignorado
    printf("\n--- Teste 3: Atualizacao Valida ---\n");
    tentarAtualizar(agenda, 200, criarMissao(777, "Recalibrar giroscopio e acelerometro", 2));
    verificar(agenda, 3);
    Missao m;
    if (buscarMissaoPorId(agenda, 200, &m) && !buscarMissaoPorId(agenda, 777, NULL)) {
        printf("[Chave Preservada] #200 continua existindo e #777 nao foi criado.\n");
    }

    // 4. Atualizacao do primeiro e do ultimo no
    printf("\n--- Teste 4: Primeiro e Ultimo No ---\n");
    tentarAtualizar(agenda, 100, criarMissao(100, "Inspecionar casco com drone", 1));
    tentarAtualizar(agenda, 300, criarMissao(300, "Transmitir telemetria ao solo", 4));
    verificar(agenda, 3);

    // 5. Dados invalidos: lista deve permanecer igual
    printf("\n--- Teste 5: Dados Invalidos ---\n");
    tentarAtualizar(agenda, 100, criarMissao(100, "", 2));       // descricao vazia
    tentarAtualizar(agenda, 100, criarMissao(100, "Valida", 0)); // prioridade invalida
    tentarAtualizar(agenda, 100, criarMissao(100, "Valida", -3));
    if (buscarMissaoPorId(agenda, 100, &m) && strcmp(m.descricao, "Inspecionar casco com drone") == 0) {
        printf("[Integridade OK] Missao #100 manteve os dados anteriores.\n");
    }

    // 6. Id inexistente e agenda nula
    printf("\n--- Teste 6: Id Inexistente e Agenda Nula ---\n");
    tentarAtualizar(agenda, 999, criarMissao(999, "Fantasma", 1));
    if (atualizarMissao(NULL, 100, criarMissao(100, "Valida", 1)) == 0) {
        printf("[Protecao OK] Atualizacao em agenda nula rejeitada.\n");
    }
    verificar(agenda, 3);

    // 7. Limpeza completa e reuso da agenda
    printf("\n--- Teste 7: Limpar Lista com Nos ---\n");
    limparAgenda(agenda);
    verificar(agenda, 0);
    limparAgenda(agenda); // segunda limpeza seguida
    verificar(agenda, 0);

    printf("\n--- Teste 8: Reuso Apos Limpeza ---\n");
    inserirInicio(agenda, criarMissao(10, "Nova campanha: missao inicial", 1));
    inserirFim(agenda, criarMissao(20, "Nova campanha: missao final", 2));
    verificar(agenda, 2);

    // 9. Encerramento: destruirAgenda limpa automaticamente as missoes restantes
    printf("\n--- Teste 9: Encerramento ---\n");
    printf("Encerrando com %d missao(oes) pendente(s)...\n", contarMissoes(agenda));
    destruirAgenda(agenda);
    printf("Agenda encerrada e memoria liberada com sucesso.\n");

    return 0;
}
