#include "include/Graph.hpp"
#include "include/defines.hpp"
#include <queue>
#include <unordered_set>
#include <stack>
#include <set>
#include <vector>
#include <cfloat>

using namespace std;

// Construtor que lê o arquivo e constrói o grafo
Graph::Graph(ifstream& instance) : _first(nullptr) {
    string line;
    // Procurar pela linha com "param p :="
    while (getline(instance, line)) {
        if (line.find("param p") != string::npos) {
            // Ignora "param p :=" e lê o número diretamente
            istringstream param_stream(line);
            string ignore;
            param_stream >> ignore >> ignore >> ignore >> _num_clusters;
            break; // Já encontramos o número de partições, então podemos sair
        }
    }

    // Ler vértices
    getline(instance, line); // Ignorar linha de comentário
    getline(instance, line); // set V :=
    while (getline(instance, line) && line.find(';') == string::npos) {
        istringstream vertex_stream(line);
        size_t node_id;
        while (vertex_stream >> node_id) {
            add_node(node_id, 0.0f); // Inicialmente, peso 0.0f
        }
    }

    // Ler pesos dos vértices
    getline(instance, line); // Ignorar linha de comentário
    getline(instance, line); // param w :=
    while (getline(instance, line) && line.find(';') == string::npos) {
        istringstream weight_stream(line);
        size_t node_id;
        float weight;
        if (weight_stream >> node_id >> weight) {
            add_node(node_id, weight); // Atualizar o peso do nó existente
        }
    }

    // Ler arestas
    getline(instance, line); // Ignorar linha de comentário
    getline(instance, line); // set E :=
    while (getline(instance, line) && line.find(';') == string::npos) {
        stringstream ss(line);
        char ignore;
        size_t node_id_1, node_id_2;

        // Ler arestas no formato (n1, n2)
        while (ss >> ignore >> node_id_1 >> ignore >> node_id_2 >> ignore) {
            add_edge(node_id_1, node_id_2, 0.0f); // Peso padrão 0.0f
            add_edge(node_id_2, node_id_1, 0.0f); // Adiciona a aresta reversa para garantir que o grafo seja não direcionado
        }
    }
}

Graph::Graph() : _first(nullptr) {}

Graph::~Graph() {
    // Liberar memória dos nós e arestas
    Node* current_node = _first;
    while (current_node) {
        Node* next_node = current_node->_next_node;
        Edge* current_edge = current_node->_first_edge;
        while (current_edge) {
            Edge* next_edge = current_edge->_next_edge;
            delete current_edge;
            current_edge = next_edge;
        }
        delete current_node;
        current_node = next_node;
    }
}

// Função auxiliar para adicionar um nó ao grafo
void Graph::add_node(size_t node_id, float weight) {
    Node* current = _first;
    while (current) {
        if (current->_id == node_id) {
            // Atualizar o peso se o nó já existir
            current->_weight = weight;
            return;
        }
        current = current->_next_node;
    }

    // Adicionar novo nó se não existir
    Node* new_node = new Node;
    new_node->_id = node_id;
    new_node->_weight = weight;
    new_node->_first_edge = nullptr;
    new_node->_next_node = _first;
    new_node->_previous_node = nullptr;

    if (_first) {
        _first->_previous_node = new_node;
    }
    _first = new_node;

    // Atualizar o número de nós
    _number_of_nodes++;
}

// Função auxiliar para adicionar uma aresta ao grafo
void Graph::add_edge(size_t node_id_1, size_t node_id_2, float weight) {
    Node* node1 = _first;
    while (node1 && node1->_id != node_id_1) {
        node1 = node1->_next_node;
    }
    Node* node2 = _first;
    while (node2 && node2->_id != node_id_2) {
        node2 = node2->_next_node;
    }

    if (node1 && node2) {
        // Verifica se a aresta já existe para evitar duplicação em grafos não direcionados
        Edge* edge = node1->_first_edge;
        while (edge) {
            if (edge->_target_id == node_id_2) {
                return; // Aresta já existe, não adicionar novamente
            }
            edge = edge->_next_edge;
        }

        // Caso contrário, adiciona a nova aresta
        Edge* new_edge = new Edge;
        new_edge->_target_id = node_id_2;
        new_edge->_weight = weight;
        new_edge->_next_edge = node1->_first_edge;
        node1->_first_edge = new_edge;
    }
}

// Imprime o grafo, mostrando vértices, peso e arestas no formato (n1, n2)
void Graph::print_graph() {
    Node* node = _first;
    while (node) {
        cout << "Vertice: " << node->_id << " Peso: " << node->_weight << "\n";
        Edge* edge = node->_first_edge;
        while (edge) {
            cout << "  Aresta: (" << node->_id << ", " << edge->_target_id << ")\n";
            edge = edge->_next_edge;
        }
        node = node->_next_node;
    }
    // mensagem de depuracao para verificar p numero de subgrafos obtido na leitura
    cout <<"O numero de particoes eh: " << _num_clusters << endl;
}

int Graph::conected(size_t node_id_1, size_t node_id_2) {
    if (node_id_1 == node_id_2) return 1; // Vértices iguais são sempre conectados

    // Verificar se ambos os nós existem no grafo
    Node* start_node = _first;
    while (start_node && start_node->_id != node_id_1) {
        start_node = start_node->_next_node;
    }
    Node* end_node = _first;
    while (end_node && end_node->_id != node_id_2) {
        end_node = end_node->_next_node;
    }

    if (!start_node || !end_node) {
        cerr << "Erro: Um ou ambos os vértices não existem no grafo." << endl;
        return 0; // Retorna 0 se qualquer um dos nós não existir
    }

    // Implementar busca em profundidade (DFS) para verificar conectividade
    stack<size_t> s;
    unordered_set<size_t> visited;
    s.push(node_id_1);
    visited.insert(node_id_1);

    while (!s.empty()) {
        size_t current_node_id = s.top();
        s.pop();

        Node* current_node = _first;
        while (current_node && current_node->_id != current_node_id) {
            current_node = current_node->_next_node;
        }

        if (!current_node) {
            cerr << "Erro: Vértice " << current_node_id << " não encontrado." << endl;
            return 0;
        }

        Edge* edge = current_node->_first_edge;
        while (edge) {
            size_t neighbor_id = edge->_target_id;
            if (neighbor_id == node_id_2) {
                return 1; // Encontrou o nó alvo
            }
            if (visited.find(neighbor_id) == visited.end()) {
                s.push(neighbor_id);
                visited.insert(neighbor_id);
            }
            edge = edge->_next_edge;
        }
    }

    return 0;
}


Node* Graph::find_node(size_t id) {
    Node* current = _first;
    while (current) {
        if (current->_id == id) {
            return current;
        }
        current = current->_next_node;
    }
    return nullptr; // Nó não encontrado
}

// Gap de um subgrafo
float Graph::gap(const Subgraph& subgraph) {
    if (subgraph.vertices.empty()) {
        return 0.0f;
    }
    float gap_value = subgraph.max_weight - subgraph.min_weight;
    return gap_value;
}


// Guloso
float Graph::guloso(size_t p) {
    if (p > _number_of_nodes) {
        cerr << "Número de clusters não pode ser maior que o número de vértices.\n";
        return -1;
    }

    // Criar um vetor para armazenar os subgrafos
    vector<Subgraph> subgraphs(p);
    for (auto& subgraph : subgraphs) {
        subgraph.max_weight = numeric_limits<float>::min();
        subgraph.min_weight = numeric_limits<float>::max();
        subgraph.total_weight = 0.0f;
    }

    // Usar um conjunto para acompanhar quais nós foram visitados
    unordered_set<size_t> visited;
    vector<Node*> nodes;

    // Coletar todos os nós
    Node* current = _first;
    while (current) {
        nodes.push_back(current);
        current = current->_next_node;
    }

    // Realizar DFS para criar subgrafos
    size_t cluster_size = _number_of_nodes / p; // Tamanho alvo para cada subgrafo
    for (size_t i = 0; i < p && !nodes.empty(); ++i) {
        vector<size_t> vertices_in_subgraph;
        stack<size_t> s;

        // Escolher um nó que não foi visitado
        size_t start_index = rand() % nodes.size();
        while (visited.find(nodes[start_index]->_id) != visited.end()) {
            start_index = (start_index + 1) % nodes.size(); // Encontrar um nó não visitado
        }
        
        s.push(nodes[start_index]->_id);
        
        // DFS para coletar vértices conexos
        while (!s.empty() && vertices_in_subgraph.size() < cluster_size) {
            size_t current_id = s.top();
            s.pop();

            if (visited.find(current_id) == visited.end()) {
                visited.insert(current_id);
                vertices_in_subgraph.push_back(current_id);

                // Adicionar arestas conectadas
                Node* current_node = find_node(current_id);
                Edge* edge = current_node->_first_edge;
                while (edge) {
                    if (visited.find(edge->_target_id) == visited.end()) {
                        s.push(edge->_target_id);
                    }
                    edge = edge->_next_edge;
                }
            }
        }

        // Adicionar o subgrafo criado
        Subgraph& current_subgraph = subgraphs[i];
        current_subgraph.vertices = vertices_in_subgraph;

        // Atualizar pesos e limites
        for (size_t vertex_id : vertices_in_subgraph) {
            Node* node = find_node(vertex_id);
            current_subgraph.total_weight += node->_weight;
            current_subgraph.max_weight = max(current_subgraph.max_weight, node->_weight);
            current_subgraph.min_weight = min(current_subgraph.min_weight, node->_weight);
        }
    }

    // Calcular e imprimir o gap para cada subgrafo
    float total_gap = 0;
    for (size_t i = 0; i < p; ++i) {
        float subgraph_gap = gap(subgraphs[i]);
        cout << "Subgrafo " << (i + 1) << " (Vértices: ";
        for (size_t vertex : subgraphs[i].vertices) {
            cout << vertex << " ";
        }
        cout << ") - Gap: " << subgraph_gap << endl;
        total_gap += subgraph_gap;
    }
    cout << "Gap total calculado: " << total_gap << endl;
    return total_gap;
}


// Função para verificar se um subgrafo é conexo
bool Graph::check_connected(const vector<size_t>& vertices) {
    if (vertices.empty()) return false;

    unordered_set<size_t> visited;
    stack<size_t> s;
    s.push(vertices[0]); // Começa a busca a partir do primeiro vértice
    visited.insert(vertices[0]);

    while (!s.empty()) {
        size_t current = s.top();
        s.pop();

        for (const size_t neighbor : vertices) {
            if (conected(current, neighbor) && visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                s.push(neighbor);
            }
        }
    }

    return visited.size() == vertices.size(); // Verifica se todos os vértices foram visitados
}

// Algoritmo guloso randomizado adaptativo (GRASP) para particionar o grafo e minimizar o gap
float Graph::guloso_randomizado_adaptativo(size_t p, float alpha) {
    if (p > _number_of_nodes) {
        cerr << "Número de clusters não pode ser maior que o número de vértices.\n";
        return -1;
    }

    vector<Subgraph> subgraphs(p);
    for (auto& subgraph : subgraphs) {
        subgraph.max_weight = numeric_limits<float>::min();
        subgraph.min_weight = numeric_limits<float>::max();
        subgraph.total_weight = 0.0f;
    }

    unordered_set<size_t> visited;
    vector<Node*> nodes;

    Node* current = _first;
    while (current) {
        nodes.push_back(current);
        current = current->_next_node;
    }

    size_t cluster_size = _number_of_nodes / p;

    // Primeira fase: alocar vértices em subgrafos
    for (size_t i = 0; i < p && !nodes.empty(); ++i) {
        vector<size_t> vertices_in_subgraph;
        stack<size_t> s;

        size_t start_index = rand() % nodes.size();
        while (visited.find(nodes[start_index]->_id) != visited.end()) {
            start_index = (start_index + 1) % nodes.size();
        }

        s.push(nodes[start_index]->_id);

        while (!s.empty() && vertices_in_subgraph.size() < cluster_size) {
            size_t current_id = s.top();
            s.pop();

            if (visited.find(current_id) == visited.end()) {
                visited.insert(current_id);
                vertices_in_subgraph.push_back(current_id);

                vector<pair<size_t, float>> RCL;
                Node* current_node = find_node(current_id);
                Edge* edge = current_node->_first_edge;
                float max_evaluation = -numeric_limits<float>::infinity();

                while (edge) {
                    if (visited.find(edge->_target_id) == visited.end()) {
                        Node* target_node = find_node(edge->_target_id);
                        float evaluation = target_node->_weight;
                        RCL.emplace_back(edge->_target_id, evaluation);
                        max_evaluation = max(max_evaluation, evaluation);
                    }
                    edge = edge->_next_edge;
                }

                vector<pair<size_t, float>> filtered_RCL;
                for (const auto& candidate : RCL) {
                    if (candidate.second >= max_evaluation * alpha) {
                        filtered_RCL.push_back(candidate);
                    }
                }

                if (!filtered_RCL.empty()) {
                    size_t rcl_index = rand() % filtered_RCL.size();
                    s.push(filtered_RCL[rcl_index].first);
                }
            }
        }

        Subgraph& current_subgraph = subgraphs[i];
        current_subgraph.vertices = vertices_in_subgraph;

        for (size_t vertex_id : vertices_in_subgraph) {
            Node* node = find_node(vertex_id);
            current_subgraph.total_weight += node->_weight;
            current_subgraph.max_weight = max(current_subgraph.max_weight, node->_weight);
            current_subgraph.min_weight = min(current_subgraph.min_weight, node->_weight);
        }
    }

    // Segunda fase: alocar vértices restantes em subgrafos garantindo a conectividade
    for (Node* node = _first; node; node = node->_next_node) {
        if (visited.find(node->_id) == visited.end()) {
            // Tentativa de adicionar o vértice a um subgrafo existente mantendo a conectividade
            for (auto& subgraph : subgraphs) {
                if (check_connected(subgraph.vertices)) {
                    for (size_t vertex_id : subgraph.vertices) {
                        if (conected(node->_id, vertex_id)) {
                            subgraph.vertices.push_back(node->_id);
                            visited.insert(node->_id);

                            // Atualizar pesos e limites
                            subgraph.total_weight += node->_weight;
                            subgraph.max_weight = max(subgraph.max_weight, node->_weight);
                            subgraph.min_weight = min(subgraph.min_weight, node->_weight);
                            goto next_vertex; // Saia do loop ao adicionar o vértice
                        }
                    }
                }
            }
        }
next_vertex: ; // Marca o ponto de saída do loop
    }

    float total_gap = 0;
    for (size_t i = 0; i < p; ++i) {
        float subgraph_gap = gap(subgraphs[i]);
        cout << "Subgrafo " << (i + 1) << " (Vértices: ";
        for (size_t vertex : subgraphs[i].vertices) {
            cout << vertex << " ";
        }
        cout << ") - Gap: " << subgraph_gap << endl;
        total_gap += subgraph_gap;
    }
    cout << "Gap total calculado: " << total_gap << endl;
    return total_gap;
}

float Graph::guloso_randomizado_adaptativo_reativo(size_t p, size_t max_iter) {
    if (p > _number_of_nodes) {
        cerr << "Número de clusters não pode ser maior que o número de vértices.\n";
        return -1;
    }

    vector<Subgraph> best_subgraphs(p);
    vector<float> alphas = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f}; // Lista de alphas predefinidos
    size_t iter = 0;
    float total_gap = numeric_limits<float>::max();
    vector<float> gaps_per_alpha(alphas.size(), 0);
    vector<size_t> counts_per_alpha(alphas.size(), 0);
    vector<float> performance(alphas.size(), numeric_limits<float>::max());

    while (iter < max_iter) {
        vector<Subgraph> subgraphs(p);
        unordered_set<size_t> visited;
        vector<Node*> nodes;

        Node* current = _first;
        while (current) {
            nodes.push_back(current);
            current = current->_next_node;
        }

        size_t cluster_size = _number_of_nodes / p;

        for (size_t i = 0; i < p && !nodes.empty(); ++i) {
            vector<size_t> vertices_in_subgraph;
            stack<size_t> s;

            size_t start_index = rand() % nodes.size();
            while (visited.find(nodes[start_index]->_id) != visited.end()) {
                start_index = (start_index + 1) % nodes.size();
            }

            s.push(nodes[start_index]->_id);
            
            while (!s.empty() && vertices_in_subgraph.size() < cluster_size) {
                size_t current_id = s.top();
                s.pop();

                if (visited.find(current_id) == visited.end()) {
                    visited.insert(current_id);
                    vertices_in_subgraph.push_back(current_id);

                    vector<pair<size_t, float>> RCL;
                    Node* current_node = find_node(current_id);
                    Edge* edge = current_node->_first_edge;

                    while (edge) {
                        if (visited.find(edge->_target_id) == visited.end()) {
                            Node* target_node = find_node(edge->_target_id);
                            RCL.emplace_back(edge->_target_id, target_node->_weight);
                        }
                        edge = edge->_next_edge;
                    }

                    float max_weight = -numeric_limits<float>::infinity();
                    for (const auto& candidate : RCL) {
                        max_weight = max(max_weight, candidate.second);
                    }

                    vector<pair<size_t, float>> filtered_RCL;
                    for (const auto& candidate : RCL) {
                        if (candidate.second >= max_weight * 0.1f) {
                            filtered_RCL.push_back(candidate);
                        }
                    }

                    if (!filtered_RCL.empty()) {
                        size_t rcl_index = rand() % filtered_RCL.size();
                        s.push(filtered_RCL[rcl_index].first);
                    }
                }
            }

            Subgraph& current_subgraph = subgraphs[i];
            current_subgraph.vertices = vertices_in_subgraph;

            for (size_t vertex_id : vertices_in_subgraph) {
                Node* node = find_node(vertex_id);
                current_subgraph.total_weight += node->_weight;
                current_subgraph.max_weight = max(current_subgraph.max_weight, node->_weight);
                current_subgraph.min_weight = min(current_subgraph.min_weight, node->_weight);
            }
        }

        float current_gap = 0;
        cout << "Iteração " << iter + 1 << endl;
        for (size_t i = 0; i < p; ++i) {
            float subgraph_gap = gap(subgraphs[i]);
            if (std::isnan(subgraph_gap) || std::isinf(subgraph_gap)) {
                cerr << "Erro: gap inválido calculado para o subgrafo " << i + 1 << ".\n";
                return -1;
            }
            current_gap += subgraph_gap;

            cout << "Subgrafo " << (i + 1) << " (Vértices: ";
            for (size_t vertex : subgraphs[i].vertices) {
                cout << vertex << " ";
            }
            cout << ") - Gap: " << subgraph_gap << endl;
        }
        cout << "Gap total: " << current_gap << endl;

        // Atualizar o desempenho do alpha
        size_t alpha_index = rand() % alphas.size();
        gaps_per_alpha[alpha_index] += current_gap;
        counts_per_alpha[alpha_index]++;

        if (current_gap < total_gap) {
            total_gap = current_gap;
            best_subgraphs = subgraphs; // Armazenar a melhor solução
        }

        iter++;
    }

    // Impressão dos melhores subgrafos encontrados
    cout << "Melhores subgrafos encontrados:\n";
    for (size_t i = 0; i < best_subgraphs.size(); ++i) {
        cout << "Subgrafo " << (i + 1) << " (Vértices: ";
        for (size_t vertex : best_subgraphs[i].vertices) {
            cout << vertex << " ";
        }
        float subgraph_gap = gap(best_subgraphs[i]);
        if (std::isnan(subgraph_gap) || std::isinf(subgraph_gap)) {
            cerr << "Erro: gap inválido calculado para o subgrafo " << i + 1 << ".\n";
            return -1;
        }
        cout << ") - Gap: " << subgraph_gap << endl;
    }
    cout << "Gap total final: " << total_gap << endl;

    return total_gap;
}



// Algoritmo guloso randomizado adaptativo Reativo
// float Graph::guloso_randomizado_adaptativo_reativo(size_t p, size_t max_iter) {
//     // Lista de valores possíveis para alpha e suas probabilidades
//     vector<float> alphas = {0.1f, 0.3f, 0.5f, 0.7f, 0.9f};
//     vector<float> probabilities(alphas.size(), 1.0f / alphas.size()); // Probabilidade inicial uniforme
//     vector<float> alpha_performance(alphas.size(), 0.0f); // Para medir o desempenho de cada alpha
//     vector<int> alpha_count(alphas.size(), 0); // Contador de uso de cada alpha

//     random_device rd;
//     mt19937 gen(rd());

//     float best_gap = numeric_limits<float>::max(); // Melhor gap encontrado
//     auto start = chrono::high_resolution_clock::now(); // Tempo inicial

//     // Iterar até o número máximo de iterações
//     for (size_t iter = 0; iter < max_iter; ++iter) {
//         // Escolher alpha com base nas probabilidades
//         discrete_distribution<size_t> alpha_distribution(probabilities.begin(), probabilities.end());
//         size_t alpha_index = alpha_distribution(gen);
//         float alpha = alphas[alpha_index];

//         cout << "Iteração " << iter + 1 << " - Alpha escolhido: " << alpha << endl;

//         // Executar uma iteração do algoritmo guloso randomizado adaptativo
//         vector<Subgraph> subgraphs(p);
//         vector<float> subgraph_weights(p, 0.0f);
//         float current_gap = guloso_randomizado_adaptativo(p, alpha);

//         // Verificação de conectividade dos subgrafos
//         bool conexo = true;
//         for (const auto& subgraph : subgraphs) {
//             if (!check_connected(subgraph.vertices)) {
//                 conexo = false;
//                 break;
//             }
//         }

//         if (!conexo) {
//             cerr << "Erro: Subgrafo não é conexo." << endl;
//             current_gap = numeric_limits<float>::max(); // Penalizar a solução
//         }

//         // Atualizar o desempenho do alpha escolhido
//         alpha_performance[alpha_index] += current_gap;
//         alpha_count[alpha_index] += 1;

//         // Atualizar o melhor gap se necessário
//         if (current_gap < best_gap) {
//             best_gap = current_gap;
//         }

//         // Recalcular as probabilidades a cada 10 iterações
//         if ((iter + 1) % 10 == 0) {
//             float total_performance = 0.0f;
//             cout << "Probabilidades antes da atualização: ";
//             for (const auto& prob : probabilities) {
//                 cout << prob << " ";
//             }
//             cout << endl;

//             for (size_t i = 0; i < alphas.size(); ++i) {
//                 if (alpha_count[i] > 0) {
//                     probabilities[i] = 1.0f / (alpha_performance[i] / alpha_count[i]);
//                 } else {
//                     probabilities[i] = 1.0f;
//                 }
//                 total_performance += probabilities[i];
//             }

//             // Normalizar as probabilidades
//             for (auto& prob : probabilities) {
//                 prob /= total_performance;
//             }

//             cout << "Probabilidades normalizadas: ";
//             for (const auto& prob : probabilities) {
//                 cout << prob << " ";
//             }
//             cout << endl;
//         }
//     }

//     auto end = chrono::high_resolution_clock::now();
//     chrono::duration<double> elapsed = end - start;
//     cout << "Tempo total de execução: " << elapsed.count() << " segundos." << endl;

//     return best_gap; // Retorna o melhor gap encontrado
// }
