# reduz cada elemento da lista à maior diagonal superior possível nele
def limit_to_max_upper(wall, size):
    wall[0] = min(wall[0], 1)

    for i in range(1, size, 1):
        wall[i] = min(wall[i], wall[i-1] + 1)   # ou é possível construir uma diagonal 1 tijolo maior que antes,
                                                # ou a coluna atual é menor ou igual à anterior, e limita a diagonal


# reduz cada elemento da lista à maior diagonal inferior possível nele
def limit_to_max_lower(wall, size):
    wall[size-1] = min(wall[size-1], 1)

    for i in range(size-2, 0, -1):
        wall[i] = min(wall[i], wall[i+1] + 1)


# acha o maior triângulo isósceles possível na lista
def max_triangle(wall, size):
    if size == 0:
        return 0
    
    limit_to_max_upper(wall, size)  # para cada i, corta fora tudo que não pertence à maior diagonal superior
    limit_to_max_lower(wall, size)  # faz o mesmo pra maior diagonal inferior

    return max(wall)


# distancia entre dois pontos
def distance(p1, p2):
    dx = p1[0] - p2[0]
    dy = p1[1] - p2[1]
    return (dx*dx + dy*dy) ** 0.5


# perímetro de um triangulo
def perimeter(p1, p2, p3):
    return distance(p1, p2) + distance(p2, p3) + distance(p3, p1)


# acha o menor perímetro entre n pontos na força bruta
# usado como caso base da recursão
# a entrada é uma lista de tuplas no formato ( (x, y), indice_original )
def brute_force_min_perimeter(points_with_indices):
    min_p = float('inf')
    best_indices = None # armazena os índices do triângulo com menor perímetro
    n = len(points_with_indices)

    if n < 3:
        return min_p, best_indices
    
    for i in range(n):
        for j in range(i + 1, n):
            for k in range(j + 1, n):
                p1 = points_with_indices[i][0]
                p2 = points_with_indices[j][0]
                p3 = points_with_indices[k][0]
                
                p = perimeter(p1, p2, p3)
                
                # ordena os índices pra comparação lexicográfica
                idx1 = points_with_indices[i][1]
                idx2 = points_with_indices[j][1]
                idx3 = points_with_indices[k][1]
                current_indices = tuple(sorted((idx1, idx2, idx3))) # sempre ordena 3 itens -> O(1)

                if p < min_p:
                    min_p = p
                    best_indices = current_indices
                elif p == min_p:
                    # se os perímetros são iguais, checa a ordem lexicográfica dos índices
                    if best_indices is None or current_indices < best_indices:
                        best_indices = current_indices

    return min_p, best_indices


# acha o menor perímetro em uma sub-região de uma partição
# 'subregion' é uma lista de pontos já ordenada pela coordenada y
def find_subregion_min(subregion, min_p_from_halves, best_indices_from_halves):
    min_p_final = min_p_from_halves
    best_indices_final = best_indices_from_halves
    n = len(subregion)
    
    for i in range(n):
        for j in range(i + 1, n):
            # se a distância y entre os pontos já é > min_p / 2,
            # o perímetro total certamente vai ser > min_p.
            # como a lista está ordenada por y, podemos parar o loop
            if subregion[j][0][1] - subregion[i][0][1] >= min_p_from_halves / 2:
                break
                
            for k in range(j + 1, n):
                if subregion[k][0][1] - subregion[j][0][1] >= min_p_from_halves / 2:
                    break
                
                p1 = subregion[i][0]
                p2 = subregion[j][0]
                p3 = subregion[k][0]
                p = perimeter(p1, p2, p3)
                
                idx1 = subregion[i][1]
                idx2 = subregion[j][1]
                idx3 = subregion[k][1]
                current_indices = tuple(sorted((idx1, idx2, idx3)))

                if p < min_p_final:
                    min_p_final = p
                    best_indices_final = current_indices
                elif p == min_p_final:
                    # se perímetros são iguais, checa a ordem lexicográfica dos índices
                    if best_indices_final is None or current_indices < best_indices_final:
                        best_indices_final = current_indices
                    
    return min_p_final, best_indices_final


# parte recursiva da função de achar o menor perímetro
# Px é a lista de pontos ordenada por x
# Py é a lista de pontos ordenada por y
def find_min_perimeter_recursive(Px, Py):
    n = len(Px)
    # caso base: se temos 5 ou menos pontos, usamos força bruta
    if n <= 5:
        return brute_force_min_perimeter(Px)

    mid = n // 2
    median_x = Px[mid][0][0] # coordenada x do ponto mediano

    Px_left = Px[:mid]
    Px_right = Px[mid:]
    
    # divide a lista Py (ordenada por y) em duas, mantendo a ordem
    Py_left = []
    Py_right = []
    
    for point_with_index in Py:
        if point_with_index[0][0] < median_x:
            Py_left.append(point_with_index)
        else:
            Py_right.append(point_with_index)
            
    p_left, indices_left = find_min_perimeter_recursive(Px_left, Py_left)
    p_right, indices_right = find_min_perimeter_recursive(Px_right, Py_right)
    
    min_p = p_left
    best_indices = indices_left

    # compara os resultados das duas metades
    if p_right < min_p:
        min_p = p_right
        best_indices = indices_right
    elif p_right == min_p:
        # quebra o empate entre os índices se houver
        if best_indices is None or (indices_right is not None and indices_right < best_indices):
            best_indices = indices_right
    
    # cria a sub-região com pontos a uma distância < min_p/2 da linha mediana
    subregion = []
    for point_with_index in Py:
        # point_with_index[0][0] é a coordenada x
        if abs(point_with_index[0][0] - median_x) < min_p / 2:
            subregion.append(point_with_index)
            
    # o resultado final é o melhor entre as metades e a sub-região
    # a sub-região já está ordenada por y
    return find_subregion_min(subregion, min_p, best_indices)


# acha o triângulo com menor perímetro entre os pontos
# 'points_with_indices' é uma lista de tuplas no formato ((x, y), indice_original)
def find_min_perimeter_triangle(points_with_indices):
    # remove duplicatas de coordenadas (x, y), mantendo o menor índice original
    # isso é crucial para a corretude da comparação lexicográfica
    
    # primeiro, ordena por (x, y, indice) para garantir que o menor índice venha primeiro
    sorted_points = sorted(points_with_indices, key=lambda p: (p[0][0], p[0][1], p[1]))
    
    unique_points_with_indices = []
    if not sorted_points:
        return float('inf'), None
        
    last_coord = sorted_points[0][0]
    unique_points_with_indices.append(sorted_points[0])
    
    for point_with_index in sorted_points[1:]:
        if point_with_index[0] != last_coord:
            unique_points_with_indices.append(point_with_index)
            last_coord = point_with_index[0]

    n = len(unique_points_with_indices)
    if n < 3:
        return float('inf'), None

    # ordena por x (e y, e indice como tie-break)
    Px = sorted(unique_points_with_indices, key=lambda p: (p[0][0], p[0][1], p[1]))
    # ordena por y (e x, e indice como tie-break)
    Py = sorted(unique_points_with_indices, key=lambda p: (p[0][1], p[0][0], p[1]))
    
    return find_min_perimeter_recursive(Px, Py)

def main():
    try:
        # parte 1
        N_str = input()
            
        N = int(N_str)
        if N <= 0:
            wall = []
        else:
            wall = list(map(int, input().split()))
        
        if len(wall) != N:
            # lidar com entrada mal formatada se necessário
            pass

        H = max_triangle(wall, N)
        print(f"Parte 1: {H}")


        # parte 2
        Z_str = input()
        if not Z_str:
            return # entrada vazia
            
        Z = int(Z_str)
        trees = []

        # lê todas as Z árvores
        for i in range(Z):
            x, y = map(int, input().split()) 
            # armazena a tupla ((x, y), indice_original)
            # o índice é i + 1 (base 1)
            trees.append( ( (x, y), i + 1 ) ) 
        
        min_p, best_indices = find_min_perimeter_triangle(trees)
        
        if best_indices:
            # imprime o perímetro formatado e os 3 índices
            print(f"Parte 2: {min_p:.4f} {best_indices[0]} {best_indices[1]} {best_indices[2]}")
        else:
            # caso não encontre 
            print("Parte 2: 0.0000 0 0 0")

    except EOFError:
        # fim da entrada
        pass
    except Exception as e:
        # captura outras exceções
        pass

if __name__ == "__main__":
    main()