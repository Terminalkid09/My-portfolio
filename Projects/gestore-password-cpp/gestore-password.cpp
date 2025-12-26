#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <random>
#include <limits>

using namespace std;

struct PasswordEntry {
    string sito;
    string username;
    string password;
};

vector<PasswordEntry> passwordList;
string masterPassword = "";
const string FILE_NAME = "passwords.dat";
const unsigned char XOR_KEY = 0xAA; // chiave base per criptazione XOR

// Cripta/Decripta (XOR è simmetrico)
string xorCrypt(const string& input) {
    string output = input;
    for (size_t i = 0; i < output.size(); ++i) {
        output[i] = static_cast<char>(static_cast<unsigned char>(output[i]) ^ XOR_KEY);
    }
    return output;
}

// Salva su file: compone tutto il testo, cifra l'intero buffer e scrive
void saveToFile() {
    ostringstream oss;
    for (const auto& e : passwordList) {
        oss << e.sito << '|' << e.username << '|' << e.password << '\n';
    }
    string plaintext = oss.str();
    string encrypted = xorCrypt(plaintext);

    ofstream file(FILE_NAME, ios::binary);
    if (!file) {
        cout << "Errore salvataggio!\n";
        return;
    }
    file.write(encrypted.data(), static_cast<streamsize>(encrypted.size()));
    file.close();
    cout << "Password salvate con successo!\n";
}

// Carica da file: legge tutto, decripta e divide per righe
void loadFromFile() {
    ifstream file(FILE_NAME, ios::binary);
    if (!file) return; // file inesistente -> niente da caricare

    string encrypted((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    if (encrypted.empty()) return;

    string decrypted = xorCrypt(encrypted);
    istringstream iss(decrypted);
    string line;
    passwordList.clear();
    while (std::getline(iss, line)) {
        if (line.empty()) continue;
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        if (p1 != string::npos && p2 != string::npos) {
            PasswordEntry e;
            e.sito = line.substr(0, p1);
            e.username = line.substr(p1 + 1, p2 - p1 - 1);
            e.password = line.substr(p2 + 1);
            passwordList.push_back(std::move(e));
        }
    }
}

// Generatore di password usando <random> (più sicuro di rand())
string generatePassword(int length = 12) {
    const string chars = "abcdefghijklmnopqrstuvwxyz"
                         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                         "0123456789"
                         "!@#$%^&*+.";
    static thread_local std::mt19937_64 rng((std::random_device())());
    uniform_int_distribution<size_t> dist(0, chars.size() - 1);

    string pass;
    pass.reserve(length);
    for (int i = 0; i < length; ++i) {
        pass += chars[dist(rng)];
    }
    return pass;
}

string hidePassword(const string& pass) {
    return string(pass.length(), '*');
}

void addPassword() {
    PasswordEntry entry;
    cout << "Sito/App: ";
    getline(cin, entry.sito);
    cout << "Username/Email: ";
    getline(cin, entry.username);
    cout << "Password (lascia vuoto per generare): ";
    getline(cin, entry.password);
    if (entry.password.empty()) {
        entry.password = generatePassword();
        cout << "Password generata: " << entry.password << "\n";
    }
    passwordList.push_back(entry);
    saveToFile();
}

void listPasswords(bool showIndex = false) {
    if (passwordList.empty()) {
        cout << "Nessuna password salvata.\n";
        return;
    }
    cout << "\n=== Le tue password ===\n";
    for (size_t i = 0; i < passwordList.size(); i++) {
        cout << (showIndex ? to_string(i + 1) + ". " : "") 
             << passwordList[i].sito << " | "
             << passwordList[i].username << " | "
             << hidePassword(passwordList[i].password)
             << "\n";
    }
}

void listAndShowPassword() {
    listPasswords(true);
    cout << "\nInserisci numero per mostrare password (0 per tornare): ";
    int choice;
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (choice > 0 && choice <= (int)passwordList.size()) {
        cout << "Password: " << passwordList[choice - 1].password << "\n";
    }
}

// Modifica entry esistente
void editPassword() {
    if (passwordList.empty()) {
        cout << "Nessuna password da modificare.\n";
        return;
    }
    listPasswords(true);
    cout << "\nInserisci numero della entry da modificare (0 per tornare): ";
    int idx;
    if (!(cin >> idx)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Input non valido.\n";
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (idx <= 0 || idx > (int)passwordList.size()) {
        cout << "Indice non valido.\n";
        return;
    }
    PasswordEntry &entry = passwordList[idx - 1];
    cout << "Modifica entry: " << entry.sito << " | " << entry.username << "\n";
    cout << "Lascia vuoto per mantenere il valore attuale.\n";

    cout << "Nuovo Sito/App (attuale: " << entry.sito << "): ";
    string nuovoSito;
    getline(cin, nuovoSito);
    if (!nuovoSito.empty()) entry.sito = nuovoSito;

    cout << "Nuovo Username/Email (attuale: " << entry.username << "): ";
    string nuovoUser;
    getline(cin, nuovoUser);
    if (!nuovoUser.empty()) entry.username = nuovoUser;

    cout << "Nuova Password (lascia vuoto per mantenere, digita 'g' per generare): ";
    string nuovoPass;
    getline(cin, nuovoPass);
    if (nuovoPass == "g" || nuovoPass == "G") {
        cout << "Lunghezza password da generare (es. 12): ";
        int len;
        if (!(cin >> len)) len = 12;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        entry.password = generatePassword(len);
        cout << "Password generata: " << entry.password << "\n";
    } else if (!nuovoPass.empty()) {
        entry.password = nuovoPass;
    }

    saveToFile();
    cout << "Entry modificata correttamente.\n";
}

// Cancella entry esistente
void deletePassword() {
    if (passwordList.empty()) {
        cout << "Nessuna password da cancellare.\n";
        return;
    }
    listPasswords(true);
    cout << "\nInserisci numero della entry da cancellare (0 per tornare): ";
    int idx;
    if (!(cin >> idx)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Input non valido.\n";
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (idx <= 0 || idx > (int)passwordList.size()) {
        cout << "Indice non valido.\n";
        return;
    }
    PasswordEntry entry = passwordList[idx - 1];
    cout << "Sei sicuro di voler cancellare: " << entry.sito << " | " << entry.username << " ? (s/n): ";
    char conf;
    if (!(cin >> conf)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Operazione annullata.\n";
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (conf == 's' || conf == 'S' || conf == 'y' || conf == 'Y') {
        passwordList.erase(passwordList.begin() + (idx - 1));
        saveToFile();
        cout << "Entry cancellata.\n";
    } else {
        cout << "Cancellazione annullata.\n";
    }
}

void searchPassword() {
    cout << "Cerca sito: ";
    string search;
    getline(cin, search);
    bool found = false;
    for (const auto& entry : passwordList) {
        if (entry.sito.find(search) != string::npos) {
            cout << entry.sito << " | " << entry.username << " | " << hidePassword(entry.password) << "\n";
            found = true;
        }
    }
    if (!found) cout << "Nessuna password trovata.\n";
}

int main() {
    loadFromFile();

    cout << "=== Password Manager by Terminalkid09 ===\n";
    cout << "Inserisci master password (per ora demo, premi Enter): ";
    getline(cin, masterPassword);

    int choice;
    do {
#ifdef _WIN32
        system("cls");
#else
        cout << "\x1B[2J\x1B[H";
#endif
        cout << "\n1. Aggiungi password\n";
        cout << "2. Lista password (solo vista)\n";
        cout << "3. Mostra password\n";
        cout << "4. Cerca password\n";
        cout << "5. Modifica entry\n";
        cout << "6. Cancella entry\n";
        cout << "7. Genera password forte\n";
        cout << "0. Esci\n";
        cout << "Scelta: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (choice) {
            case 1: addPassword(); break;
            case 2: listPasswords(); break;
            case 3: listAndShowPassword(); break;
            case 4: searchPassword(); break;
            case 5: editPassword(); break;
            case 6: deletePassword(); break;
            case 7: {
                cout << "Lunghezza password desiderata (es. 12): ";
                int len;
                if (!(cin >> len)) len = 12;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Password generata: " << generatePassword(len) << "\n";
                break;
            }
            case 0: cout << "Arrivederci!\n"; break;
            default: cout << "Scelta non valida.\n";
        }

        if (choice != 0) {
#ifdef _WIN32
            system("pause");
#else
            cout << "Premi Invio per continuare...";
            cin.get();
#endif
        }
    } while (choice != 0);

    return 0;
}