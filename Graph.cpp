#include "include/Graph.hpp"
#include "include/defines.hpp"

using namespace std;

// Construtor que lê o arquivo e constrói o grafo
Graph::Graph(ifstream& instance) : _first(nullptr) {
    string line;
    while (getline(instance, line)) {
        if (line.find("param p") != string::npos) {
            // Ignora "param p :=" e lê o número diretamente
            istringstream param_stream(line);
            string ignore;
            param_stream >> ignore >> ignore >> ignore >> _num_clusters;
            break; 
        }
    }

    // Ler vérticesqlq 
    getline(instance, line); 
    getline(instance, line); 
    while (getline(instance, line) && line.find(';') == string::npos) {
        istringstream vertex_stream(line);
        size_t node_id;
        while (vertex_stream >> node_id) {
            add_node(node_id, 0.0f); 
        }
    }

    // Ler pesos dos vértices
    getline(instance, line); 
    getline(instance, line); 
    while (getline(instance, line) && line.find(';') == string::npos) {
        istringstream weight_stream(line);
        size_t node_id;
        float weight;
        if (weight_stream >> node_id >> weight) {
            add_node(node_id, weight); 
        }
    }

    // Ler arestas
    // Aqui estava o problema, grafo nao direcionado precisa de aresta para cada direção
    getline(instance, line); 
    getline(instance, line); 
    while (getline(instance, line) && line.find(';') == string::npos) {
        stringstream ss(line);
        char ignore;
        size_t node_id_1, node_id_2;

        while (ss >> ignore >> node_id_1 >> ignore >> node_id_2 >> ignore) {
            add_edge(node_id_1, node_id_2, 0.0f); 
            add_edge(node_id_2, node_id_1, 0.0f); 
        }
    }
}

Graph::Graph() : _first(nullptr) {}

Graph::~Graph() {
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

void Graph::add_node(size_t node_id, float weight) {
    Node* current = _first;
    while (current) {
        if (current->_id == node_id) {
            current->_weight = weight;
            return;
        }
        current = current->_next_node;
    }
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
    _number_of_nodes++;
}

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
        Edge* edge = node1->_first_edge;
        while (edge) {
            if (edge->_target_id == node_id_2) { // Verifica se a aresta já existe para evitar duplicação
                return;
            }
            edge = edge->_next_edge;
        }

        Edge* new_edge = new Edge;
        new_edge->_target_id = node_id_2;
        new_edge->_weight = weight;
        new_edge->_next_edge = node1->_first_edge;
        node1->_first_edge = new_edge;
    }
}

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
    cout <<"O numero de particoes eh: " << _num_clusters << endl;
}

int Graph::conected(size_t node_id_1, size_t node_id_2) {
    if (node_id_1 == node_id_2) return 1; 
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
        return 0; 
    }

    //busca em profundidade para verificar conectividade
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


float Graph::gap(const Subgraph& subgraph) {
    if (subgraph.vertices.empty()) {
        return 0.0f;
    }
    float gap_value = subgraph.max_weight - subgraph.min_weight;
    return gap_value;
}


/// GULOSO
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
    size_t cluster_size = _number_of_nodes / p; 
    for (size_t i = 0; i < p && !nodes.empty(); ++i) {
        vector<size_t> vertices_in_subgraph;
        stack<size_t> s;

        size_t start_index = rand() % nodes.size();
        while (visited.find(nodes[start_index]->_id) != visited.end()) {
            start_index = (start_index + 1) % nodes.size(); 
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

        // Verificar se o subgrafo tem pelo menos 2 vértices
        if (vertices_in_subgraph.size() < 2 && i > 0) {
            // Se o subgrafo for muito pequeno, mova vértices dos subgrafos anteriores
            for (size_t j = 0; j < i; ++j) {
                if (subgraphs[j].vertices.size() > 2) {
                    vertices_in_subgraph.push_back(subgraphs[j].vertices.back());
                    subgraphs[j].vertices.pop_back();
                    break;
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

    // Verificar se todos os subgrafos têm pelo menos 2 vértices
    for (size_t i = 0; i < p; ++i) {
        if (subgraphs[i].vertices.size() < 2) {
            cerr << "Erro: Subgrafo " << i + 1 << " tem menos de 2 vértices. Corrigindo...\n";
            // Mover vértices de outros subgrafos com mais de 2 vértices
            for (size_t j = 0; j < p; ++j) {
                if (subgraphs[j].vertices.size() > 2) {
                    subgraphs[i].vertices.push_back(subgraphs[j].vertices.back());
                    subgraphs[j].vertices.pop_back();
                    break;
                }
            }
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




// Otimizar a verificação da conectividade apenas uma vez e manter o estado
bool Graph::verifica_conexo(const vector<size_t>& vertices, size_t new_vertex) {
    if (vertices.empty()) return true;
    for (const size_t vertex : vertices) {
        if (conected(new_vertex, vertex)) {
            return true;
        }
    }
    return false;
}


/// GULOSO RANDOMIZADO ADAPTATIVO
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
        size_t start_index = rand() % nodes.size();
        while (visited.find(nodes[start_index]->_id) != visited.end()) {
            start_index = (start_index + 1) % nodes.size();
        }

        size_t start_vertex = nodes[start_index]->_id;
        visited.insert(start_vertex);
        vertices_in_subgraph.push_back(start_vertex);

        Node* start_node = find_node(start_vertex);
        priority_queue<pair<float, size_t>> candidates;

        // Inserir vértices conectados na fila de prioridades
        for (Edge* edge = start_node->_first_edge; edge; edge = edge->_next_edge) {
            if (visited.find(edge->_target_id) == visited.end()) {
                Node* target_node = find_node(edge->_target_id);
                candidates.push({ target_node->_weight, edge->_target_id });
            }
        }

        // Expandir subgrafo até atingir o tamanho desejado ou até o máximo de candidatos
        while (!candidates.empty() && vertices_in_subgraph.size() < cluster_size) {
            auto [weight, candidate_id] = candidates.top();
            candidates.pop();

            if (visited.find(candidate_id) == visited.end()) {
                visited.insert(candidate_id);
                vertices_in_subgraph.push_back(candidate_id);

                // Adicionar vértices conectados ao novo candidato na fila
                Node* candidate_node = find_node(candidate_id);
                for (Edge* edge = candidate_node->_first_edge; edge; edge = edge->_next_edge) {
                    if (visited.find(edge->_target_id) == visited.end()) {
                        Node* target_node = find_node(edge->_target_id);
                        candidates.push({ target_node->_weight, edge->_target_id });
                    }
                }
            }
        }

        // Verificar se o subgrafo contém pelo menos dois vértices
        if (vertices_in_subgraph.size() < 2) {
            cerr << "O subgrafo gerado contém menos de dois vértices. Ajustando...\n";
            
            // Procurar e adicionar vértices não visitados adjacentes
            for (Node* extra_node : nodes) {
                if (visited.find(extra_node->_id) == visited.end()) {
                    if (verifica_conexo(vertices_in_subgraph, extra_node->_id)) {
                        vertices_in_subgraph.push_back(extra_node->_id);
                        visited.insert(extra_node->_id);

                        if (vertices_in_subgraph.size() >= 2) {
                            break;
                        }
                    }
                }
            }

            // Se ainda não temos dois vértices, houve uma falha no ajuste
            if (vertices_in_subgraph.size() < 2) {
                cerr << "Não foi possível encontrar vértices suficientes para o subgrafo.\n";
                return -1;
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
                if (verifica_conexo(subgraph.vertices, node->_id)) {
                    subgraph.vertices.push_back(node->_id);
                    visited.insert(node->_id);

                    // Atualizar pesos e limites
                    subgraph.total_weight += node->_weight;
                    subgraph.max_weight = max(subgraph.max_weight, node->_weight);
                    subgraph.min_weight = min(subgraph.min_weight, node->_weight);
                    break; // Saia do loop ao adicionar o vértice
                }
            }
        }
    }

    // Verificar novamente que todos os subgrafos têm pelo menos dois vértices
    for (auto& subgraph : subgraphs) {
        if (subgraph.vertices.size() < 2) {
            cerr << "Ajustando subgrafo com menos de dois vértices na fase final.\n";
            for (Node* extra_node : nodes) {
                if (visited.find(extra_node->_id) == visited.end() && verifica_conexo(subgraph.vertices, extra_node->_id)) {
                    subgraph.vertices.push_back(extra_node->_id);
                    visited.insert(extra_node->_id);
                    if (subgraph.vertices.size() >= 2) {
                        break;
                    }
                }
            }
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



/// GULOSO RANDOMIZADO ADAPTATIVO REATIVO 
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

            // Verificação se o subgrafo contém pelo menos dois vértices
            if (current_subgraph.vertices.size() < 2) {
                cerr << "O subgrafo gerado contém menos de dois vértices. Ajustando...\n";
                
                // Procurar e adicionar vértices não visitados adjacentes
                for (Node* extra_node : nodes) {
                    if (visited.find(extra_node->_id) == visited.end() && 
                        verifica_conexo(current_subgraph.vertices, extra_node->_id)) {
                        current_subgraph.vertices.push_back(extra_node->_id);
                        visited.insert(extra_node->_id);
                        if (current_subgraph.vertices.size() >= 2) {
                            break;
                        }
                    }
                }

                // Se ainda não temos dois vértices, houve uma falha no ajuste
                if (current_subgraph.vertices.size() < 2) {
                    cerr << "Não foi possível encontrar vértices suficientes para o subgrafo.\n";
                    return -1;
                }
            }

            for (size_t vertex_id : current_subgraph.vertices) {
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
            if (isnan(subgraph_gap) || isinf(subgraph_gap)) {
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
            best_subgraphs = subgraphs; 
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
        if (isnan(subgraph_gap) || isinf(subgraph_gap)) {
            cerr << "Erro: gap inválido calculado para o subgrafo " << i + 1 << ".\n";
            return -1;
        }
        cout << ") - Gap: " << subgraph_gap << endl;
    }
    cout << "Gap total final: " << total_gap << endl;

    return total_gap;
}

