#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "Node.hpp"
#include "defines.hpp"

using namespace std;

// Declaração da estrutura Subgraph
struct Subgraph {
    vector<size_t> vertices; // Vértices no subgrafo
    float max_weight;             // Maior peso no subgrafo
    float min_weight;             // Menor peso no subgrafo
    float total_weight;

    Subgraph() : max_weight(0), min_weight(0) {}
};


class Graph
{
public:
    /*Assinatura dos métodos básicos para o funcionamento da classe*/

    Graph(ifstream& instance);
    Graph();
    ~Graph();
    size_t _num_clusters; 
    void remove_node(size_t node_id);
    void remove_edge(size_t node_id_1, size_t node_id_2);
    void add_node(size_t node_id, float weight = 0);
    void add_edge(size_t node_id_1, size_t node_id_2, float weight = 0);
    void print_graph(ofstream& output_file);
    void print_graph();
    int conected(size_t node_id_1, size_t node_id_2);
    // Funcoes do problema
    float gap(const Subgraph& subgraph);
    Node* find_node(size_t id);
    float guloso(size_t p);
    float guloso_randomizado_adaptativo(size_t p, float alpha);
    float guloso_randomizado_adaptativo_reativo(size_t p, size_t max_iter);
    bool check_connected(const vector<size_t>& vertices);
    bool is_connected_incremental(const vector<size_t>& vertices, size_t new_vertex);

private:
    size_t _number_of_nodes;
    size_t _number_of_edges;
    bool   _directed;
    bool   _weighted_edges;
    bool   _weighted_nodes;
    Node  *_first;
    Node  *_last;
    // Adicione um vetor para armazenar os subgrafos
    vector<Subgraph> subgraphs;
};

#endif  //GRAPH_HPP