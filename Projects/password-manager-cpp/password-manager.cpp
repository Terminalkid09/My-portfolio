#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <random>
#include <limits>

using namespace std;

struct PasswordEntry {
    string site;
    string username;
    string password;
};

vector<PasswordEntry> passwordList;
string masterPassword = "";
const string FILE_NAME = "passwords.dat";
const unsigned char XOR_KEY = 0xAA; // base key for XOR encryption

// Encrypt/Decrypt (XOR is symmetric)
string xorCrypt(const string& input) {
    string output = input;
    for (size_t i = 0; i < output.size(); ++i) {
        output[i] = static_cast<char>(static_cast<unsigned char>(output[i]) ^ XOR_KEY);
    }
    return output;
}

// Save to file: compose the whole text, encrypt the buffer and write it
void saveToFile() {
    ostringstream oss;
    for (const auto& e : passwordList) {
        oss << e.site << '|' << e.username << '|' << e.password << '\n';
    }
    string plaintext = oss.str();
    string encrypted = xorCrypt(plaintext);

    ofstream file(FILE_NAME, ios::binary);
    if (!file) {
        cout << "Error saving!\n";
        return;
    }
    file.write(encrypted.data(), static_cast<streamsize>(encrypted.size()));
    file.close();
    cout << "Passwords saved successfully!\n";
}

// Load from file: read everything, decrypt and split into lines
void loadFromFile() {
    ifstream file(FILE_NAME, ios::binary);
    if (!file) return; // no file -> nothing to load

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
            e.site = line.substr(0, p1);
            e.username = line.substr(p1 + 1, p2 - p1 - 1);
            e.password = line.substr(p2 + 1);
            passwordList.push_back(std::move(e));
        }
    }
}

// Password generator using <random>
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
    cout << "Site/App: ";
    getline(cin, entry.site);
    cout << "Username/Email: ";
    getline(cin, entry.username);
    cout << "Password (leave empty to generate): ";
    getline(cin, entry.password);
    if (entry.password.empty()) {
        entry.password = generatePassword();
        cout << "Generated password: " << entry.password << "\n";
    }
    passwordList.push_back(entry);
    saveToFile();
}

void listPasswords(bool showIndex = false) {
    if (passwordList.empty()) {
        cout << "No passwords saved.\n";
        return;
    }
    cout << "\n=== Your passwords ===\n";
    for (size_t i = 0; i < passwordList.size(); i++) {
        cout << (showIndex ? to_string(i + 1) + ". " : "")
             << passwordList[i].site << " | "
             << passwordList[i].username << " | "
             << hidePassword(passwordList[i].password)
             << "\n";
    }
}

void listAndShowPassword() {
    listPasswords(true);
    cout << "\nEnter number to show password (0 to return): ";
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

// Edit an existing entry
void editPassword() {
    if (passwordList.empty()) {
        cout << "No passwords to edit.\n";
        return;
    }
    listPasswords(true);
    cout << "\nEnter entry number to edit (0 to return): ";
    int idx;
    if (!(cin >> idx)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input.\n";
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (idx <= 0 || idx > (int)passwordList.size()) {
        cout << "Invalid index.\n";
        return;
    }
    PasswordEntry &entry = passwordList[idx - 1];
    cout << "Editing entry: " << entry.site << " | " << entry.username << "\n";
    cout << "Leave empty to keep the current value.\n";

    cout << "New Site/App (current: " << entry.site << "): ";
    string newSite;
    getline(cin, newSite);
    if (!newSite.empty()) entry.site = newSite;

    cout << "New Username/Email (current: " << entry.username << "): ";
    string newUser;
    getline(cin, newUser);
    if (!newUser.empty()) entry.username = newUser;

    cout << "New Password (leave empty to keep, type 'g' to generate): ";
    string newPass;
    getline(cin, newPass);
    if (newPass == "g" || newPass == "G") {
        cout << "Password length to generate (e.g. 12): ";
        int len;
        if (!(cin >> len)) len = 12;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        entry.password = generatePassword(len);
        cout << "Generated password: " << entry.password << "\n";
    } else if (!newPass.empty()) {
        entry.password = newPass;
    }

    saveToFile();
    cout << "Entry updated successfully.\n";
}

// Delete an existing entry
void deletePassword() {
    if (passwordList.empty()) {
        cout << "No passwords to delete.\n";
        return;
    }
    listPasswords(true);
    cout << "\nEnter entry number to delete (0 to return): ";
    int idx;
    if (!(cin >> idx)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input.\n";
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (idx <= 0 || idx > (int)passwordList.size()) {
        cout << "Invalid index.\n";
        return;
    }
    PasswordEntry entry = passwordList[idx - 1];
    cout << "Are you sure you want to delete: " << entry.site << " | " << entry.username << " ? (y/n): ";
    char conf;
    if (!(cin >> conf)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Operation cancelled.\n";
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (conf == 'y' || conf == 'Y' || conf == 's' || conf == 'S') { // accept 's' for users who type Italian "s"
        passwordList.erase(passwordList.begin() + (idx - 1));
        saveToFile();
        cout << "Entry deleted.\n";
    } else {
        cout << "Deletion cancelled.\n";
    }
}

void searchPassword() {
    cout << "Search site: ";
    string search;
    getline(cin, search);
    bool found = false;
    for (const auto& entry : passwordList) {
        if (entry.site.find(search) != string::npos) {
            cout << entry.site << " | " << entry.username << " | " << hidePassword(entry.password) << "\n";
            found = true;
        }
    }
    if (!found) cout << "No password found.\n";
}

int main() {
    loadFromFile();

    cout << "=== Password Manager by Terminalkid09 ===\n";
    cout << "Enter master password (demo only, press Enter): ";
    getline(cin, masterPassword);

    int choice;
    do {
#ifdef _WIN32
        system("cls");
#else
        cout << "\x1B[2J\x1B[H";
#endif
        cout << "\n1. Add password\n";
        cout << "2. List passwords (view only)\n";
        cout << "3. Show password\n";
        cout << "4. Search password\n";
        cout << "5. Edit entry\n";
        cout << "6. Delete entry\n";
        cout << "7. Generate strong password\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
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
                cout << "Desired password length (e.g. 12): ";
                int len;
                if (!(cin >> len)) len = 12;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Generated password: " << generatePassword(len) << "\n";
                break;
            }
            case 0: cout << "Goodbye!\n"; break;
            default: cout << "Invalid choice.\n";
        }

        if (choice != 0) {
#ifdef _WIN32
            system("pause");
#else
            cout << "Press Enter to continue...";
            cin.get();
#endif
        }
    } while (choice != 0);

    return 0;
}
