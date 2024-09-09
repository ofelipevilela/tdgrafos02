#include <iostream>
#include <fstream>
#include <sstream>
#include "include/Graph.hpp"

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
    std::getline(instance, line); // p :=
    std::istringstream param_stream(line);
    std::string param_label;
    param_stream >> param_label >> _num_clusters;

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
}

int Graph::conected(size_t node_id_1, size_t node_id_2) {
    return 0; // Implementação simplificada
}
