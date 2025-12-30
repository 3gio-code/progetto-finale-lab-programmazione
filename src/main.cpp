#include "Simulatore.h"
int main()
{
    // testing simulatore
    Simulatore s;

    s.leggi_memorizza_autostrada("data/Highway.txt");
    s.genera_percorsi();
    s.genera_passaggi();
}