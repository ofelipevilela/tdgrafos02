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
                size_t p = graph._num_clusters;
                float alpha;
                cout << "Quanto menor o alpha, mais guloso o algoritmo se torna; quanto maior, mais aleatoriedade." << endl;
                //cout << "Digite o valor de alpha (0 <= alpha <= 1): ";
                //cout << "Vamos considerar alpha = 0.5 " << endl;
                //cin >> alpha;
                auto start = chrono::high_resolution_clock::now();
                float total_gap = graph.guloso_randomizado_adaptativo(p, 0.5);
                auto end = chrono::high_resolution_clock::now();
                chrono::duration<double> elapsed = end - start;
                cout << "Tempo de execução (Guloso Randomizado Adaptativo): " << elapsed.count() << " segundos\n";
                break;
            }
            case 3: {
                size_t p = graph._num_clusters;
                size_t max_iter;
                cout << "Digite o número de iterações para o algoritmo reativo: ";
                cin >> max_iter;
                auto start = chrono::high_resolution_clock::now();
                float total_gap = graph.guloso_randomizado_adaptativo_reativo(p, max_iter);
                auto end = chrono::high_resolution_clock::now();
                chrono::duration<double> elapsed = end - start;
                cout << "Gap total (Guloso Randomizado Adaptativo Reativo): " << total_gap << endl;
                cout << "Tempo de execução (Guloso Randomizado Adaptativo Reativo): " << elapsed.count() << " segundos\n";
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