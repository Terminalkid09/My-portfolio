 #include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <functional>
using namespace std;

struct Carta {
    string nome;
    int valore;
};

Carta pescaCarta() {
    int n = rand() % 13 + 1;
    if (n == 1) return {"A", 11};
    else if (n == 11) return {"J", 10};
    else if (n == 12) return {"Q", 10};
    else if (n == 13) return {"K", 10};
    else return {to_string(n), n};
}

int sommaMano(const vector<Carta>& mano) {
    int somma = 0, assi = 0;
    for (const auto& c : mano) {
        somma += c.valore;
        if (c.nome == "A") assi++;
    }
    while (somma > 21 && assi > 0) {
        somma -= 10;
        assi--;
    }
    return somma;
}

void mostraMano(string nome, const vector<Carta>& mano) {
    cout << nome << ": ";
    for (const auto& c : mano) cout << c.nome << " ";
    cout << "(Totale: " << sommaMano(mano) << ")" << endl;
}

// Funzione ausiliaria: turno del giocatore (spostata fuori da main per compatibilità)
void turnoGiocatoreFun(vector<Carta>& mano) {
    char scelta;
    do {
        if (sommaMano(mano) >= 21) break;
        cout << "Vuoi pescare un'altra carta? (s/n): ";
        cin >> scelta;
        if (scelta == 's') {
            mano.push_back(pescaCarta());
            mostraMano("Giocatore", mano);
        }
    } while (scelta == 's');
}

// Funzione per valutare il risultato tra una mano del giocatore e il banco
double valutaRisultatoFun(const vector<Carta>& mano, const vector<Carta>& banco, double puntata) {
    int totG = sommaMano(mano), totB = sommaMano(banco);
    if (totG > 21) return -puntata;
    else if (totB > 21 || totG > totB) return puntata;
    else if (totG == totB) return 0.0;
    else return -puntata;
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    double saldo = 100;
    char continua;

    do {
        if (saldo <= 0) {
            cout << "Hai finito i soldi! Fine gioco.\n";
            break;
        }

        double puntata;
        cout << "\nSaldo attuale: €" << saldo << "\nQuanto vuoi puntare? ";
        cin >> puntata;
        if (puntata > saldo || puntata <= 0) {
            cout << "Puntata non valida.\n";
            continue;
        }

        // Inizializza mani e variabili
        vector<Carta> giocatore;
        vector<Carta> mano2;
        vector<Carta> banco;
        bool split = false;

        // Distribuzione iniziale: due carte al giocatore e due al banco
        giocatore.push_back(pescaCarta());
        giocatore.push_back(pescaCarta());
        banco.push_back(pescaCarta());
        banco.push_back(pescaCarta());

        // Mostra mano giocatore e una carta del banco
        mostraMano("Giocatore", giocatore);
        cout << "Banco: " << banco[0].nome << " ?" << " (seconda carta nascosta)\n";

        // Permetti lo split solo se le prime due carte del giocatore hanno lo stesso valore/nome
        if (giocatore.size() == 2 && giocatore[0].nome == giocatore[1].nome) {
            cout << "Vuoi fare lo split? (s/n): ";
            char s; cin >> s;
            if (s == 's') {
                split = true;
                // crea seconda mano spostando la seconda carta
                mano2.push_back(giocatore[1]);
                giocatore.pop_back();
                // pesca una carta di rimpiazzo per ciascuna mano
                giocatore.push_back(pescaCarta());
                mano2.push_back(pescaCarta());
            }
        }

        // Turno giocatore per la prima mano
        cout << "\n--- Prima mano ---\n";
        mostraMano("Giocatore", giocatore);
        turnoGiocatoreFun(giocatore);

        // Se split è stato effettuato, gioca la seconda mano
        if (split) {
            cout << "\n--- Seconda mano (split) ---\n";
            mostraMano("Seconda mano", mano2);
            turnoGiocatoreFun(mano2);
        }

        // Turno banco: pesca fino a 17 o più
        while (sommaMano(banco) < 17) {
            banco.push_back(pescaCarta());
        }
        cout << "\n--- Banco ---\n";
        mostraMano("Banco", banco);

        // Valuta risultati e aggiorna il saldo
        double risultato = valutaRisultatoFun(giocatore, banco, puntata);
        saldo += risultato;
        cout << "Risultato mano: " << risultato << "  Saldo: €" << saldo << endl;

        if (split) {
            double risultato2 = valutaRisultatoFun(mano2, banco, puntata);
            saldo += risultato2;
            cout << "Risultato seconda mano: " << risultato2 << "  Saldo: €" << saldo << endl;
        }

        // Chiediamo se continuare la partita
        cout << "Vuoi giocare ancora? (s/n): ";
        cin >> continua;
    } while (continua == 's' || continua == 'S');

    return 0;
}
