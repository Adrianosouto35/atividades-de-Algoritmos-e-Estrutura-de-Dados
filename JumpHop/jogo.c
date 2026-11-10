#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "raylib.h"

#define LARGURA_TELA 800              
#define ALTURA_TELA 900               
#define FPS 60
#define GRAVIDADE 900.0f

#define MIN_FORCA_PULO 380.0f  

#define LARGURA_JOGADOR 70
#define ALTURA_JOGADOR 70

#define MAX_PLATAFORMAS 30
#define MAX_MOEDAS 30

#define DISTANCIA_MIN_PLATAFORMA 120
#define DISTANCIA_MAX_PLATAFORMA 180
#define ALTURA_PLATAFORMA 20

#define NUVEM_X 0
#define NUVEM_Y 28
#define NUVEM_LARG 56
#define NUVEM_ALT 27
#define NUVEM_OFFSET_Y 6   

#define OBST_ALCANCE_X 110.0f    
#define OBST_ALCANCE_Y 170.0f    
#define OBST_RECARGA 1.2f        
#define QUADRO_LUVA_MIN 4        
#define LUVA_LARGURA 20          
#define OBST_MARGEM_PES 8.0f     
                                 
#define LARGURA_OBSTACULO 30
#define ALTURA_OBSTACULO 25

#define VALOR_MOEDA 1
#define MAX_CORACOES 3       
#define TAM_NOME_SKIN 50
#define TEMPO_CONTAGEM_RETORNO 3

#define ARQUIVO_SAVE "save.dat"
#define NUM_SKINS_LOJA 4
#define TAXA_ACELERACAO 1.0f     
#define TAXA_REDUCAO_PULO 1.5f   

#define MAX_RECORDES 5 

typedef enum { MENU_PRINCIPAL, MENU_DIFICULDADE, MANUAL, LOJA, RANKING, JOGANDO, PAUSA, GAMEOVER } EstadoJogo;
typedef enum { FACIL, MEDIO, DIFICIL } Dificuldade;
typedef enum { PODER_NENHUM, PODER_VIDA_EXTRA, PODER_PULO_DUPLO } TipoPoder;

typedef struct {
    float velocidadeScroll;
    int larguraPlataforma;
    int chanceObstaculo;
    float forcaPulo; 
} ConfigDificuldade;

typedef struct {
    float x;
    float y;
} Posicao;

typedef struct {
    Posicao pos;
    int largura;
    int altura;
    int temMoeda;
    int temObstaculo;
    TipoPoder poder;
    int obstEstado;     
    int obstPasso;      
    float obstTempo;    
} Plataforma;

typedef struct {
    Posicao pos;
    float velocidade_y;
    int moedas;
    int pontuacao;
    int vidas;              
    int temPuloDuplo;       
    int pulosRestantes;     
    int skinAtual;          
    float tempoVivo; 
} Personagem;

typedef struct {
    char nome[TAM_NOME_SKIN];
    Color cor;
    int preco;
} InfoSkin;

typedef struct {
    int moedas;
    float tempoVivo; 
} Recorde;

typedef struct {
    int moedasTotais;
    int skinSelecionada;
    int skinsDesbloqueadas[NUM_SKINS_LOJA];
    Recorde recordes[MAX_RECORDES];
} DadosSave;

InfoSkin catalogoSkins[NUM_SKINS_LOJA] = {
    {"Padrao (Branco)", RAYWHITE, 0},
    {"Azul (Veloz)", BLUE, 15},
    {"Vermelho (Ninja)", RED, 30},
    {"Dourado (Rei)", GOLD, 75}
};

Texture2D texSkins[NUM_SKINS_LOJA];
Texture2D texObstaculo;

typedef struct {
    int x, y, w, h;
    int caixaX;
} QuadroObst;

#define NUM_QUADROS_OBST 8
const QuadroObst quadrosObst[NUM_QUADROS_OBST] = {
    {   6, 64, 32, 32, 0 },  
    { 186, 60, 36, 36, 0 },  
    { 231, 59, 37, 37, 0 },  
    { 276, 50, 32, 46, 0 },  
    { 321, 32, 32, 64, 0 },  
    { 360, 16, 44, 80, 6 },  
    { 405,  0, 44, 96, 6 },  
    { 450,  1, 44, 95, 6 }   
};

typedef struct {
    int quadro;
    float duracao;
} PassoObst;

#define NUM_PASSOS_OBST 13
const PassoObst passosObst[NUM_PASSOS_OBST] = {
    {1, 0.10f}, {2, 0.10f}, {3, 0.10f},               
    {4, 0.06f}, {5, 0.06f}, {6, 0.06f},               
    {7, 0.50f},                                       
    {6, 0.07f}, {5, 0.07f}, {4, 0.07f},               
    {3, 0.07f}, {2, 0.07f}, {1, 0.07f}                
};

void inicializarPersonagem(Personagem *p, int skinID);
void gerarPlataformas(Plataforma *plats, int numPlats, ConfigDificuldade cfg);
int atualizarFisica(Personagem *p, Plataforma *plats, int numPlats, float dt, ConfigDificuldade cfg);
void desenharCenaJogo(Personagem *jogador, Plataforma *plataformas, int partidaIniciada, Texture2D texPlataforma);
void desenharSkin(int skinID, Rectangle destino, Color tinta);
void desenharCoracao(int x, int y, int escala, int cheio);
void desenharObstaculo(float centroX, float baseY, int quadro);
int quadroObstaculo(const Plataforma *p);
void atualizarObstaculo(Plataforma *p, Rectangle recJogador, float dt);
int obstaculoColide(const Plataforma *p, Rectangle recJogador);

void carregarDados(DadosSave *dados);
void salvarDados(DadosSave *dados);

void ordenarRanking(Recorde *recordes, int n);
int buscarPosicaoRanking(Recorde *recordes, int n, int moedas, float tempo);
void formatarTempo(float totalSegundos, char *buffer);

int main(void) {
    SetTraceLogLevel(LOG_WARNING); 
    
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Jump Hop - Game");
    SetTargetFPS(FPS);
    SetExitKey(KEY_NULL); 

    Texture2D imagemFundo = LoadTexture("fundo.png");
    Texture2D imagemPlataforma = LoadTexture("plataforma.png");
    SetTextureFilter(imagemPlataforma, TEXTURE_FILTER_POINT);

    texObstaculo = LoadTexture("obstaculo.png");
    SetTextureFilter(texObstaculo, TEXTURE_FILTER_POINT);

    for (int i = 0; i < NUM_SKINS_LOJA; i++) {
        texSkins[i] = LoadTexture(TextFormat("skin%d.png", i));
    }

    EstadoJogo estadoAtual = MENU_PRINCIPAL;
    int opcaoMenuPrincipal = 0; 
    int opcaoDificuldade = 1;   
    
    float temporizador = 0.0f;
    int partidaIniciada = 0;
    int opcaoPausa = 0;     
    int opcaoLoja = 0;      
    int deveSairDoJogo = 0;

    DadosSave dadosSave;
    carregarDados(&dadosSave);

    ConfigDificuldade niveisDificuldade[3] = {
        { 50.0f,  180, 15, 550.0f }, 
        { 90.0f,  140, 35, 550.0f }, 
        { 130.0f, 110, 55, 550.0f }  
    };
    
    ConfigDificuldade cfgAtual = niveisDificuldade[MEDIO]; 

    Plataforma *plataformas = (Plataforma *)malloc(MAX_PLATAFORMAS * sizeof(Plataforma));
    Personagem jogador;

    while (!WindowShouldClose() && !deveSairDoJogo) {
        float dt = GetFrameTime();

        if (estadoAtual == MENU_PRINCIPAL) {
            if (IsKeyPressed(KEY_UP)) {
                opcaoMenuPrincipal--;
                if (opcaoMenuPrincipal < 0) opcaoMenuPrincipal = 3; 
            }
            if (IsKeyPressed(KEY_DOWN)) {
                opcaoMenuPrincipal++;
                if (opcaoMenuPrincipal > 3) opcaoMenuPrincipal = 0; 
            }

            if (IsKeyPressed(KEY_ENTER)) {
                if (opcaoMenuPrincipal == 0) estadoAtual = MENU_DIFICULDADE;
                else if (opcaoMenuPrincipal == 1) { estadoAtual = LOJA; opcaoLoja = dadosSave.skinSelecionada; }
                else if (opcaoMenuPrincipal == 2) estadoAtual = RANKING;
                else if (opcaoMenuPrincipal == 3) deveSairDoJogo = 1; 
            }
        }
        else if (estadoAtual == RANKING) {
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
                estadoAtual = MENU_PRINCIPAL;
            }
        }
        else if (estadoAtual == MENU_DIFICULDADE) {
            if (IsKeyPressed(KEY_ESCAPE)) {
                estadoAtual = MENU_PRINCIPAL; 
            }
            if (IsKeyPressed(KEY_UP)) {
                opcaoDificuldade--;
                if (opcaoDificuldade < 0) opcaoDificuldade = 2; 
            }
            if (IsKeyPressed(KEY_DOWN)) {
                opcaoDificuldade++;
                if (opcaoDificuldade > 2) opcaoDificuldade = 0; 
            }

            if (IsKeyPressed(KEY_ENTER)) {
                cfgAtual = niveisDificuldade[opcaoDificuldade]; 
                
                inicializarPersonagem(&jogador, dadosSave.skinSelecionada);
                gerarPlataformas(plataformas, MAX_PLATAFORMAS, cfgAtual);
                estadoAtual = MANUAL; 
                partidaIniciada = 0; 
            }
        }
        else if (estadoAtual == MANUAL) {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                estadoAtual = JOGANDO;
            }
        }
        else if (estadoAtual == LOJA) {
            if (IsKeyPressed(KEY_ESCAPE) || (IsKeyPressed(KEY_ENTER) && dadosSave.skinsDesbloqueadas[opcaoLoja] && dadosSave.skinSelecionada == opcaoLoja)) {
                estadoAtual = MENU_PRINCIPAL;
            }
            
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_RIGHT)) {
                opcaoLoja = (opcaoLoja + 1) % NUM_SKINS_LOJA;
            }
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_LEFT)) {
                opcaoLoja = (opcaoLoja - 1 + NUM_SKINS_LOJA) % NUM_SKINS_LOJA;
            }
            
            if (IsKeyPressed(KEY_ENTER)) {
                if (dadosSave.skinsDesbloqueadas[opcaoLoja]) {
                    dadosSave.skinSelecionada = opcaoLoja;
                    salvarDados(&dadosSave);
                    estadoAtual = MENU_PRINCIPAL; 
                } else if (dadosSave.moedasTotais >= catalogoSkins[opcaoLoja].preco) {
                    dadosSave.moedasTotais -= catalogoSkins[opcaoLoja].preco;
                    dadosSave.skinsDesbloqueadas[opcaoLoja] = 1;
                    dadosSave.skinSelecionada = opcaoLoja;
                    salvarDados(&dadosSave);
                    estadoAtual = MENU_PRINCIPAL; 
                }
            }
        }
        else if (estadoAtual == JOGANDO) {
            if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
                estadoAtual = PAUSA;
                opcaoPausa = 0;
            }
            else if (!partidaIniciada) {
                if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W)) {
                    partidaIniciada = 1;
                    jogador.velocidade_y = -cfgAtual.forcaPulo; 
                }
            } 
            else {
                jogador.tempoVivo += dt; 
                
                cfgAtual.velocidadeScroll += TAXA_ACELERACAO * dt;
                cfgAtual.forcaPulo -= TAXA_REDUCAO_PULO * dt;
                if (cfgAtual.forcaPulo < MIN_FORCA_PULO) cfgAtual.forcaPulo = MIN_FORCA_PULO;

                int morreuParaObstaculo = atualizarFisica(&jogador, plataformas, MAX_PLATAFORMAS, dt, cfgAtual);
                
                if (jogador.pos.y > ALTURA_TELA || morreuParaObstaculo) {
                    jogador.vidas--;

                    if (jogador.vidas > 0) {
                        jogador.pos.y = ALTURA_TELA / 2.0f;
                        jogador.pos.x = LARGURA_TELA / 2.0f - LARGURA_JOGADOR / 2.0f;
                        jogador.velocidade_y = -cfgAtual.forcaPulo; 
                    } else {
                        dadosSave.moedasTotais += jogador.moedas;

                        int pos = buscarPosicaoRanking(dadosSave.recordes, MAX_RECORDES, jogador.moedas, jogador.tempoVivo);
                        if (pos < MAX_RECORDES) {
                            for (int i = MAX_RECORDES - 1; i > pos; i--) {
                                dadosSave.recordes[i] = dadosSave.recordes[i - 1];
                            }
                            dadosSave.recordes[pos].moedas = jogador.moedas;
                            dadosSave.recordes[pos].tempoVivo = jogador.tempoVivo;
                        }

                        ordenarRanking(dadosSave.recordes, MAX_RECORDES);
                        
                        salvarDados(&dadosSave);
                        estadoAtual = GAMEOVER;
                        temporizador = TEMPO_CONTAGEM_RETORNO;
                    }
                }
            }
        } 
        else if (estadoAtual == PAUSA) {
            if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
                estadoAtual = JOGANDO;
            }

            if (IsKeyPressed(KEY_UP)) {
                opcaoPausa--;
                if (opcaoPausa < 0) opcaoPausa = 2;
            }
            if (IsKeyPressed(KEY_DOWN)) {
                opcaoPausa++;
                if (opcaoPausa > 2) opcaoPausa = 0;
            }

            if (IsKeyPressed(KEY_ENTER)) {
                if (opcaoPausa == 0) {
                    estadoAtual = JOGANDO;
                } 
                else if (opcaoPausa == 1) { 
                    dadosSave.moedasTotais += jogador.moedas; 
                    salvarDados(&dadosSave);
                    
                    cfgAtual = niveisDificuldade[opcaoDificuldade]; 
                    inicializarPersonagem(&jogador, dadosSave.skinSelecionada);
                    gerarPlataformas(plataformas, MAX_PLATAFORMAS, cfgAtual);
                    estadoAtual = JOGANDO;
                    partidaIniciada = 0;
                } 
                else if (opcaoPausa == 2) { 
                    dadosSave.moedasTotais += jogador.moedas; 
                    salvarDados(&dadosSave);
                    estadoAtual = MENU_PRINCIPAL;
                }
            }
        }
        else if (estadoAtual == GAMEOVER) {
            temporizador -= dt;
            if (temporizador <= 0.0f || IsKeyPressed(KEY_ENTER)) {
                estadoAtual = MENU_PRINCIPAL;
            }
        }

        BeginDrawing();
        ClearBackground(DARKBLUE);

        DrawTexturePro(
            imagemFundo,
            (Rectangle){ 0, 0, (float)imagemFundo.width, (float)imagemFundo.height },
            (Rectangle){ 0, 0, LARGURA_TELA, ALTURA_TELA },
            (Vector2){ 0, 0 },
            0.0f,
            WHITE
        );

        if (estadoAtual == MENU_PRINCIPAL) {
            DrawText("JUMP HOP", LARGURA_TELA / 2 - 120, ALTURA_TELA / 4, 50, WHITE);
            
            char strMoedas[50];
            sprintf(strMoedas, "Banco: %d Moedas", dadosSave.moedasTotais);
            DrawText(strMoedas, 20, 20, 24, GOLD);

            DrawText((opcaoMenuPrincipal == 0) ? "> JOGAR <" : "  JOGAR  ", LARGURA_TELA / 2 - 85, ALTURA_TELA / 2 - 40, 40, (opcaoMenuPrincipal == 0) ? YELLOW : DARKGRAY);
            DrawText((opcaoMenuPrincipal == 1) ? "> LOJA DE SKINS <" : "  LOJA DE SKINS  ", LARGURA_TELA / 2 - 150, ALTURA_TELA / 2 + 30, 40, (opcaoMenuPrincipal == 1) ? YELLOW : DARKGRAY);
            DrawText((opcaoMenuPrincipal == 2) ? "> RANKING <" : "  RANKING  ", LARGURA_TELA / 2 - 100, ALTURA_TELA / 2 + 100, 40, (opcaoMenuPrincipal == 2) ? YELLOW : DARKGRAY);
            DrawText((opcaoMenuPrincipal == 3) ? "> SAIR <" : "  SAIR  ", LARGURA_TELA / 2 - 75, ALTURA_TELA / 2 + 170, 40, (opcaoMenuPrincipal == 3) ? YELLOW : DARKGRAY);

            DrawText("Setas para escolher e ENTER para confirmar", LARGURA_TELA / 2 - 220, ALTURA_TELA - 80, 20, LIGHTGRAY);
        }
        else if (estadoAtual == RANKING) {
            DrawText("TOP 5 TEMPOS", LARGURA_TELA / 2 - 160, 80, 40, YELLOW);
            
            for (int i = 0; i < MAX_RECORDES; i++) {
                char linha[150];
                char strTempo[20];
                formatarTempo(dadosSave.recordes[i].tempoVivo, strTempo);
                
                sprintf(linha, "%d. %d Moedas | %s", i + 1, dadosSave.recordes[i].moedas, strTempo);
                DrawText(linha, LARGURA_TELA / 2 - 130, 180 + (i * 60), 24, WHITE);
            }

            DrawText("Pressione ESC para voltar", LARGURA_TELA / 2 - 150, ALTURA_TELA - 80, 24, LIGHTGRAY);
        }
        else if (estadoAtual == MENU_DIFICULDADE) {
            DrawText("DIFICULDADE", LARGURA_TELA / 2 - 140, ALTURA_TELA / 4, 40, WHITE);

            Color corFacil = (opcaoDificuldade == 0) ? YELLOW : DARKGRAY;
            Color corMedio = (opcaoDificuldade == 1) ? YELLOW : DARKGRAY;
            Color corDificil = (opcaoDificuldade == 2) ? YELLOW : DARKGRAY;

            DrawText((opcaoDificuldade == 0) ? "> FACIL <" : "  FACIL  ", LARGURA_TELA / 2 - 70, ALTURA_TELA / 2 - 20, 30, corFacil);
            DrawText((opcaoDificuldade == 1) ? "> MEDIO <" : "  MEDIO  ", LARGURA_TELA / 2 - 70, ALTURA_TELA / 2 + 30, 30, corMedio);
            DrawText((opcaoDificuldade == 2) ? "> DIFICIL <" : "  DIFICIL  ", LARGURA_TELA / 2 - 80, ALTURA_TELA / 2 + 80, 30, corDificil);

            DrawText("ESC para voltar ao Menu Principal", LARGURA_TELA / 2 - 170, ALTURA_TELA - 80, 20, LIGHTGRAY);
        }
        else if (estadoAtual == MANUAL) {
            DrawText("COMO JOGAR", LARGURA_TELA / 2 - MeasureText("COMO JOGAR", 40)/2, 80, 40, YELLOW);

            int startY = 180;
            int espacamento = 45;

            DrawText("CONTROLES:", 50, startY, 24, WHITE);
            DrawText("- Mover: Setas Esquerda/Direita ou A/D", 80, startY + espacamento, 20, LIGHTGRAY);
            DrawText("- Pular: Espaco ou W", 80, startY + espacamento * 2, 20, LIGHTGRAY);
            DrawText("- Pausar: Tecla P ou ESC", 80, startY + espacamento * 3, 20, LIGHTGRAY);

            startY += espacamento * 5;

            DrawText("O QUE ENCONTRAR NO CAMINHO:", 50, startY, 24, WHITE);

            DrawCircle(80, startY + espacamento + 10, 8, YELLOW);
            DrawText("- Moeda (Pegue para comprar SKINS na loja!)", 110, startY + espacamento, 20, LIGHTGRAY);

            desenharObstaculo(86, startY + espacamento*2 + 30, 0);
            DrawText("- Obstaculo (Cuidado! Encostar = Perca 1 Coracao)", 110, startY + espacamento*2 + 2, 20, LIGHTGRAY);

            DrawCircle(80, startY + espacamento*3 + 10, 10, PINK);
            DrawText("+1V - Vida Extra (Recupera 1 coracao perdido)", 110, startY + espacamento*3, 20, LIGHTGRAY);

            DrawCircle(80, startY + espacamento*4 + 10, 10, SKYBLUE);
            DrawText("2X  - Pulo Duplo (Pule novamente enquanto esta no ar!)", 110, startY + espacamento*4, 20, LIGHTGRAY);

            if ((int)(GetTime() * 2) % 2 == 0) {
                DrawText("Pressione ENTER para Comecar!", LARGURA_TELA / 2 - MeasureText("Pressione ENTER para Comecar!", 24)/2, ALTURA_TELA - 100, 24, YELLOW);
            }
        }
        else if (estadoAtual == LOJA) {
            DrawText("LOJINHA DE SKINS", LARGURA_TELA / 2 - 160, 50, 40, YELLOW);
            
            char strMoedas[50];
            sprintf(strMoedas, "Saldo: %d Moedas", dadosSave.moedasTotais);
            DrawText(strMoedas, LARGURA_TELA / 2 - MeasureText(strMoedas, 24) / 2, 110, 24, GOLD);

            for (int i = 0; i < NUM_SKINS_LOJA; i++) {
                int caixaX = LARGURA_TELA / 2 - 170;
                int caixaY = 180 + i * 90;
                
                Color corBorda = (opcaoLoja == i) ? YELLOW : DARKGRAY;
                DrawRectangleLines(caixaX, caixaY, 340, 70, corBorda);
                
                Color tintaSkin = dadosSave.skinsDesbloqueadas[i] ? WHITE : DARKGRAY;
                desenharSkin(i, (Rectangle){ (float)(caixaX + 15), (float)(caixaY + 10), 50, 50 }, tintaSkin);
                DrawText(catalogoSkins[i].nome, caixaX + 80, caixaY + 15, 20, WHITE);
                
                if (dadosSave.skinsDesbloqueadas[i]) {
                    if (dadosSave.skinSelecionada == i) {
                        DrawText(">> EQUIPADO <<", caixaX + 80, caixaY + 40, 18, GREEN);
                    } else {
                        DrawText("Aperte ENTER para Equipar", caixaX + 80, caixaY + 40, 14, LIGHTGRAY);
                    }
                } else {
                    char strPreco[30];
                    sprintf(strPreco, "Preco: %d (ENTER P/ COMPRAR)", catalogoSkins[i].preco);
                    Color corPreco = (dadosSave.moedasTotais >= catalogoSkins[i].preco) ? GREEN : RED;
                    DrawText(strPreco, caixaX + 80, caixaY + 40, 16, corPreco);
                }
            }

            DrawText("Setas: Mover | ESC: Voltar ao Menu", LARGURA_TELA / 2 - 200, ALTURA_TELA - 80, 20, LIGHTGRAY);
        }
        else if (estadoAtual == JOGANDO) {
            desenharCenaJogo(&jogador, plataformas, partidaIniciada, imagemPlataforma);
            DrawText("ESC: Pausar", LARGURA_TELA - 140, 20, 18, LIGHTGRAY);
            
            char strTempoHUD[20];
            formatarTempo(jogador.tempoVivo, strTempoHUD);
            char textoTempo[50];
            sprintf(textoTempo, "Tempo: %s", strTempoHUD);
            DrawText(textoTempo, LARGURA_TELA - 180, 50, 22, GOLD);
        } 
        else if (estadoAtual == PAUSA) {
            desenharCenaJogo(&jogador, plataformas, partidaIniciada, imagemPlataforma);
            DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, ColorAlpha(BLACK, 0.7f));

            DrawText("JOGO PAUSADO", LARGURA_TELA / 2 - 140, ALTURA_TELA / 3 - 40, 40, WHITE);

            Color corVoltar = (opcaoPausa == 0) ? YELLOW : RAYWHITE;
            Color corReiniciar = (opcaoPausa == 1) ? YELLOW : RAYWHITE;
            Color corMenu = (opcaoPausa == 2) ? YELLOW : RAYWHITE;

            DrawText((opcaoPausa == 0) ? "> Continuar <" : "  Continuar  ", LARGURA_TELA / 2 - 100, ALTURA_TELA / 2 - 10, 24, corVoltar);
            DrawText((opcaoPausa == 1) ? "> Reiniciar <" : "  Reiniciar  ", LARGURA_TELA / 2 - 100, ALTURA_TELA / 2 + 40, 24, corReiniciar);
            DrawText((opcaoPausa == 2) ? "> Voltar ao Menu <" : "  Voltar ao Menu  ", LARGURA_TELA / 2 - 120, ALTURA_TELA / 2 + 90, 24, corMenu);
        }
        else if (estadoAtual == GAMEOVER) {
            DrawText("GAME OVER", LARGURA_TELA / 2 - 120, ALTURA_TELA / 3, 40, RED);
            char textoRetorno[50];
            sprintf(textoRetorno, "Retornando ao Menu em: %d s", (int)temporizador + 1);
            DrawText(textoRetorno, LARGURA_TELA / 2 - MeasureText(textoRetorno, 22)/2, ALTURA_TELA / 2, 22, LIGHTGRAY);
            
            char strTempoGO[20];
            formatarTempo(jogador.tempoVivo, strTempoGO);
            char moedas[100];
            sprintf(moedas, "Voce coletou %d moedas em %s!", jogador.moedas, strTempoGO);
            DrawText(moedas, LARGURA_TELA / 2 - MeasureText(moedas, 20)/2, ALTURA_TELA / 2 + 40, 20, GOLD);
        }

        EndDrawing();
    }

    free(plataformas);
    UnloadTexture(imagemFundo);
    UnloadTexture(imagemPlataforma);
    UnloadTexture(texObstaculo);
    for (int i = 0; i < NUM_SKINS_LOJA; i++) UnloadTexture(texSkins[i]);
    CloseWindow();
    return 0;
}

// Converte float de segundos em string formatada "Xm YYs"
void formatarTempo(float totalSegundos, char *buffer) {
    int minutos = (int)totalSegundos / 60;
    int segundos = (int)totalSegundos % 60;
    if (minutos > 0) {
        sprintf(buffer, "%dm %02ds", minutos, segundos);
    } else {
        sprintf(buffer, "%ds", segundos);
    }
}

void carregarDados(DadosSave *dados) {
    FILE *f = fopen(ARQUIVO_SAVE, "rb");
    int resetarSave = 0;

    if (f != NULL) {
        fread(dados, sizeof(DadosSave), 1, f);
        fclose(f);

        // Validação contra dados corrompidos de versões anteriores
        for (int i = 0; i < MAX_RECORDES; i++) {
            if (dados->recordes[i].moedas < 0 || dados->recordes[i].moedas > 10000 || dados->recordes[i].tempoVivo < 0) {
                resetarSave = 1;
                break;
            }
        }
    } else {
        resetarSave = 1;
    }

    if (resetarSave) {
        dados->moedasTotais = 0;
        dados->skinSelecionada = 0; 
        dados->skinsDesbloqueadas[0] = 1; 
        for (int i = 1; i < NUM_SKINS_LOJA; i++) {
            dados->skinsDesbloqueadas[i] = 0;
        }
        for (int i = 0; i < MAX_RECORDES; i++) {
            dados->recordes[i].moedas = 0;
            dados->recordes[i].tempoVivo = 0.0f;
        }
        salvarDados(dados);
    }
}

void salvarDados(DadosSave *dados) {
    FILE *f = fopen(ARQUIVO_SAVE, "wb");
    if (f != NULL) {
        fwrite(dados, sizeof(DadosSave), 1, f);
        fclose(f);
    }
}

void ordenarRanking(Recorde *recordes, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            int trocar = 0;
            if (recordes[j].moedas < recordes[j + 1].moedas) {
                trocar = 1;
            } else if (recordes[j].moedas == recordes[j + 1].moedas && recordes[j].tempoVivo < recordes[j + 1].tempoVivo) {
                trocar = 1; 
            }
            
            if (trocar) {
                Recorde temp = recordes[j];
                recordes[j] = recordes[j + 1];
                recordes[j + 1] = temp;
            }
        }
    }
}

int buscarPosicaoRanking(Recorde *recordes, int n, int moedas, float tempo) {
    for (int i = 0; i < n; i++) {
        if (moedas > recordes[i].moedas || (moedas == recordes[i].moedas && tempo > recordes[i].tempoVivo)) {
            return i;
        }
    }
    return n; 
}

void desenharSkin(int skinID, Rectangle destino, Color tinta) {
    Texture2D tex = texSkins[skinID];

    if (tex.id == 0) {
        DrawRectangleRec(destino, (tinta.r == WHITE.r && tinta.g == WHITE.g && tinta.b == WHITE.b)
                                    ? catalogoSkins[skinID].cor : DARKGRAY);
        return;
    }

    DrawTexturePro(
        tex,
        (Rectangle){ 0, 0, (float)tex.width, (float)tex.height },
        destino,
        (Vector2){ 0, 0 },
        0.0f,
        tinta
    );
}

void desenharCoracao(int x, int y, int escala, int cheio) {
    static const char *forma[8] = {
        ".OOO.OOO.",
        "OHHRORRRO",
        "OHRRRRRRO",
        "ORRRRRRRO",
        ".ORRRRRO.",
        "..ORRRO..",
        "...ORO...",
        "....O...."
    };

    Color corContorno = cheio ? (Color){ 90, 10, 25, 255 }    : (Color){ 25, 25, 35, 255 };
    Color corPreench  = cheio ? (Color){ 230, 41, 55, 255 }   : (Color){ 60, 60, 72, 255 };
    Color corBrilho   = cheio ? (Color){ 255, 150, 160, 255 } : (Color){ 95, 95, 108, 255 };

    for (int linha = 0; linha < 8; linha++) {
        for (int coluna = 0; coluna < 9; coluna++) {
            char c = forma[linha][coluna];
            if (c == '.') continue;

            Color cor = corPreench;
            if (c == 'O') cor = corContorno;
            else if (c == 'H') cor = corBrilho;

            DrawRectangle(x + coluna * escala, y + linha * escala, escala, escala, cor);
        }
    }
}

void desenharObstaculo(float centroX, float baseY, int quadro) {
    if (texObstaculo.id == 0) {
        DrawRectangle((int)(centroX - LARGURA_OBSTACULO / 2.0f), (int)(baseY - ALTURA_OBSTACULO),
                      LARGURA_OBSTACULO, ALTURA_OBSTACULO, RED);
        return;
    }

    QuadroObst q = quadrosObst[quadro];
    DrawTexturePro(
        texObstaculo,
        (Rectangle){ (float)q.x, (float)q.y, (float)q.w, (float)q.h },
        (Rectangle){ centroX - (q.caixaX + 16.0f), baseY - q.h, (float)q.w, (float)q.h },
        (Vector2){ 0, 0 },
        0.0f,
        WHITE
    );
}

int quadroObstaculo(const Plataforma *p) {
    if (p->obstEstado == 1) return passosObst[p->obstPasso].quadro;
    return 0;
}

void atualizarObstaculo(Plataforma *p, Rectangle recJogador, float dt) {
    if (p->obstEstado == 0) {
        float centroX = p->pos.x + p->largura / 2.0f;
        float centroY = p->pos.y - 16.0f;
        float dx = (recJogador.x + recJogador.width / 2.0f) - centroX;
        float dy = (recJogador.y + recJogador.height / 2.0f) - centroY;

        if (fabsf(dx) < OBST_ALCANCE_X && fabsf(dy) < OBST_ALCANCE_Y) {
            p->obstEstado = 1;
            p->obstPasso = 0;
            p->obstTempo = 0.0f;
        }
    }
    else if (p->obstEstado == 1) {
        p->obstTempo += dt;
        while (p->obstEstado == 1 && p->obstTempo >= passosObst[p->obstPasso].duracao) {
            p->obstTempo -= passosObst[p->obstPasso].duracao;
            p->obstPasso++;
            if (p->obstPasso >= NUM_PASSOS_OBST) {
                p->obstEstado = 2;
                p->obstTempo = 0.0f;
            }
        }
    }
    else {
        p->obstTempo += dt;
        if (p->obstTempo >= OBST_RECARGA) {
            p->obstEstado = 0;
            p->obstTempo = 0.0f;
        }
    }
}

int obstaculoColide(const Plataforma *p, Rectangle recJogador) {
    float pesJogador = recJogador.y + recJogador.height;
    if (pesJogador > p->pos.y + OBST_MARGEM_PES) return 0;

    float centroX = p->pos.x + p->largura / 2.0f;

    Rectangle caixa = { centroX - LARGURA_OBSTACULO / 2.0f, p->pos.y - ALTURA_OBSTACULO,
                        LARGURA_OBSTACULO, ALTURA_OBSTACULO };
    if (CheckCollisionRecs(recJogador, caixa)) return 1;

    int quadro = quadroObstaculo(p);
    if (quadro >= QUADRO_LUVA_MIN) {
        QuadroObst q = quadrosObst[quadro];
        Rectangle luva = { centroX - LUVA_LARGURA / 2.0f, p->pos.y - q.h + 4.0f,
                           (float)LUVA_LARGURA, q.h - 4.0f };
        if (CheckCollisionRecs(recJogador, luva)) return 1;
    }

    return 0;
}

void inicializarPersonagem(Personagem *p, int skinID) {
    p->pos.x = LARGURA_TELA / 2.0f - LARGURA_JOGADOR / 2.0f;
    p->pos.y = (ALTURA_TELA - 100.0f) - ALTURA_JOGADOR; 
    p->velocidade_y = 0.0f;
    p->moedas = 0;         
    p->pontuacao = 0;
    p->vidas = MAX_CORACOES;   
    p->temPuloDuplo = 0;
    p->pulosRestantes = 0;
    p->skinAtual = skinID; 
    p->tempoVivo = 0.0f; 
}

void sortearConteudoPlataforma(Plataforma *p, ConfigDificuldade cfg, int permitirObstaculo) {
    p->obstEstado = 0;
    p->obstPasso = 0;
    p->obstTempo = 0.0f;
    int chanceRealObstaculo = permitirObstaculo ? cfg.chanceObstaculo : 0;
    
    if (GetRandomValue(1, 100) <= chanceRealObstaculo) {
        p->temObstaculo = 1;
        p->temMoeda = 0;
        p->poder = PODER_NENHUM;
    } else {
        p->temObstaculo = 0;
        int sorteio = GetRandomValue(1, 100);
        if (sorteio <= 35) {
            p->temMoeda = 1;
            p->poder = PODER_NENHUM;
        } else if (sorteio <= 55) { 
            p->temMoeda = 0;
            p->poder = (GetRandomValue(0, 1) == 0) ? PODER_VIDA_EXTRA : PODER_PULO_DUPLO;
        } else {
            p->temMoeda = 0;
            p->poder = PODER_NENHUM;
        }
    }
}

void gerarPlataformas(Plataforma *plats, int numPlats, ConfigDificuldade cfg) {
    plats[0].largura = cfg.larguraPlataforma; 
    plats[0].pos.x = LARGURA_TELA / 2.0f - plats[0].largura / 2.0f;
    plats[0].pos.y = ALTURA_TELA - 100.0f;
    plats[0].altura = ALTURA_PLATAFORMA;
    plats[0].temMoeda = 0;
    plats[0].temObstaculo = 0;
    plats[0].obstEstado = 0;
    plats[0].obstPasso = 0;
    plats[0].obstTempo = 0.0f;
    plats[0].poder = PODER_NENHUM;

    float yBase = plats[0].pos.y - GetRandomValue(DISTANCIA_MIN_PLATAFORMA, DISTANCIA_MAX_PLATAFORMA);
    
    for (int i = 1; i < numPlats; i++) {
        plats[i].largura = cfg.larguraPlataforma;
        plats[i].pos.x = GetRandomValue(20, LARGURA_TELA - plats[i].largura - 20);
        plats[i].pos.y = yBase;
        plats[i].altura = ALTURA_PLATAFORMA;
        
        sortearConteudoPlataforma(&plats[i], cfg, i >= 10);
        
        yBase -= GetRandomValue(DISTANCIA_MIN_PLATAFORMA, DISTANCIA_MAX_PLATAFORMA);
    }
}

int atualizarFisica(Personagem *p, Plataforma *plats, int numPlats, float dt, ConfigDificuldade cfg) {
    float velHorizontal = 380.0f;
    
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  p->pos.x -= velHorizontal * dt;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) p->pos.x += velHorizontal * dt;

    if (p->pos.x < 0) p->pos.x = 0;
    if (p->pos.x > LARGURA_TELA - LARGURA_JOGADOR) p->pos.x = LARGURA_TELA - LARGURA_JOGADOR;

    p->velocidade_y += GRAVIDADE * dt;
    p->pos.y += p->velocidade_y * dt;

    Rectangle recJogador = { p->pos.x, p->pos.y, LARGURA_JOGADOR, ALTURA_JOGADOR };
    
    for (int i = 0; i < numPlats; i++) {
        if (plats[i].temObstaculo) {
            atualizarObstaculo(&plats[i], recJogador, dt);

            if (obstaculoColide(&plats[i], recJogador)) {
                return 1; 
            }
        }
    }

    int noChao = 0;

    if (p->velocidade_y >= 0) { 
        for (int i = 0; i < numPlats; i++) {
            if (p->pos.x < plats[i].pos.x + plats[i].largura &&
                p->pos.x + LARGURA_JOGADOR > plats[i].pos.x &&
                p->pos.y + ALTURA_JOGADOR >= plats[i].pos.y &&
                p->pos.y + ALTURA_JOGADOR <= plats[i].pos.y + plats[i].altura + 12) {
                    
                p->pos.y = plats[i].pos.y - ALTURA_JOGADOR;
                p->velocidade_y = 0;
                noChao = 1;

                if (plats[i].temMoeda) {
                    p->moedas += VALOR_MOEDA;
                    plats[i].temMoeda = 0;
                }

                if (plats[i].poder != PODER_NENHUM) {
                    if (plats[i].poder == PODER_VIDA_EXTRA) {
                        if (p->vidas < MAX_CORACOES) p->vidas++;   
                    }
                    else if (plats[i].poder == PODER_PULO_DUPLO) p->temPuloDuplo = 1;
                    plats[i].poder = PODER_NENHUM;
                }
            }
        }
    }

    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W)) {
        if (noChao) {
            p->velocidade_y = -cfg.forcaPulo;
            if (p->temPuloDuplo) p->pulosRestantes = 1; 
        } else if (p->temPuloDuplo && p->pulosRestantes > 0) {
            p->velocidade_y = -cfg.forcaPulo * 1.25f; 
            p->pulosRestantes--;
            p->temPuloDuplo = 0; 
        }
    }

    float minY = plats[0].pos.y;
    for (int j = 1; j < numPlats; j++) {
        if (plats[j].pos.y < minY) minY = plats[j].pos.y;
    }

    for (int i = 0; i < numPlats; i++) {
        plats[i].pos.y += cfg.velocidadeScroll * dt; 
        
        if (plats[i].pos.y > ALTURA_TELA) {
            plats[i].pos.y = minY - GetRandomValue(DISTANCIA_MIN_PLATAFORMA, DISTANCIA_MAX_PLATAFORMA);
            minY = plats[i].pos.y; 
            plats[i].pos.x = GetRandomValue(20, LARGURA_TELA - plats[i].largura - 20);
            sortearConteudoPlataforma(&plats[i], cfg, 1);
        }
    }

    if (noChao && p->velocidade_y >= 0) {
        p->pos.y += cfg.velocidadeScroll * dt; 
    }
    
    return 0;
}

void desenharCenaJogo(Personagem *jogador, Plataforma *plataformas, int partidaIniciada, Texture2D texPlataforma) {
    for (int i = 0; i < MAX_PLATAFORMAS; i++) {
        float alturaNuvem = plataformas[i].largura * ((float)NUVEM_ALT / NUVEM_LARG);
        DrawTexturePro(
            texPlataforma,
            (Rectangle){ NUVEM_X, NUVEM_Y, NUVEM_LARG, NUVEM_ALT },
            (Rectangle){ plataformas[i].pos.x, plataformas[i].pos.y - NUVEM_OFFSET_Y,
                         (float)plataformas[i].largura, alturaNuvem },
            (Vector2){ 0, 0 },
            0.0f,
            WHITE
        );
        
        if (plataformas[i].temMoeda) {
            DrawCircle(plataformas[i].pos.x + plataformas[i].largura / 2, 
                       plataformas[i].pos.y - 12, 8, YELLOW);
        }
        
        if (plataformas[i].temObstaculo) {
            float obsX = plataformas[i].pos.x + plataformas[i].largura / 2.0f - LARGURA_OBSTACULO / 2.0f;
            desenharObstaculo(obsX + LARGURA_OBSTACULO / 2.0f, plataformas[i].pos.y, quadroObstaculo(&plataformas[i]));
        }

        if (plataformas[i].poder == PODER_VIDA_EXTRA) {
            float posX = plataformas[i].pos.x + plataformas[i].largura / 2.0f;
            DrawCircle(posX, plataformas[i].pos.y - 12, 10, PINK);
            DrawText("+1V", posX - 10, plataformas[i].pos.y - 18, 12, WHITE);
        } 
        else if (plataformas[i].poder == PODER_PULO_DUPLO) {
            float posX = plataformas[i].pos.x + plataformas[i].largura / 2.0f;
            DrawCircle(posX, plataformas[i].pos.y - 12, 10, SKYBLUE);
            DrawText("2X", posX - 8, plataformas[i].pos.y - 18, 12, DARKBLUE);
        }
    }

    desenharSkin(jogador->skinAtual,
                 (Rectangle){ jogador->pos.x, jogador->pos.y, LARGURA_JOGADOR, ALTURA_JOGADOR },
                 WHITE);

    char textoPlacar[50];
    sprintf(textoPlacar, "Moedas: %d", jogador->moedas);
    DrawText(textoPlacar, 20, 20, 22, RAYWHITE);

    for (int i = 0; i < MAX_CORACOES; i++) {
        desenharCoracao(20 + i * (9 * 3 + 6), 52, 3, i < jogador->vidas);
    }

    if (jogador->temPuloDuplo) {
        DrawText("Pulo Duplo: PRONTO!", 20, 90, 18, SKYBLUE);
    }

    if (!partidaIniciada) {
        DrawText("Pressione ESPACO ou W para pular e iniciar!", LARGURA_TELA / 2 - 250, ALTURA_TELA / 2, 22, YELLOW);
    }
}