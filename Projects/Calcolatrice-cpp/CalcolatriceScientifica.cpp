

#include <iostream>
#include <cmath>
using namespace std;

int main() {
int scelta;
double num1, num2, risultato;

do
{
    //mostra il menù
cout << "Calcolatrice scientifica\n";
cout << "1. Somma\n";
cout << "2. Sottrazione\n";
cout << "3. Moltiplicazione\n";
cout << "4. Divisione\n";
cout << "5. Potenza\n";
cout << "6. Radice quadrata\n";
cout << "7. Seno\n";
cout << "8. Coseno\n";
cout << "Scegli un'operazione (1-8):";
cin >> scelta;

switch (scelta) {
  case 1:
  cout << "Inserisci 2 numeri:";
  cin >> num1 >> num2;
  risultato = num1 + num2;
  break;
  case 2:
  cout << "Inserisci 2 numeri:";
  cin >> num1 >> num2;
  risultato = num1 - num2;
  break;
  case 3:
  cout << "Inserisci 2 numeri";
  cin >> num1 >> num2;
  risultato = num1 * num2;
  break;
  case 4:
  cout << "Inserisci 2 numeri";
  cin >> num1 >> num2;
  if (num2 !=0){
    risultato = num1 / num2;
  } else {
    cout << "Syntax error!";
     return 1;
     }
  
  break;
  case 5:
  cout << "Inserisci base ed esponente:";
  cin >> num1 >> num2;
  risultato = pow (num1, num2);
  break;
  case 6:
  cout << "Inserisci 1 numero:";
  cin >> num1;
  if (num1 >= 0){
  risultato = sqrt (num1);
  } else {
    cout << "Syntax error!";
  return 1;
  }
  break;
  case 7:
  cout << "Inserisci un'angolo in radianti:";
  cin >> num1;
  risultato = sin (num1);
  break;
  case 8:
  cout << "Inserisci un'angolo in radianti:";
  cin >> num1;
  risultato = cos (num1);
  break;
  

}

cout << "Risultato:" << risultato << endl;

// Chiedi all'utente se vuole continuare
char continua;
cout << "Vuoi continuare? (s/n): ";
cin >> continua;
if (continua == 'n' || continua == 'N') {
    break;
}
} while (true);

return 0;
}


