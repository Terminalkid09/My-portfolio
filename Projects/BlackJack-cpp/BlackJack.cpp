#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <limits>

using namespace std;

struct Card {
    string name;
    int value;
};

Card drawCard() {
    int n = rand() % 13 + 1;
    if (n == 1) return {"A", 11};
    else if (n == 11) return {"J", 10};
    else if (n == 12) return {"Q", 10};
    else if (n == 13) return {"K", 10};
    else return {to_string(n), n};
}

int handSum(const vector<Card>& hand) {
    int sum = 0, aces = 0;
    for (const auto& c : hand) {
        sum += c.value;
        if (c.name == "A") aces++;
    }
    while (sum > 21 && aces > 0) {
        sum -= 10;
        aces--;
    }
    return sum;
}

void showHand(const string& label, const vector<Card>& hand) {
    cout << label << ": ";
    for (const auto& c : hand) cout << c.name << " ";
    cout << "(Total: " << handSum(hand) << ")" << endl;
}

void playerTurn(vector<Card>& hand) {
    char choice;
    do {
        if (handSum(hand) >= 21) break;
        cout << "Do you want to draw another card? (y/n): ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
        if (choice == 'y' || choice == 'Y') {
            hand.push_back(drawCard());
            showHand("Player", hand);
        }
    } while (choice == 'y' || choice == 'Y');
    // clear trailing newline if any for next getline/inputs
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

double evaluateResult(const vector<Card>& player, const vector<Card>& dealer, double bet) {
    int pTotal = handSum(player);
    int dTotal = handSum(dealer);
    if (pTotal > 21) return -bet;
    else if (dTotal > 21 || pTotal > dTotal) return bet;
    else if (pTotal == dTotal) return 0.0;
    else return -bet;
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    double balance = 100.0;
    char cont = 'y';

    do {
        if (balance <= 0) {
            cout << "You ran out of money! Game over.\n";
            break;
        }

        double bet;
        cout << "\nCurrent balance: $" << balance << "\nHow much would you like to bet? ";
        if (!(cin >> bet)) {
            cout << "Invalid bet.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        if (bet > balance || bet <= 0) {
            cout << "Invalid bet amount.\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // Initialize hands and variables
        vector<Card> playerHand;
        vector<Card> splitHand;
        vector<Card> dealerHand;
        bool didSplit = false;

        // Initial deal: two cards to player and two to dealer
        playerHand.push_back(drawCard());
        playerHand.push_back(drawCard());
        dealerHand.push_back(drawCard());
        dealerHand.push_back(drawCard());

        // Show player hand and one dealer card
        showHand("Player", playerHand);
        cout << "Dealer: " << dealerHand[0].name << " ?" << " (second card hidden)\n";

        // Allow split if first two player cards have same name/value
        if (playerHand.size() == 2 && playerHand[0].name == playerHand[1].name) {
            cout << "Do you want to split? (y/n): ";
            char s;
            if (cin >> s) {
                if (s == 'y' || s == 'Y') {
                    didSplit = true;
                    // move second card to split hand
                    splitHand.push_back(playerHand[1]);
                    playerHand.pop_back();
                    // draw replacement card for each hand
                    playerHand.push_back(drawCard());
                    splitHand.push_back(drawCard());
                }
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        // Player's turn for first hand
        cout << "\n--- First hand ---\n";
        showHand("Player", playerHand);
        playerTurn(playerHand);

        // If split, play second hand
        if (didSplit) {
            cout << "\n--- Second hand (split) ---\n";
            showHand("Second hand", splitHand);
            playerTurn(splitHand);
        }

        // Dealer's turn: draw until 17 or more
        while (handSum(dealerHand) < 17) {
            dealerHand.push_back(drawCard());
        }
        cout << "\n--- Dealer ---\n";
        showHand("Dealer", dealerHand);

        // Evaluate results and update balance
        double result = evaluateResult(playerHand, dealerHand, bet);
        balance += result;
        cout << "First hand result: " << (result >= 0 ? "+" : "") << result << "  Balance: $" << balance << endl;

        if (didSplit) {
            double result2 = evaluateResult(splitHand, dealerHand, bet);
            balance += result2;
            cout << "Second hand result: " << (result2 >= 0 ? "+" : "") << result2 << "  Balance: $" << balance << endl;
        }

        // Ask to continue
        cout << "Do you want to play again? (y/n): ";
        if (!(cin >> cont)) {
            break;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    } while (cont == 'y' || cont == 'Y');

    return 0;
}