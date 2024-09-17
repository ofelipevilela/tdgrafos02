#include "include/Graph.hpp"
#include "include/defines.hpp"
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
    return 0; // Implementação simplificada
}

/// GULOSO

// Função para calcular o gap de um subgrafo
float Graph::gap(const Subgraph& subgraph) {
    if (subgraph.vertices.empty()) {
        return 0;  // Gap é zero se o subgrafo estiver vazio
    }
    // diferença entre o peso máximo e o mínimo
    float gap = subgraph.max_weight - subgraph.min_weight;
    
    // Depuração para garantir que o gap está sendo calculado corretamente
    //cout << "Calculando gap: max = " << subgraph.max_weight << ", min = " << subgraph.min_weight << ", gap = " << gap << endl;

    return gap;
}
// Algoritmo guloso para particionar o grafo e minimizar o gap
float Graph::guloso(size_t p) {
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

    cout << "Iniciando particionamento guloso com " << p << " subgrafos." << endl;

    // Inicializar p subgrafos
    vector<Subgraph> subgraphs(p);

    // Obter os vértices e seus pesos (ordenados por peso decrescente)
    vector<pair<size_t, float>> vertices; // (id, peso)
    Node* current_node = _first;

    if (current_node == nullptr) {
        cerr << "Erro: Nenhum nó encontrado no grafo!" << endl;
        return -1;
    }

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
    cout << "Atribuindo os " << _num_clusters << " maiores vértices aos subgrafos." << endl;
    for (size_t i = 0; i < p; ++i) {
        size_t vertex_id = vertices[i].first;
        float vertex_weight = vertices[i].second;
        subgraphs[i].vertices.push_back(vertex_id);
        subgraphs[i].max_weight = subgraphs[i].min_weight = vertex_weight;
        cout << "Vértice " << vertex_id << " atribuído ao subgrafo " << i + 1 << " com peso " << vertex_weight << "." << endl;
    }

    // Atribuir os vértices restantes
    cout << "Atribuindo os vértices restantes..." << endl;
    for (size_t i = p; i < vertices.size(); ++i) {
        size_t vertex_id = vertices[i].first;
        float vertex_weight = vertices[i].second;

        size_t best_subgraph = 0;
        float best_gap_increase = numeric_limits<float>::max();

        // Encontrar o subgrafo que minimiza o aumento do gap
        for (size_t j = 0; j < p; ++j) {
            float new_max_weight = max(subgraphs[j].max_weight, vertex_weight);
            float new_min_weight = min(subgraphs[j].min_weight, vertex_weight);
            float gap_increase = (new_max_weight - new_min_weight) - gap(subgraphs[j]);

            if (gap_increase < best_gap_increase) {
                best_gap_increase = gap_increase;
                best_subgraph = j;
            }
        }

        // Adicionar o vértice ao melhor subgrafo
        subgraphs[best_subgraph].vertices.push_back(vertex_id);
        subgraphs[best_subgraph].max_weight = max(subgraphs[best_subgraph].max_weight, vertex_weight);
        subgraphs[best_subgraph].min_weight = min(subgraphs[best_subgraph].min_weight, vertex_weight);
        //cout << "Vértice " << vertex_id << " atribuído ao subgrafo " << best_subgraph << " com peso " << vertex_weight << "." << endl;
    }

    // Calcular o gap total
    float total_gap = 0;
    cout << "Calculando o gap total..." << endl;
    for (const auto& subgraph : subgraphs) {
        total_gap += gap(subgraph);
    }

    cout << "Gap total calculado: " << total_gap << endl;
    return total_gap;

}