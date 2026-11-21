#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "missao.h"

#define ARQUIVO_PADRAO "agenda.txt"

/* ======================================================================
 *  Entrada de dados segura
 * ====================================================================== */

// Le uma linha do teclado sem estourar o buffer; descarta o excesso digitado.
// Retorna 0 em fim de entrada (EOF).
static int lerLinha(const char* prompt, char* buf, size_t tam) {
    printf("%s", prompt);
    if (fgets(buf, (int)tam, stdin) == NULL) {
        return 0;
    }
    if (strchr(buf, '\n') == NULL) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    }
    buf[strcspn(buf, "\r\n")] = '\0';
    return 1;
}

// Le um inteiro. Retorna 1 se leu, 0 se a entrada foi invalida ou acabou.
static int lerInt(const char* prompt, int* saida) {
    char buf[64];
    if (!lerLinha(prompt, buf, sizeof(buf))) {
        return 0;
    }

    char* fim;
    errno = 0;
    long v = strtol(buf, &fim, 10);
    if (fim == buf || *fim != '\0' || errno == ERANGE || v < INT_MIN || v > INT_MAX) {
        printf("[Aviso] Numero invalido.\n");
        return 0;
    }
    *saida = (int)v;
    return 1;
}

static int lerMissao(Missao* m) {
    int id, prioridade;
    char descricao[256];

    if (!lerInt("  ID: ", &id)) return 0;
    if (!lerLinha("  Descricao: ", descricao, sizeof(descricao))) return 0;
    if (!lerInt("  Prioridade (1 = mais urgente): ", &prioridade)) return 0;

    *m = criarMissao(id, descricao, prioridade);
    if (!missaoValida(m)) {
        printf("[Aviso] Dados invalidos (descricao vazia ou prioridade < 1).\n");
        return 0;
    }
    return 1;
}

static void lerCaminho(char* buf, size_t tam) {
    if (!lerLinha("  Arquivo (Enter = " ARQUIVO_PADRAO "): ", buf, tam) || buf[0] == '\0') {
        snprintf(buf, tam, "%s", ARQUIVO_PADRAO);
    }
}

/* ======================================================================
 *  Mensagens de resultado
 * ====================================================================== */

static void msgInsercao(int ok, const char* onde) {
    if (ok) {
        printf("[Sucesso] Missao inserida %s.\n", onde);
    } else {
        printf("[Aviso] Nao foi possivel inserir %s (referencia inexistente ou erro). Lista nao modificada.\n", onde);
    }
}

static void msgRemocao(int r, const Missao* removida, const char* onde) {
    if (r == 1) {
        printf("[Removida %s] ", onde);
        exibirMissao(removida);
    } else if (r == 0) {
        printf("[Aviso] Nada removido (%s): agenda vazia ou referencia ausente.\n", onde);
    } else {
        printf("[Aviso] Nao ha missao nessa posicao (%s). Lista nao modificada.\n", onde);
    }
}

static void relatarCarga(const Agenda* agenda, int r, const char* caminho) {
    switch (r) {
        case CARGA_OK:
            printf("[Sucesso] Agenda carregada de '%s': %d missao(oes).\n", caminho, contarMissoes(agenda));
            break;
        case CARGA_ARQUIVO_INEXISTENTE:
            printf("[Aviso] '%s' nao existe. A agenda continua vazia.\n", caminho);
            break;
        case CARGA_RECUSADA_AGENDA_NAO_VAZIA:
            printf("[Recusado] A agenda ja tem missoes; carregar duplicaria os dados. Use a opcao de limpar antes.\n");
            break;
        case CARGA_ARQUIVO_INVALIDO:
            printf("[Erro] '%s' contem registro invalido. Nada foi carregado.\n", caminho);
            break;
        case CARGA_SEM_MEMORIA:
            printf("[Erro] Memoria insuficiente. Nada foi carregado.\n");
            break;
        case CARGA_ERRO_LEITURA:
            printf("[Erro] Falha ao abrir ou ler '%s'.\n", caminho);
            break;
        default:
            printf("[Aviso] Parametros invalidos.\n");
            break;
    }
}

static void relatarSalvamento(int r, const char* caminho) {
    if (r == 1) {
        printf("[Sucesso] Agenda salva em '%s'.\n", caminho);
    } else if (r == -1) {
        printf("[Erro] Nao foi possivel abrir '%s' para escrita.\n", caminho);
    } else if (r == -2) {
        printf("[Erro] Falha ao gravar em '%s'.\n", caminho);
    } else {
        printf("[Aviso] Agenda ou caminho nulo. Nada salvo.\n");
    }
}

/* ======================================================================
 *  Roteiro automatico de testes (opcao 17 do menu)
 *  Usa agendas proprias: nao mexe na agenda do menu.
 * ====================================================================== */

static int testesOk = 0, testesFalha = 0;

static void conferir(int condicao, const char* descricao) {
    if (condicao) {
        testesOk++;
        printf("  [OK]    %s\n", descricao);
    } else {
        testesFalha++;
        printf("  [FALHA] %s\n", descricao);
    }
}

static int escreverArquivo(const char* caminho, const char* conteudo) {
    FILE* f = fopen(caminho, "wb");
    if (f == NULL) return 0;
    int ok = fputs(conteudo, f) >= 0;
    return fclose(f) == 0 && ok;
}

static int arquivosIguais(const char* a, const char* b) {
    FILE* fa = fopen(a, "rb");
    FILE* fb = fopen(b, "rb");
    int iguais = (fa != NULL && fb != NULL);
    while (iguais) {
        int ca = fgetc(fa), cb = fgetc(fb);
        if (ca != cb) iguais = 0;
        if (ca == EOF || cb == EOF) break;
    }
    if (fa) fclose(fa);
    if (fb) fclose(fb);
    return iguais;
}

static void executarTestes(void) {
    const char* A = "teste_a.txt";
    const char* B = "teste_b.txt";
    const char* C = "teste_c.txt";
    testesOk = testesFalha = 0;

    printf("\n=== ROTEIRO AUTOMATICO DE TESTES ===\n");

    Agenda* origem = criarAgenda();
    Agenda* nova = criarAgenda();
    if (origem == NULL || nova == NULL) {
        printf("[Erro] Sem memoria para executar os testes.\n");
        destruirAgenda(origem);
        destruirAgenda(nova);
        return;
    }

    printf("\n--- Arquivo inexistente ---\n");
    remove(C);
    conferir(carregarAgenda(nova, C) == CARGA_ARQUIVO_INEXISTENTE, "retorna CARGA_ARQUIVO_INEXISTENTE");
    conferir(contarMissoes(nova) == 0, "agenda continua vazia");

    printf("\n--- Ida e volta preservando a ordem ---\n");
    Missao lote[] = {
        criarMissao(105, "Atualizar manual de bordo", 5),
        criarMissao(103, "Enviar relatorio de telemetria", 3),
        criarMissao(108, "Calibrar giroscopio", 1),
        criarMissao(102, "Verificar integridade do casco", 3),
        criarMissao(101, "Estabilizar pressao interna", 1)
    };
    inserirLote(origem, lote, 5);
    ordenarAgenda(origem);
    conferir(salvarAgenda(origem, A) == 1, "salva a agenda ordenada");
    conferir(carregarAgenda(nova, A) == CARGA_OK, "carrega o arquivo salvo");
    conferir(contarMissoes(nova) == 5, "mesma quantidade de missoes (5)");
    conferir(salvarAgenda(nova, B) == 1, "salva a agenda recarregada");
    conferir(arquivosIguais(A, B), "arquivo regravado e identico (mesma ordem e dados)");

    printf("\n--- Antiduplicacao em carregamentos sucessivos ---\n");
    conferir(carregarAgenda(nova, A) == CARGA_RECUSADA_AGENDA_NAO_VAZIA, "segundo carregamento e recusado");
    conferir(contarMissoes(nova) == 5, "nenhuma missao foi duplicada");
    limparAgenda(nova);
    conferir(carregarAgenda(nova, A) == CARGA_OK && contarMissoes(nova) == 5,
             "limpar e carregar de novo substitui o conteudo (5 missoes)");
    limparAgenda(nova);

    printf("\n--- Formatos tolerados ---\n");
    escreverArquivo(C, "5;2;com CRLF\r\n\n7;1;a;b;c\n9;3;sem quebra final");
    Missao m;
    conferir(carregarAgenda(nova, C) == CARGA_OK, "carrega CRLF, linha em branco e ultima linha sem quebra");
    conferir(contarMissoes(nova) == 3, "3 missoes carregadas");
    conferir(buscarMissaoPorId(nova, 5, &m) && strcmp(m.descricao, "com CRLF") == 0, "CRLF removido da descricao");
    conferir(buscarMissaoPorId(nova, 7, &m) && strcmp(m.descricao, "a;b;c") == 0, "';' dentro da descricao e preservado");
    conferir(buscarMissaoPorId(nova, 9, &m) && m.prioridade == 3, "ultima linha sem quebra e lida");
    limparAgenda(nova);

    printf("\n--- Arquivo vazio ---\n");
    escreverArquivo(C, "");
    conferir(carregarAgenda(nova, C) == CARGA_OK && contarMissoes(nova) == 0, "arquivo vazio: OK com agenda vazia");

    printf("\n--- Arquivos invalidos (operacao atomica) ---\n");
    char longa[400];
    snprintf(longa, sizeof(longa), "1;1;");
    memset(longa + 4, 'x', 150);
    strcpy(longa + 154, "\n");
    const char* ruins[] = {
        "abc\n",                      // sem estrutura
        "1;1\n",                      // falta a descricao
        "1;;desc\n",                  // prioridade ausente
        "1;1;\n",                     // descricao vazia
        "1;0;desc\n",                 // prioridade invalida
        "1;-2;desc\n",                // prioridade negativa
        "99999999999;1;desc\n",       // id fora do intervalo
        "1;1;ok\n2;x;ruim\n3;1;ok\n", // registro ruim no meio: nenhum entra
        longa                          // descricao maior que o limite
    };
    int todosRecusados = 1, nenhumEntrou = 1;
    for (size_t i = 0; i < sizeof(ruins) / sizeof(ruins[0]); i++) {
        escreverArquivo(C, ruins[i]);
        if (carregarAgenda(nova, C) != CARGA_ARQUIVO_INVALIDO) todosRecusados = 0;
        if (contarMissoes(nova) != 0) nenhumEntrou = 0;
    }
    conferir(todosRecusados, "todos os 9 arquivos invalidos retornam CARGA_ARQUIVO_INVALIDO");
    conferir(nenhumEntrou, "a agenda continua vazia depois de cada falha (nenhuma insercao parcial)");

    printf("\n--- Parametros e falhas de abertura ---\n");
    conferir(carregarAgenda(NULL, A) == CARGA_PARAMETRO_INVALIDO, "agenda nula");
    conferir(carregarAgenda(nova, NULL) == CARGA_PARAMETRO_INVALIDO, "caminho nulo");
    conferir(carregarAgenda(nova, ".") == CARGA_ERRO_LEITURA, "diretorio no lugar de arquivo: erro de leitura");
    conferir(contarMissoes(nova) == 0, "agenda continua vazia");

    remove(A);
    remove(B);
    remove(C);
    destruirAgenda(origem);
    destruirAgenda(nova);

    printf("\nResultado: %d verificacao(oes) OK, %d falha(s).\n", testesOk, testesFalha);
}

/* ======================================================================
 *  Menu
 * ====================================================================== */

static void mostrarMenu(void) {
    printf("\n========== AGENDA DE MISSOES ==========\n");
    printf(" 1) Inserir no inicio          10) Buscar por ID\n");
    printf(" 2) Inserir no fim             11) Atualizar missao\n");
    printf(" 3) Inserir antes de ID        12) Ordenar por prioridade\n");
    printf(" 4) Inserir depois de ID       13) Exibir agenda\n");
    printf(" 5) Remover primeira           14) Salvar em arquivo\n");
    printf(" 6) Remover ultima             15) Carregar de arquivo\n");
    printf(" 7) Remover por ID             16) Limpar agenda\n");
    printf(" 8) Remover antes de ID        17) Roteiro automatico de testes\n");
    printf(" 9) Remover depois de ID        0) Sair\n");
    printf("=======================================\n");
}

int main(void) {
    Agenda* agenda = criarAgenda();
    if (agenda == NULL) {
        return 1;
    }

    // Reconstroi a agenda da sessao anterior, se houver
    int r = carregarAgenda(agenda, ARQUIVO_PADRAO);
    if (r == CARGA_ARQUIVO_INEXISTENTE) {
        printf("Nenhuma agenda salva encontrada. Iniciando com agenda vazia.\n");
    } else {
        relatarCarga(agenda, r, ARQUIVO_PADRAO);
    }

    int opcao = -1;
    do {
        mostrarMenu();
        if (!lerInt("Opcao: ", &opcao)) {
            if (feof(stdin)) {
                break;
            }
            opcao = -1;
            continue;
        }

        Missao m, removida;
        int id, ok;
        char caminho[256];

        switch (opcao) {
            case 1:
                if (lerMissao(&m)) msgInsercao(inserirInicio(agenda, m), "no inicio");
                break;
            case 2:
                if (lerMissao(&m)) msgInsercao(inserirFim(agenda, m), "no fim");
                break;
            case 3:
                if (lerInt("  ID de referencia: ", &id) && lerMissao(&m)) {
                    msgInsercao(inserirAntesDe(agenda, id, m), "antes da referencia");
                }
                break;
            case 4:
                if (lerInt("  ID de referencia: ", &id) && lerMissao(&m)) {
                    msgInsercao(inserirDepoisDe(agenda, id, m), "depois da referencia");
                }
                break;
            case 5:
                msgRemocao(removerInicio(agenda, &removida), &removida, "primeira");
                break;
            case 6:
                msgRemocao(removerFim(agenda, &removida), &removida, "ultima");
                break;
            case 7:
                if (lerInt("  ID: ", &id)) {
                    ok = removerPorId(agenda, id, &removida);
                    msgRemocao(ok, &removida, "por ID");
                }
                break;
            case 8:
                if (lerInt("  ID de referencia: ", &id)) {
                    msgRemocao(removerAntesDe(agenda, id, &removida), &removida, "antes da referencia");
                }
                break;
            case 9:
                if (lerInt("  ID de referencia: ", &id)) {
                    msgRemocao(removerDepoisDe(agenda, id, &removida), &removida, "depois da referencia");
                }
                break;
            case 10:
                if (lerInt("  ID: ", &id)) {
                    if (buscarMissaoPorId(agenda, id, &m)) {
                        printf("[Encontrada] ");
                        exibirMissao(&m);
                    } else {
                        printf("[Aviso] Missao #%d nao encontrada.\n", id);
                    }
                }
                break;
            case 11:
                if (lerInt("  ID da missao a corrigir: ", &id)) {
                    printf("  Novos dados (o ID nao e alterado):\n");
                    if (lerMissao(&m)) {
                        ok = atualizarMissao(agenda, id, m);
                        if (ok == 1) printf("[Sucesso] Missao #%d atualizada.\n", id);
                        else if (ok == 0) printf("[Aviso] Missao #%d inexistente. Nada alterado.\n", id);
                        else printf("[Aviso] Dados invalidos. Nada alterado.\n");
                    }
                }
                break;
            case 12:
                if (ordenarAgenda(agenda)) printf("[Sucesso] Agenda ordenada por prioridade (desempate por ID).\n");
                break;
            case 13:
                printf("Total de missoes: %d\n", contarMissoes(agenda));
                exibirAgenda(agenda);
                break;
            case 14:
                lerCaminho(caminho, sizeof(caminho));
                relatarSalvamento(salvarAgenda(agenda, caminho), caminho);
                break;
            case 15:
                lerCaminho(caminho, sizeof(caminho));
                relatarCarga(agenda, carregarAgenda(agenda, caminho), caminho);
                break;
            case 16:
                limparAgenda(agenda);
                printf("[Sucesso] Agenda limpa.\n");
                break;
            case 17:
                executarTestes();
                break;
            case 0:
                break;
            default:
                printf("[Aviso] Opcao invalida.\n");
                break;
        }
    } while (opcao != 0);

    // Encerramento: destruirAgenda descarta automaticamente as missoes restantes
    destruirAgenda(agenda);
    printf("\nAgenda encerrada e memoria liberada com sucesso.\n");

    return 0;
}
