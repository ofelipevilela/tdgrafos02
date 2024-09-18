#include "include/Graph.hpp"
#include "include/defines.hpp"
#include <queue>
#include <unordered_set>
#include <stack>
#include <set>

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

    // Depuração: Verificar o início e fim
    cout << "Verificando conectividade entre: " << node_id_1 << " e " << node_id_2 << endl;

    // Implementar busca em largura (BFS) para verificar conectividade
    queue<size_t> q;
    unordered_set<size_t> visited;
    q.push(node_id_1);
    visited.insert(node_id_1);

    while (!q.empty()) {
        size_t current_node_id = q.front();
        q.pop();

        Node* current_node = _first;
        while (current_node && current_node->_id != current_node_id) {
            current_node = current_node->_next_node;
        }

        if (!current_node) {
            cerr << "Erro: Vértice " << current_node_id << " não encontrado." << endl;
            return 0;
        }

        // Depuração: Nó visitado
        //cout << "Visitando nó: " << current_node->_id << endl;

        Edge* edge = current_node->_first_edge;
        while (edge) {
            size_t neighbor_id = edge->_target_id;
            if (neighbor_id == node_id_2) {
                cout << "Nó " << node_id_2 << " encontrado! Conectados." << endl;
                return 1; // Encontrou o nó alvo
            }
            if (visited.find(neighbor_id) == visited.end()) {
                q.push(neighbor_id);
                visited.insert(neighbor_id);
            }
            edge = edge->_next_edge;
        }
    }

    cout << "Nó " << node_id_2 << " não está conectado ao nó " << node_id_1 << endl;
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
    float gap_value = subgraph.max_weight - subgraph.min_weight;
    return gap_value;
}

// Algoritmo guloso randomizado adaptativo (GRASP) para particionar o grafo e minimizar o gap
float Graph::guloso_randomizado_adaptativo(size_t p, float alpha) {
    if (p <= 1 || p > _number_of_nodes) {
        cerr << "Erro: número inválido de subgrafos (" << p << "). Deve ser maior que 1 e menor ou igual ao número de vértices." << endl;
        return -1;
    }

    if (_first == nullptr) {
        cerr << "Erro: Grafo vazio!" << endl;
        return -1;
    }

    auto start = chrono::high_resolution_clock::now();

    cout << "Iniciando particionamento guloso randomizado adaptativo com " << p << " subgrafos." << endl;

    vector<Subgraph> subgraphs(p);

    vector<pair<size_t, float>> vertices;
    Node* current_node = _first;

    while (current_node) {
        vertices.push_back({current_node->_id, current_node->_weight});
        current_node = current_node->_next_node;
    }

    if (vertices.size() < p) {
        cerr << "Erro: Número insuficiente de vértices para " << p << " subgrafos!" << endl;
        return -1;
    }

    sort(vertices.begin(), vertices.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    float total_weight = 0.0f;
    for (const auto& vertex : vertices) {
        total_weight += vertex.second;
    }
    float ideal_weight_per_subgraph = total_weight / p;

    vector<float> subgraph_weights(p, 0.0f);
    random_device rd;
    mt19937 gen(rd());

    auto are_connected = [&](size_t id1, size_t id2) {
        return conected(id1, id2) == 1;
    };

    for (const auto& vertex : vertices) {
        size_t vertex_id = vertex.first;
        float vertex_weight = vertex.second;

        vector<pair<size_t, float>> lcr;
        for (size_t j = 0; j < p; ++j) {
            bool can_add = true;
            for (size_t k = 0; k < subgraphs[j].vertices.size(); ++k) {
                size_t existing_vertex_id = subgraphs[j].vertices[k];
                if (!are_connected(vertex_id, existing_vertex_id)) {
                    can_add = false;
                    break;
                }
            }
            if (can_add) {
                float new_weight = subgraph_weights[j] + vertex_weight;
                float weight_gap = abs(new_weight - ideal_weight_per_subgraph);
                lcr.push_back({j, weight_gap});
            }
        }

        if (lcr.empty()) {
            // Se nenhum subgrafo pode adicionar o vértice, escolha o subgrafo com menor peso total
            size_t least_filled_subgraph = distance(subgraph_weights.begin(), min_element(subgraph_weights.begin(), subgraph_weights.end()));
            lcr.push_back({least_filled_subgraph, abs(subgraph_weights[least_filled_subgraph] + vertex_weight - ideal_weight_per_subgraph)});
        }

        sort(lcr.begin(), lcr.end(), [](const auto& a, const auto& b) {
            return a.second < b.second;
        });

        size_t lcr_size = max(size_t(1), size_t(lcr.size() * alpha));
        uniform_int_distribution<size_t> dist(0, lcr_size - 1);
        size_t selected_subgraph = lcr[dist(gen)].first;

        subgraphs[selected_subgraph].vertices.push_back(vertex_id);
        subgraph_weights[selected_subgraph] += vertex_weight;
        subgraphs[selected_subgraph].max_weight = max(subgraphs[selected_subgraph].max_weight, vertex_weight);
        subgraphs[selected_subgraph].min_weight = min(subgraphs[selected_subgraph].min_weight, vertex_weight);
    }

    // Verificação de conectividade e realocação de vértices
    for (size_t i = 0; i < subgraphs.size(); ++i) {
        vector<size_t> to_relocate;  // Lista de vértices a realocar

        for (size_t j = 0; j < subgraphs[i].vertices.size(); ++j) {
            bool is_connected = false;
            for (size_t k = 0; k < subgraphs[i].vertices.size(); ++k) {
                if (j != k && are_connected(subgraphs[i].vertices[j], subgraphs[i].vertices[k])) {
                    is_connected = true;
                    break;
                }
            }
            if (!is_connected) {
                to_relocate.push_back(subgraphs[i].vertices[j]);  // Adicionar para realocar
            }
        }

        // Realocar vértices que não estão conectados
        for (size_t vertex_id : to_relocate) {
            subgraphs[i].vertices.erase(remove(subgraphs[i].vertices.begin(), subgraphs[i].vertices.end(), vertex_id), subgraphs[i].vertices.end());

            // Tentar encontrar outro subgrafo onde o vértice tenha conectividade
            bool relocated = false;
            for (size_t j = 0; j < subgraphs.size(); ++j) {
                if (j != i) {
                    bool can_add = true;
                    for (size_t existing_vertex_id : subgraphs[j].vertices) {
                        if (!are_connected(vertex_id, existing_vertex_id)) {
                            can_add = false;
                            break;
                        }
                    }
                    if (can_add) {
                        subgraphs[j].vertices.push_back(vertex_id);
                        relocated = true;
                        break;
                    }
                }
            }

            // Se não encontrar subgrafo com conectividade, adiciona ao subgrafo com menor peso
            if (!relocated) {
                size_t least_filled_subgraph = distance(subgraph_weights.begin(), min_element(subgraph_weights.begin(), subgraph_weights.end()));
                subgraphs[least_filled_subgraph].vertices.push_back(vertex_id);
            }
        }
    }

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
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Tempo total de execução: " << elapsed.count() << " segundos." << endl;

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