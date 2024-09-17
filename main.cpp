#include "include/Graph.hpp"
#include "include/defines.hpp"

using namespace std;
void showMenu() {
    cout << "\nMenu:\n";
    cout << "1. Guloso\n";
    cout << "2. Guloso randomizado adaptativo\n";
    cout << "3. Guloso randomizado adaptativo reativo\n";
    cout << "4. Imprimir grafo\n"; // Nova opção para imprimir o grafo
    cout << "0. Sair\n";
    cout << "Escolha uma opcao: ";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Uso incorreto! Uso correto: " << argv[0] << " <input_file>\n";
        return 1;
    }

    const char* input_file_name = argv[1];

    ifstream input_file(input_file_name);
    if (!input_file) {
        cerr << "Erro ao abrir o arquivo de entrada: " << input_file_name << "\n";
        return 1;
    }

    // Passa o arquivo para o construtor de Graph
    Graph graph(input_file);
    input_file.close();

    int option;
    do {
        showMenu();
        cin >> option;

        switch(option) {
            case 1: {
                size_t p = graph._num_clusters;
                auto start = chrono::high_resolution_clock::now();
                float total_gap = graph.guloso(p);
                auto end = chrono::high_resolution_clock::now();
                chrono::duration<double> elapsed = end - start;
                cout << "Tempo de execução (Guloso): " << elapsed.count() << " segundos\n";
                break;
            }
            case 2: {
                cout << "Opção 2 selecionada\n";
                break;
            }
            case 3: {
                cout << "Opção 3 selecionada\n";
                break;
            }
            case 4: {
                // Chama a função para imprimir o grafo
                graph.print_graph();
                break;
            }
            case 0: {
                cout << "Saindo...\n";
                break;
            }
            default:
                cout << "Opção inválida!\n";
                break;
        }

    } while (option != 0);

    return 0;
}