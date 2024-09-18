#include "include/Graph.hpp"
#include "include/defines.hpp"
#include <queue>

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
    // Verifica se os dois vértices são os mesmos
    if (node_id_1 == node_id_2) return 1;

    // Usar uma fila para implementar a BFS
    queue<size_t> queue;
    vector<bool> visited(_number_of_nodes, false);

    // Adicionar o primeiro nó na fila e marcar como visitado
    queue.push(node_id_1);
    visited[node_id_1] = true;

    while (!queue.empty()) {
        size_t current_node_id = queue.front();
        queue.pop();

        // Obter o nó atual da lista de nós
        Node* current_node = _first;
        while (current_node != nullptr && current_node->_id != current_node_id) {
            current_node = current_node->_next_node;
        }

        // Percorrer todas as arestas do nó atual
        Edge* edge = current_node->_first_edge;
        while (edge != nullptr) {
            size_t neighbor_id = edge->_target_id;

            // Se encontrarmos o segundo nó, retornamos verdadeiro (1)
            if (neighbor_id == node_id_2) {
                return 1;
            }

            // Se o vizinho ainda não foi visitado, o adicionamos na fila
            if (!visited[neighbor_id]) {
                visited[neighbor_id] = true;
                queue.push(neighbor_id);
            }

            // Continuar para a próxima aresta
            edge = edge->_next_edge;
        }
    }

    // Se a busca terminar e não encontramos uma conexão, retornamos falso (0)
    return 0;
}


/// GULOSO
float Graph::guloso(size_t p) {
    if (p > _number_of_nodes) 
        cerr << "Número de clusters não pode ser maior que o número de vértices.\n";
    
    // Ordenar vértices por peso em ordem decrescente
    vector<Node*> nodes;
    Node* current = _first;
    while (current) {
        nodes.push_back(current);
        current = current->_next_node;
    }
    sort(nodes.begin(), nodes.end(), [](Node* a, Node* b) {
        return a->_weight > b->_weight;
    });

    // Inicializar subgrafos
    vector<Subgraph> subgraphs(p);

    // Atribuir dois vértices a cada subgrafo
    size_t node_index = 0;
    for (size_t i = 0; i < p; ++i) {
        if (node_index < nodes.size()) {
            subgraphs[i].vertices.push_back(nodes[node_index]->_id);
            subgraphs[i].max_weight = nodes[node_index]->_weight;
            subgraphs[i].min_weight = nodes[node_index]->_weight;
            ++node_index;
        }
        if (node_index < nodes.size()) {
            subgraphs[i].vertices.push_back(nodes[node_index]->_id);
            subgraphs[i].max_weight = max(subgraphs[i].max_weight, nodes[node_index]->_weight);
            subgraphs[i].min_weight = min(subgraphs[i].min_weight, nodes[node_index]->_weight);
            ++node_index;
        }
    }

    // Atribuir os vértices restantes
    for (; node_index < nodes.size(); ++node_index) {
        // Escolher o subgrafo com o menor gap atual para adicionar o próximo vértice
        auto min_gap_subgraph = min_element(subgraphs.begin(), subgraphs.end(), [](const Subgraph& a, const Subgraph& b) {
            return a.max_weight - a.min_weight < b.max_weight - b.min_weight;
        });
        min_gap_subgraph->vertices.push_back(nodes[node_index]->_id);
        min_gap_subgraph->max_weight = max(min_gap_subgraph->max_weight, nodes[node_index]->_weight);
        min_gap_subgraph->min_weight = min(min_gap_subgraph->min_weight, nodes[node_index]->_weight);
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

// Adicione este método à classe Graph para calcular o gap de um subgrafo
float Graph::gap(const Subgraph& subgraph) {
    if (subgraph.vertices.empty()) {
        return 0.0f;
    }
    return subgraph.max_weight - subgraph.min_weight;
}

// Algoritmo guloso randomizado adaptativo (GRASP) para particionar o grafo e minimizar o gap
float Graph::guloso_randomizado_adaptativo(size_t p, float alpha) {
    // Verificar se o número de subgrafos é válido
    if (p <= 1 || p > _number_of_nodes) {
        cerr << "Erro: número inválido de subgrafos (" << p << "). Deve ser maior que 1 e menor ou igual ao número de vértices." << endl;
        return -1;
    }

    // Verifique se o grafo tem nós suficientes para particionar
    if (_first == nullptr) {
        cerr << "Erro: Grafo vazio!" << endl;
        return -1;
    }

    cout << "Iniciando particionamento guloso randomizado adaptativo com " << p << " subgrafos." << endl;

    // Inicializar p subgrafos
    vector<Subgraph> subgraphs(p);

    // Obter os vértices e seus pesos (ordenados por peso decrescente)
    vector<pair<size_t, float>> vertices; // (id, peso)
    Node* current_node = _first;

    // Adicionando os vértices ao vetor de vértices
    while (current_node) {
        vertices.push_back({current_node->_id, current_node->_weight});
        current_node = current_node->_next_node;
    }

    // Verificar se há vértices suficientes para os subgrafos
    if (vertices.size() < p) {
        cerr << "Erro: Número insuficiente de vértices para " << p << " subgrafos!" << endl;
        return -1;
    }

    // Ordenar os vértices por peso (maior para menor)
    sort(vertices.begin(), vertices.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    // Atribuir os p maiores vértices aos subgrafos
    for (size_t i = 0; i < p; ++i) {
        size_t vertex_id = vertices[i].first;
        float vertex_weight = vertices[i].second;
        subgraphs[i].vertices.push_back(vertex_id);
        subgraphs[i].max_weight = subgraphs[i].min_weight = vertex_weight;
    }

    // Atribuir os vértices restantes com randomização
    random_device rd;
    mt19937 gen(rd());

    for (size_t i = p; i < vertices.size(); ++i) {
        size_t vertex_id = vertices[i].first;
        float vertex_weight = vertices[i].second;

        // Lista de Candidatos Restrita (LCR)
        vector<pair<size_t, float>> lcr;
        float best_total_gap_increase = numeric_limits<float>::max();

        // Calcular o gap total atual antes da inserção
        float current_total_gap = 0.0f;
        for (const auto& subgraph : subgraphs) {
            current_total_gap += gap(subgraph);
        }

        // Avaliar o impacto de adicionar o vértice em cada subgrafo
        for (size_t j = 0; j < p; ++j) {
            // Impedir que subgrafos fiquem desbalanceados (muitos ou poucos vértices)
            if (subgraphs[j].vertices.size() > vertices.size() / p + 1) {
                continue;  // Pular esse subgrafo se ele já tem mais vértices que o necessário
            }

            float new_max_weight = max(subgraphs[j].max_weight, vertex_weight);
            float new_min_weight = min(subgraphs[j].min_weight, vertex_weight);
            float new_gap = new_max_weight - new_min_weight;

            // Calcular o novo gap total caso o vértice seja adicionado ao subgrafo 'j'
            float new_total_gap = current_total_gap - gap(subgraphs[j]) + new_gap;

            // Adicionar penalização para subgrafos com menos vértices
            float vertex_count_penalty = float(subgraphs[j].vertices.size()) / float(vertices.size() / p);
            new_total_gap += vertex_count_penalty;

            // Adicionar à LCR
            lcr.push_back({j, new_total_gap});

            if (new_total_gap < best_total_gap_increase) {
                best_total_gap_increase = new_total_gap;
            }
        }

        // Ordenar a LCR pelo menor aumento na soma total dos gaps
        sort(lcr.begin(), lcr.end(), [](const auto& a, const auto& b) {
            return a.second < b.second;
        });

        // Selecionar randomicamente um índice da LCR usando o parâmetro alpha
        size_t lcr_size = max(size_t(1), size_t(lcr.size() * alpha));
        uniform_int_distribution<size_t> dist(0, lcr_size - 1);
        size_t selected_subgraph = lcr[dist(gen)].first;

        // Adicionar o vértice ao subgrafo selecionado
        subgraphs[selected_subgraph].vertices.push_back(vertex_id);
        subgraphs[selected_subgraph].max_weight = max(subgraphs[selected_subgraph].max_weight, vertex_weight);
        subgraphs[selected_subgraph].min_weight = min(subgraphs[selected_subgraph].min_weight, vertex_weight);
    }

    // Calcular o gap total e imprimir os resultados
    float total_gap = 0;
    cout << "Calculando o gap total...\n";
    for (size_t i = 0; i < subgraphs.size(); ++i) {
        float subgraph_gap = gap(subgraphs[i]);

        cout << "Subgrafo " << i + 1 << " (Vértices: ";
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
    // Lista de valores possíveis para alpha e suas probabilidades
    vector<float> alphas = {0.1f, 0.3f, 0.5f, 0.7f, 0.9f};
    vector<float> probabilities(alphas.size(), 1.0f / alphas.size()); // Probabilidade inicial uniforme
    vector<float> alpha_performance(alphas.size(), 0.0f); // Para medir o desempenho de cada alpha
    vector<int> alpha_count(alphas.size(), 0); // Contador de uso de cada alpha

    random_device rd;
    mt19937 gen(rd());

    float best_gap = numeric_limits<float>::max(); // Melhor gap encontrado
    auto start = chrono::high_resolution_clock::now(); // Tempo inicial

    // Iterar até o número máximo de iterações
    for (size_t iter = 0; iter < max_iter; ++iter) {
        // Escolher alpha com base nas probabilidades
        discrete_distribution<size_t> alpha_distribution(probabilities.begin(), probabilities.end());
        size_t alpha_index = alpha_distribution(gen);
        float alpha = alphas[alpha_index];

        // Executar uma iteração do algoritmo guloso randomizado adaptativo
        float current_gap = guloso_randomizado_adaptativo(p, alpha);

        // Atualizar o desempenho do alpha escolhido
        alpha_performance[alpha_index] += current_gap;
        alpha_count[alpha_index] += 1;

        // Atualizar o melhor gap se necessário
        if (current_gap < best_gap) {
            best_gap = current_gap;
        }

        // Recalcular as probabilidades a cada 10 iterações
        if ((iter + 1) % 10 == 0) {
            float total_performance = 0.0f;
            for (size_t i = 0; i < alphas.size(); ++i) {
                if (alpha_count[i] > 0) {
                    probabilities[i] = 1.0f / (alpha_performance[i] / alpha_count[i]);
                } else {
                    probabilities[i] = 1.0f;
                }
                total_performance += probabilities[i];
            }

            // Normalizar as probabilidades
            for (auto& prob : probabilities) {
                prob /= total_performance;
            }
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Tempo total de execução: " << elapsed.count() << " segundos." << endl;

    return best_gap; // Retorna o melhor gap encontrado
}