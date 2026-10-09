#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define NUM_VERTICES 150
float distancias[NUM_VERTICES][NUM_VERTICES];

typedef struct {
    float sepal_length;
    float sepal_width;
    float petal_length;
    float petal_width;
} VerticeIris;

void carregar_dataset(const char *nome_arquivo, VerticeIris vertices[]) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        exit(1); 
    }

    char linha[1024];
    int i = 0;

    fgets(linha, sizeof(linha), arquivo);

    while (fgets(linha, sizeof(linha), arquivo) != NULL && i < NUM_VERTICES) {
        sscanf(linha, "%*[^,],%f,%f,%f,%f", 
               &vertices[i].sepal_length, 
               &vertices[i].sepal_width, 
               &vertices[i].petal_length, 
               &vertices[i].petal_width);
        i++;
    }

    fclose(arquivo);
    printf("Leitura concluida com sucesso. %d registros lidos.\n", i);
}

void calcular_e_normalizar_distancias(VerticeIris vertices[]){
    
}

int main(){
    VerticeIris vertices[NUM_VERTICES];
    carregar_dataset("original_IrisDataset.csv", vertices);
    for(int i = 0; i < 150; i++) printf("%.1f, ", vertices[i].sepal_length);
    return 0;
}