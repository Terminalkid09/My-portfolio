#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

struct Libro {
    string titolo;
    string autore;
    int anno;
};

void aggiungiLibro(vector<Libro>& biblioteca);
void mostraLibri(const vector<Libro>& biblioteca);
void cercaLibro(const vector<Libro>& biblioteca);
void rimuoviLibro(vector<Libro>& biblioteca);
void salvaSuFile(const vector<Libro>& biblioteca);
void caricaDaFile(vector<Libro>& biblioteca);

int main() {
    vector<Libro> biblioteca;
    int scelta;

    caricaDaFile(biblioteca);

    do {
        cout << "\n--- MENU BIBLIOTECA ---\n";
        cout << "1. Aggiungi libro\n";
        cout << "2. Mostra tutti i libri\n";
        cout << "3. Cerca libro per titolo\n";
        cout << "4. Rimuovi libro\n";
        cout << "5. Salva su file\n";
        cout << "6. Esci\n";
        cout << "Scelta: ";
        cin >> scelta;
        cin.ignore();

        switch (scelta) {
            case 1: aggiungiLibro(biblioteca); break;
            case 2: mostraLibri(biblioteca); break;
            case 3: cercaLibro(biblioteca); break;
            case 4: rimuoviLibro(biblioteca); break;
            case 5: salvaSuFile(biblioteca); break;
            case 6: cout << "Uscita...\n"; break;
            default: cout << "Scelta non valida!\n";
        }
    } while (scelta != 6);

    return 0;
}

void aggiungiLibro(vector<Libro>& biblioteca) {
    Libro l;
    cout << "Titolo: ";
    getline(cin, l.titolo);
    cout << "Autore: ";
    getline(cin, l.autore);
    cout << "Anno: ";
    cin >> l.anno;
    cin.ignore();
    biblioteca.push_back(l);
    cout << "Libro aggiunto.\n";
}

void mostraLibri(const vector<Libro>& biblioteca) {
    if (biblioteca.empty()) {
        cout << "Nessun libro nella biblioteca.\n";
        return;
    }
    int i = 1;
    for (const auto& libro : biblioteca) {
        cout << i++ << ". " << libro.titolo << " - " << libro.autore << " (" << libro.anno << ")\n";
    }
}

void cercaLibro(const vector<Libro>& biblioteca) {
    if (biblioteca.empty()) {
        cout << "Nessun libro nella biblioteca.\n";
        return;
    }
    string titolo;
    cout << "Titolo da cercare: ";
    getline(cin, titolo);
    bool trovato = false;
    for (const auto& libro : biblioteca) {
        if (libro.titolo.find(titolo) != string::npos) {
            cout << libro.titolo << " - " << libro.autore << " (" << libro.anno << ")\n";
            trovato = true;
        }
    }
    if (!trovato) cout << "Nessun libro trovato con quel titolo.\n";
}

void rimuoviLibro(vector<Libro>& biblioteca) {
    mostraLibri(biblioteca);
    if (biblioteca.empty()) return;

    int indice;
    cout << "Numero del libro da rimuovere: ";
    cin >> indice;
    cin.ignore();

    if (indice >= 1 && indice <= (int)biblioteca.size()) {
        biblioteca.erase(biblioteca.begin() + indice - 1);
        cout << "Libro rimosso.\n";
    } else {
        cout << "Indice non valido.\n";
    }
}

void salvaSuFile(const vector<Libro>& biblioteca) {
    ofstream file("libri.txt");
    for (const auto& libro : biblioteca) {
        file << libro.titolo << "|" << libro.autore << "|" << libro.anno << "\n";
    }
    file.close();
    cout << "Libri salvati su 'libri.txt'.\n";
}

void caricaDaFile(vector<Libro>& biblioteca) {
    ifstream file("libri.txt");
    if (!file) return;

    string linea;
    while (getline(file, linea)) {
        size_t pos1 = linea.find('|');
        size_t pos2 = linea.rfind('|');

        if (pos1 != string::npos && pos2 != string::npos && pos1 != pos2) {
            Libro l;
            l.titolo = linea.substr(0, pos1);
             l.autore = linea.substr(pos1 + 1, pos2 - pos1 - 1);
            l.anno = stoi(linea.substr(pos2 + 1));
            biblioteca.push_back(l);
        }
    }

    file.close();
}

