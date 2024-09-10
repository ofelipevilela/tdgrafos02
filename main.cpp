#include "include/Graph.hpp"
#include <iostream>
#include <fstream>
#include <chrono> // Para medir o tempo
void showMenu() {
    std::cout << "\nMenu:\n";
    std::cout << "1. Guloso\n";
    std::cout << "2. Guloso randomizado\n";
    std::cout << "3. Guloso randomizado\n";
    std::cout << "4. Imprimir grafo\n"; // Nova opção para imprimir o grafo
    std::cout << "0. Sair\n";
    std::cout << "Escolha uma opcao: ";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso incorreto! Uso correto: " << argv[0] << " <input_file>\n";
        return 1;
    }

    const char* input_file_name = argv[1];

    std::ifstream input_file(input_file_name);
    if (!input_file) {
        std::cerr << "Erro ao abrir o arquivo de entrada: " << input_file_name << "\n";
        return 1;
    }

    // Passa o arquivo para o construtor de Graph
    Graph graph(input_file);
    input_file.close();

    int option;
    do {
        showMenu();
        std::cin >> option;

        switch(option) {
            case 1: {
                // Fecho transitivo direto de um vértice
                size_t p = graph._num_clusters; // Número de subgrafos
                auto start = std::chrono::high_resolution_clock::now();
                float total_gap = graph.greedy_partition(p);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = end - start;
                
                std::cout << "Gap total: " << total_gap << std::endl;
                std::cout << "Tempo de execução: " << elapsed.count() << " segundos\n";
                break;
            }
            case 2: {
                std::cout << "Opção 2 selecionada\n";
                break;
            }
            case 3: {
                std::cout << "Opção 3 selecionada\n";
                break;
            }
            case 4: {
                // Chama a função para imprimir o grafo
                graph.print_graph();
                break;
            }
            case 0: {
                std::cout << "Saindo...\n";
                break;
            }
            default:
                std::cout << "Opção inválida!\n";
                break;
        }

    } while (option != 0);

    return 0;
}