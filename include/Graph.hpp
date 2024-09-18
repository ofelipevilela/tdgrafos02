#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "Node.hpp"
#include "defines.hpp"
#include <vector>

// Declaração da estrutura Subgraph
struct Subgraph {
    std::vector<size_t> vertices; // Vértices no subgrafo
    float max_weight;             // Maior peso no subgrafo
    float min_weight;             // Menor peso no subgrafo

    Subgraph() : max_weight(0), min_weight(0) {}
};


class Graph
{
public:
    /*Assinatura dos métodos básicos para o funcionamento da classe*/

    Graph(std::ifstream& instance);
    Graph();
    ~Graph();

    void remove_node(size_t node_id);
    void remove_edge(size_t node_id_1, size_t node_id_2);
    void add_node(size_t node_id, float weight = 0);
    void add_edge(size_t node_id_1, size_t node_id_2, float weight = 0);
    void print_graph(std::ofstream& output_file);
    void print_graph();

    int conected(size_t node_id_1, size_t node_id_2);

    size_t _num_clusters; // Adiciona esta linha para armazenar o número de clusters
    float gap(const Subgraph& subgraph);
    float guloso(size_t p);
    
    bool are_connected(size_t vertex1, size_t vertex2);
    float guloso_randomizado_adaptativo(size_t p, float alpha);
    float guloso_randomizado_adaptativo_reativo(size_t p, size_t max_iter);
    bool check_connected(const std::vector<size_t>& vertices); // Certifique-se de usar std::vector

private:
    size_t _number_of_nodes;
    size_t _number_of_edges;
    bool   _directed;
    bool   _weighted_edges;
    bool   _weighted_nodes;
    Node  *_first;
    Node  *_last;
};

#endif  //GRAPH_HPP