#include <iostream>
#include <fstream>
#include <sstream>
#include "include/Graph.hpp"
#include <chrono> // Para medir o tempo
#include <random>
#include <cmath>

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

// Construtor que lê o arquivo e constrói o grafo
Graph::Graph(std::ifstream& instance) : _first(nullptr) {
    std::string line;
    // Procurar pela linha com "param p :="
    while (std::getline(instance, line)) {
        if (line.find("param p") != std::string::npos) {
            // Ignora "param p :=" e lê o número diretamente
            std::istringstream param_stream(line);
            std::string ignore;
            param_stream >> ignore >> ignore >> ignore >> _num_clusters;
            break; // Já encontramos o número de partições, então podemos sair
        }
    }

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

/// GULOSO
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
    std::cout << "Atribuindo os" << _num_clusters << " maiores vértices aos subgrafos." << std::endl;
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
        //std::cout << "Vértice " << vertex_id << " atribuído ao subgrafo " << best_subgraph << " com peso " << vertex_weight << "." << std::endl;
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

/// ALNS
void Graph::alns_optimization() {
    // Exemplo básico de uma abordagem ALNS
    std::cout << "Iniciando otimização ALNS...\n";

    // Passo 1: Destruir solução atual (exemplo)
    destroy_solution();

    // Passo 2: Reparar a solução (exemplo)
    repair_solution();

    // Passo 3: Perturbar a solução (exemplo)
    perturb_solution();

    // Você pode adicionar mais etapas e refinamentos aqui
    std::cout << "Otimização ALNS concluída.\n";
}

void Graph::destroy_solution() {
    std::cout << "Destruindo a solução atual...\n";

    // Resetar os subgrafos
    // Esta função deve reverter ou desconectar a solução atual
    // Por simplicidade, vamos apenas redefinir a estrutura dos subgrafos

    // Exemplo de redefinir todas as arestas (isto é uma simplificação)
    Node* node = _first;
    while (node) {
        Edge* edge = node->_first_edge;
        while (edge) {
            Edge* next_edge = edge->_next_edge;
            delete edge; // Libera a memória da aresta
            edge = next_edge;
        }
        node->_first_edge = nullptr; // Remove as arestas do nó
        node = node->_next_node;
    }

    std::cout << "Solução destruída.\n";
}

void Graph::repair_solution() {
    std::cout << "Reparando a solução...\n";

    // Reconstruir arestas e subgrafos (exemplo básico)
    // Isso pode incluir a reinicialização dos subgrafos ou reatribuição de arestas

    // Exemplo de adicionar arestas (simplificado, deve ser ajustado conforme a lógica do problema)
    Node* node = _first;
    while (node) {
        // Adicionar arestas fictícias ou reconectar nós se necessário
        // Implementar lógica específica de reparo aqui
        node = node->_next_node;
    }

    std::cout << "Solução reparada.\n";
}

#include <random>

void Graph::perturb_solution() {
    std::cout << "Perturbando a solução...\n";

    // Usar um gerador de números aleatórios
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);

    // Exemplo de perturbação: Adicionar arestas aleatórias
    Node* node = _first;
    while (node) {
        // Decidir aleatoriamente adicionar uma nova aresta
        if (dis(gen) < 0.1) { // Probabilidade de adicionar uma aresta
            Node* target_node = _first;
            while (target_node && target_node->_id == node->_id) {
                target_node = target_node->_next_node;
            }

            if (target_node) {
                float random_weight = dis(gen) * 10; // Peso aleatório
                add_edge(node->_id, target_node->_id, random_weight);
                std::cout << "Aresta adicionada entre " << node->_id << " e " << target_node->_id << " com peso " << random_weight << ".\n";
            }
        }

        node = node->_next_node;
    }

    std::cout << "Solução perturbada.\n";
}

void Graph::print_alns_result() {
    std::cout << "Resultado do ALNS:\n";

    Node* node = _first;
    while (node) {
        std::cout << "Vértice: " << node->_id << " Peso: " << node->_weight << "\n";
        Edge* edge = node->_first_edge;
        while (edge) {
            std::cout << "  Aresta: (" << node->_id << ", " << edge->_target_id << ") Peso: " << edge->_weight << "\n";
            edge = edge->_next_edge;
        }
        node = node->_next_node;
    }
    std::cout <<"O número de partições é: " << _num_clusters << std::endl;
}
