//antes de rodar, certificar de que o arquivo .csv de leitura esta no mesmo diretorio
//para rodar em ambiente linux ubuntu: gcc fonte.c -o run -lm ; ./run

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAXV 150

typedef struct {
    double sepal_length;
    double sepal_width;
    double petal_length;
    double petal_width;
} Flor;

typedef struct {
    int matriz_adj[MAXV][MAXV];
    Flor dados_flores[MAXV];
    int grau_max, grau_min;
    double max_de, min_de, max_den, min_den;
    int max_de_i, max_de_j;
    int min_de_i, min_de_j;
    int max_den_i, max_den_j;
    int min_den_i, min_den_j;
} Grafo;

//funcao que calcula a distancia euclidiana bruta entre duas flores e retorna o valor
double calc_DistEuclidiana(Flor florX, Flor florY){
    double soma_quadrados = 
        pow(florX.sepal_length - florY.sepal_length, 2)+ 
        pow(florX.sepal_width - florY.sepal_width, 2)+ 
        pow(florX.petal_length - florY.petal_length, 2)+ 
        pow(florX.petal_width - florY.petal_width, 2);

    double resFinal_raiz = sqrt(soma_quadrados);
    return resFinal_raiz;
}

//funcao para formar a matriz de distancias euclidianas brutas entre cada par de flores
void calcularMatriz(Grafo *g, double matriz[MAXV][MAXV], double *min, double *max) {
    //definindo min e max inciais
    *min = 99999.0;
    *max = 0.0;

    for (int i = 0; i < MAXV; i++) {
        for (int j = 0; j < MAXV; j++) {
            if (i == j) {
                matriz[i][j] = 0.0;
            } else {
                double dist = calc_DistEuclidiana(g->dados_flores[i], g->dados_flores[j]); 
                
                matriz[i][j] = dist; //define aqui a distancia euclidiana bruta entre duas flores
                
                //atualiza, se necessario, o min e o max (necessarias depois para a normalizacao)
                if (dist > *max){
                    *max = dist;
                    g->max_de = dist;
                    g->max_de_i = i;
                    g->max_de_j = j;
                }
                if (dist < *min) {
                    *min = dist;
                    g->min_de = dist;
                    g->min_de_i = i;
                    g->min_de_j = j;
                }
            }
        }
    }
}

//funcao para normalizar a matriz
void calcularMatrizNorm(Grafo *g, double matriz[MAXV][MAXV], double matriz_norm[MAXV][MAXV], double min, double max) {
    double amplitude = max - min;
    g->max_den = 0.0;
    g->min_den = 1.0;

    for (int i = 0; i < MAXV; i++) {
        for (int j = 0; j < MAXV; j++) {
            if (i != j) {
                matriz_norm[i][j] = (matriz[i][j] - min) / amplitude;
                if(matriz_norm[i][j] > g->max_den){
                    g->max_den = matriz_norm[i][j];
                    g->max_den_i = i;
                    g->max_den_j = j;
                }
                if(matriz_norm[i][j] < g->min_den){
                    g->min_den = matriz_norm[i][j];
                    g->min_den_i = i;
                    g->min_den_j = j;
                }
            } else {
                matriz_norm[i][j] = 0.0; // distancia para si mesmo
            }
        }
    }
}

//funcao apenas para gerar as arestas baseadas no limiar 
void geraArestas(Grafo *g, double matriz_norm[MAXV][MAXV]) {
    for (int i = 0; i < MAXV; i++) {
        for (int j = 0; j < MAXV; j++) {
            // se forem vertices diferentes e a matriz normalizada for <= 0.3, cria a aresta
            if (i != j && matriz_norm[i][j] <= 0.3) {
                g->matriz_adj[i][j] = 1;
            } else {
                g->matriz_adj[i][j] = 0;
            }
        }
    }
}

//funcao para descobrir o grau maximo e minimo do grafo
void analizaGraus(Grafo *grafo){
    grafo->grau_max = 0;
    grafo->grau_min = MAXV;

    for(int i = 0; i < MAXV; i++){
        int conexoes = 0;
        for(int j = 0; j < MAXV; j++) {
            if (i!=j && grafo->matriz_adj[i][j] == 1) conexoes++;
        }
        if(grafo->grau_max < conexoes) grafo->grau_max = conexoes;
        if(grafo->grau_min > conexoes) grafo->grau_min = conexoes;
    }
}

int dfs(Grafo *g, int v, int visitado[]) {
    visitado[v] = 1;
    int tamanho = 1;

    for (int i = 0; i < MAXV; i++) {
        if (g->matriz_adj[v][i] == 1 && !visitado[i]) {
            tamanho += dfs(g, i, visitado);
        }
    }
    return tamanho;
}

int encontrarComponentesConexos(Grafo *g, int tamanhos[]) {
    int visitado[MAXV] = {0};
    int qtd_componentes = 0;

    for (int i = 0; i < MAXV; i++) {
        if (!visitado[i]) {
            tamanhos[qtd_componentes] = dfs(g, i, visitado);
            qtd_componentes++;
        }
    }

    return qtd_componentes;
}

//funcao para ler o csv e carregar os dados no grafo
void carregarDadosCSV(Grafo *g, const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        exit(1); //encerra se nao achar o arquivo
    }

    char linha[256];
    int i = 0;

    //le e descarta a primeira linha
    fgets(linha, sizeof(linha), arquivo);

    //le linha por linha ate o final ou ate bater o maxv (150)
    while (fgets(linha, sizeof(linha), arquivo) != NULL && i < MAXV) {
         char *token = strtok(linha, ","); 
        
        //as proximas chamadas pegam os numeros, o atof() converte texto para double nativamente em C
        token = strtok(NULL, ",");
        if (token != NULL) g->dados_flores[i].sepal_length = atof(token);
        
        token = strtok(NULL, ",");
        if (token != NULL) g->dados_flores[i].sepal_width = atof(token);
        
        token = strtok(NULL, ",");
        if (token != NULL) g->dados_flores[i].petal_length = atof(token);
        
        token = strtok(NULL, ",");
        if (token != NULL) g->dados_flores[i].petal_width = atof(token);
        i++;
    }

    fclose(arquivo);
    printf("Dados carregados com sucesso! Total de flores lidas: %d\n", i);
}

void salvarGrafoCSV(const char *nome_arquivo, Grafo *g, 
                    double max_de, double min_de, int max_de_i, int max_de_j, int min_de_i, int min_de_j,
                    double max_den, double min_den, int max_den_i, int max_den_j, int min_den_i, int min_den_j,
                    int qtd_comp, int tamanhos_comp[]) {
    
    FILE *f = fopen(nome_arquivo, "w");
    if (f == NULL) {
        printf("Erro ao criar o arquivo de saida %s\n", nome_arquivo);
        return;
    }

    //cabecalho
    fprintf(f, "# METADADOS DO GRAFO\n");
    fprintf(f, "# Total de Vertices: %d\n", MAXV);
    fprintf(f, "# Maior DE: %.4f (V%d, V%d)\n", max_de, max_de_i, max_de_j);
    fprintf(f, "# Menor DE: %.4f (V%d, V%d)\n", min_de, min_de_i, min_de_j);
    fprintf(f, "# Maior DEN: %.4f (V%d, V%d)\n", max_den, max_den_i, max_den_j);
    fprintf(f, "# Menor DEN: %.4f (V%d, V%d)\n", min_den, min_den_i, min_den_j);
    fprintf(f, "# Grau Maximo: %d | Grau Minimo: %d\n", g->grau_max, g->grau_min);
    fprintf(f, "# Tipo: Grafo Simples (Lacos: 0, Arestas Multiplas: 0)\n");
    
    fprintf(f, "# Quantidade de Componentes Conexos: %d\n", qtd_comp);
    fprintf(f, "# Tamanhos dos Componentes:");
    for (int i = 0; i < qtd_comp; i++) {
        fprintf(f, " %d", tamanhos_comp[i]);
    }
    fprintf(f, "\n");

    //escreve a matriz de adjacencias
    for (int i = 0; i < MAXV; i++) {
        for (int j = 0; j < MAXV; j++) {
            fprintf(f, "%d", g->matriz_adj[i][j]);
            if (j < MAXV - 1) fprintf(f, ",");
        }
        fprintf(f, "\n");
    }

    fclose(f);
    printf("Grafo salvo com sucesso em '%s'.\n", nome_arquivo);
}

void carregarGrafoPersistidoCSV(const char *nome_arquivo, Grafo *g) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo persistido %s\n", nome_arquivo);
        exit(1);
    }

    char linha[1024];

    //ignorar o cabeçalho que começa com '#'
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (linha[0] != '#') {
            // A primeira linha que NÃO começa com '#' é a linha 0 da matriz
            break;
        }
    }

    // preenchimento da matriz de adjacencias
    for (int i = 0; i < MAXV; i++) {
        char *token = strtok(linha, ",\n\r");
        int j = 0;

        while (token != NULL && j < MAXV) {
            g->matriz_adj[i][j] = atoi(token);
            token = strtok(NULL, ",\n\r");
            j++;
        }

        // Lê a próxima linha do arquivo para a iteração i+1 do loop
        if (i < MAXV - 1) {
            if (fgets(linha, sizeof(linha), arquivo) == NULL) {
                break; // Fim do arquivo inesperado
            }
        }
    }

    fclose(arquivo);
    printf("Grafo recarregado com sucesso a partir de '%s'!\n", nome_arquivo);
}

int main() {
    Grafo grafo, grafo_recarregado;
    
    //matrizes auxiliares locais para os calculos
    double matriz[MAXV][MAXV];
    double matriz_norm[MAXV][MAXV];
    double min, max;
    int tamanhos_componentes[MAXV];
    
    //chamadas das funcoes
    
    //le o arquivo csv (deve estar no mesmo diretorio do codigo fonte)
    carregarDadosCSV(&grafo, "Iris_Dataset.csv");
    //calcula as distancias brutas
    calcularMatriz(&grafo, matriz, &min, &max);
    //normaliza a matriz
    calcularMatrizNorm(&grafo, matriz, matriz_norm, min, max);
    //cria a matriz de adjacencias aplicando o limiar (matriz_norm <= 0.3)
    geraArestas(&grafo, matriz_norm);
    //descobre a quantidade de componentes conexos
    int total_comp = encontrarComponentesConexos(&grafo, tamanhos_componentes);
    //descobre o grau maximo e minimo do grafo
    analizaGraus(&grafo);
    //salava os dados em um novo CSV para persistencia de dados
    salvarGrafoCSV("grafo_iris_persistido", &grafo, grafo.max_de, grafo.min_de, grafo.max_de_i, grafo.max_de_j,
    grafo.min_de_i, grafo.min_de_j, grafo.max_den, grafo.min_den, grafo.max_den_i, grafo.max_den_j, grafo.min_den_i, 
    grafo.min_den_j, total_comp, tamanhos_componentes);

    printf("Os graus maximo e minimo do grafo são: %i, %i \n", grafo.grau_max, grafo.grau_min);
    

    printf("Quantidade de componentes conexos: %d\n", total_comp);
    for (int i = 0; i < total_comp; i++) {
        printf("Componente %d: %d vértices\n", i + 1, tamanhos_componentes[i]);
    }

    printf((total_comp == 1 ? "O grafo é CONEXO.\n" : "O grafo é DESCONEXO.\n"));

    
    //testando a persistencia de dados pelo CSV

    carregarGrafoPersistidoCSV("grafo_iris_persistido", &grafo_recarregado);

    analizaGraus(&grafo_recarregado);
    int tamanhos_rec[MAXV];
    int comp_rec = encontrarComponentesConexos(&grafo_recarregado, tamanhos_rec);

    printf("Componentes do grafo recarregado: %d\n", comp_rec);
    printf("Grau maximo recarregado: %d | Grau minimo: %d\n", grafo_recarregado.grau_max, grafo_recarregado.grau_min);

    return 0;
}
