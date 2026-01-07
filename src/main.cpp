#include "Simulatore.h"

int main()
{
    // testing simulatore
    Simulatore s;

    // Chiamate senza parametri (i percorsi sono definiti nel costruttore di Simulatore)
    s.leggi_memorizza_autostrada();
    s.genera_percorsi();
    s.genera_passaggi();
}