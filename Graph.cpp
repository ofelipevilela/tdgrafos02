#include <iostream>
#include <fstream>
#include <sstream>
#include "include/Graph.hpp"
#include <chrono> // Para medir o tempo

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

// Construtor que lê o arquivo e constrói o grafo
Graph::Graph(std::ifstream& instance) : _first(nullptr) {
    std::string line;

    // Ler o parâmetro p
    std::getline(instance, line); // Ignorar linha do comentário
    std::getline(instance, line); // linha com "param p :="
    
    // Agora, vamos encontrar o número de partições manualmente
    size_t pos_p = line.find("p := ");
    if (pos_p != std::string::npos) {
        // Encontrar o início do número
        std::string num_str = line.substr(pos_p + 5); // Ignora "p :=" e vai direto ao número
        _num_clusters = std::stoi(num_str); // Converte para inteiro
    } else {
        std::cerr << "Erro: Não foi possível encontrar o número de partições no arquivo." << std::endl;
        _num_clusters = 0; // Valor de fallback
    }
    
    std::cout << "Número de partições lido: " << _num_clusters << std::endl;

    // Ler vértices
    std::getline(instance, line); // Ignorar linha de comentário
    std::getline(instance, line); // set V :=
    while (std::getline(instance, line) && line.find(';') == std::string::npos) {
        std::istringstream vertex_stream(line);
        size_t node_id;
        while (vertex_stream >> node_id) {
            add_node(node_id, 0.0f); // Inicialmente, peso 0.0f
        }
    }

    // Ler pesos dos vértices
    std::getline(instance, line); // Ignorar linha de comentário
    std::getline(instance, line); // param w :=
    while (std::getline(instance, line) && line.find(';') == std::string::npos) {
        std::istringstream weight_stream(line);
        size_t node_id;
        float weight;
        if (weight_stream >> node_id >> weight) {
            add_node(node_id, weight); // Atualizar o peso do nó existente
        }
    }

    // Ler arestas
    std::getline(instance, line); // Ignorar linha de comentário
    std::getline(instance, line); // set E :=
    while (std::getline(instance, line) && line.find(';') == std::string::npos) {
        std::stringstream ss(line);
        char ignore;
        size_t node_id_1, node_id_2;

        // Ler arestas no formato (n1, n2)
        while (ss >> ignore >> node_id_1 >> ignore >> node_id_2 >> ignore) {
            add_edge(node_id_1, node_id_2, 0.0f); // Peso padrão 0.0f
        }
    }
}


// Demais métodos da classe Graph
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

// Imprime o grafo, mostrando vértices e arestas no formato (n1, n2)
void Graph::print_graph() {
    Node* node = _first;
    while (node) {
        std::cout << "Vertice: " << node->_id << " Peso: " << node->_weight << "\n";
        Edge* edge = node->_first_edge;
        while (edge) {
            std::cout << "  Aresta: (" << node->_id << ", " << edge->_target_id << ")\n";
            edge = edge->_next_edge;
        }
        node = node->_next_node;
    }
    // mensagem de depuracao para verificar p numero de subgrafos obtido na leitura
    std::cout <<"O numero de particoes eh: " << _num_clusters << std::endl;
}

int Graph::conected(size_t node_id_1, size_t node_id_2) {
    return 0; // Implementação simplificada
}


// Função para calcular o gap de um subgrafo
float Graph::calculate_gap(const Subgraph& subgraph) {
    return subgraph.max_weight - subgraph.min_weight;
}

// Algoritmo guloso para particionar o grafo e minimizar o gap
float Graph::greedy_partition(size_t p) {
    // Verificar se o número de subgrafos é válido
    if (p <= 1 || p > _number_of_nodes) {
        std::cerr << "Erro: número inválido de subgrafos (" << p << "). Deve ser maior que 1 e menor ou igual ao número de vértices." << std::endl;
        return -1;
    }

    // Verifique se o grafo tem nós suficientes para particionar
    if (_first == nullptr) {
        std::cerr << "Erro: Grafo vazio!" << std::endl;
        return -1;
    }

    std::cout << "Iniciando particionamento guloso com " << p << " subgrafos." << std::endl;

    // Inicializar p subgrafos
    std::vector<Subgraph> subgraphs(p);

    // Obter os vértices e seus pesos (ordenados por peso decrescente)
    std::vector<std::pair<size_t, float>> vertices; // (id, peso)
    Node* current_node = _first;

    if (current_node == nullptr) {
        std::cerr << "Erro: Nenhum nó encontrado no grafo!" << std::endl;
        return -1;
    }

    // Adicionando os vértices ao vetor de vértices
    while (current_node) {
        vertices.push_back({current_node->_id, current_node->_weight});
        current_node = current_node->_next_node;
    }

    // Verificar se há vértices suficientes para os subgrafos
    if (vertices.size() < p) {
        std::cerr << "Erro: Número insuficiente de vértices para " << p << " subgrafos!" << std::endl;
        return -1;
    }

    // Ordenar os vértices por peso (maior para menor)
    std::sort(vertices.begin(), vertices.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    // Atribuir os p maiores vértices aos subgrafos
    std::cout << "Atribuindo os p maiores vértices aos subgrafos." << std::endl;
    for (size_t i = 0; i < p; ++i) {
        size_t vertex_id = vertices[i].first;
        float vertex_weight = vertices[i].second;
        subgraphs[i].vertices.push_back(vertex_id);
        subgraphs[i].max_weight = subgraphs[i].min_weight = vertex_weight;
        std::cout << "Vértice " << vertex_id << " atribuído ao subgrafo " << i << " com peso " << vertex_weight << "." << std::endl;
    }

    // Atribuir os vértices restantes
    std::cout << "Atribuindo os vértices restantes." << std::endl;
    for (size_t i = p; i < vertices.size(); ++i) {
        size_t vertex_id = vertices[i].first;
        float vertex_weight = vertices[i].second;

        size_t best_subgraph = 0;
        float best_gap_increase = std::numeric_limits<float>::max();

        // Encontrar o subgrafo que minimiza o aumento do gap
        for (size_t j = 0; j < p; ++j) {
            float new_max_weight = std::max(subgraphs[j].max_weight, vertex_weight);
            float new_min_weight = std::min(subgraphs[j].min_weight, vertex_weight);
            float gap_increase = (new_max_weight - new_min_weight) - calculate_gap(subgraphs[j]);

            if (gap_increase < best_gap_increase) {
                best_gap_increase = gap_increase;
                best_subgraph = j;
            }
        }

        // Adicionar o vértice ao melhor subgrafo
        subgraphs[best_subgraph].vertices.push_back(vertex_id);
        subgraphs[best_subgraph].max_weight = std::max(subgraphs[best_subgraph].max_weight, vertex_weight);
        subgraphs[best_subgraph].min_weight = std::min(subgraphs[best_subgraph].min_weight, vertex_weight);
        std::cout << "Vértice " << vertex_id << " atribuído ao subgrafo " << best_subgraph << " com peso " << vertex_weight << "." << std::endl;
    }

    // Calcular o gap total
    float total_gap = 0;
    std::cout << "Calculando o gap total." << std::endl;
    for (const auto& subgraph : subgraphs) {
        total_gap += calculate_gap(subgraph);
    }

    std::cout << "Gap total calculado: " << total_gap << std::endl;
    return total_gap;
}