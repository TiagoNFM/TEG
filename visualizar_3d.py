import pandas as pd
import numpy as np
import networkx as nx
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def carregar_matriz_adjacencia(caminho_csv):
    """
    Lê o CSV ignorando as linhas de cabeçalho que iniciam com '#'
    e retorna a matriz de adjacências como um array NumPy.
    """
    linhas_matriz = []
    with open(caminho_csv, 'r') as f:
        for linha in f:
            linha_limpa = linha.strip()
            if not linha_limpa.startswith('#') and linha_limpa:
                valores = [int(v) for v in linha_limpa.split(',')]
                linhas_matriz.append(valores)
    return np.array(linhas_matriz)

def visualizar_grafo_3d(caminho_csv):
    # 1. Leitura do arquivo CSV gerado em C
    matriz_adj = carregar_matriz_adjacencia(caminho_csv)
    
    # 2. Criação do grafo a partir da matriz de adjacências
    G = nx.from_numpy_array(matriz_adj)
    
    # 3. Cálculo das posições dos vértices em 3D usando algoritmo de mola
    pos = nx.spring_layout(G, dim=3, seed=42)
    
    # 4. Configuração da janela 3D do Matplotlib
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')
    ax.set_title("Visualização 3D do Grafo Iris (Rotação via Mouse)")
    
    # 5. Desenho das Arestas no espaço 3D
    for edge in G.edges():
        x = [pos[edge[0]][0], pos[edge[1]][0]]
        y = [pos[edge[0]][1], pos[edge[1]][1]]
        z = [pos[edge[0]][2], pos[edge[1]][2]]
        ax.plot(x, y, z, color='gray', alpha=0.3, linewidth=0.8)
    
    # 6. Extração e desenho dos Vértices
    xs = [pos[node][0] for node in G.nodes()]
    ys = [pos[node][1] for node in G.nodes()]
    zs = [pos[node][2] for node in G.nodes()]
    
    # Mapeamento de cores por grau do vértice para realçar a estrutura
    graus = [val for node, val in G.degree()]
    sc = ax.scatter(xs, ys, zs, c=graus, cmap='viridis', s=40, edgecolors='k')
    
    # Adiciona barra de cores referente ao grau
    plt.colorbar(sc, label='Grau do Vértice', ax=ax, shrink=0.6)
    
    # Oculta eixos numéricos para focar no grafo
    ax.set_axis_off()
    
    print("Janela de visualização aberta! Clique e arraste com o mouse para rotacionar.")
    plt.show()

if __name__ == "__main__":
    visualizar_grafo_3d("grafo_iris_persistido")