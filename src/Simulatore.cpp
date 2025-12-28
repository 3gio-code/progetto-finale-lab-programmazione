#include "Simulatore.h"
#include <iostream>
#include <fstream>
#include <random>

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_real_distribution<> vel_dist(vel_min, vel_max);
std::uniform_int_distribution<> char_dist(1, 26);
std::uniform_int_distribution<> num_dist(1, 9);
std::uniform_real_distribution<> tempo_dist(tempo_min, tempo_max);
std::uniform_real_distribution<> ritardo_dist(rit_gen_min, rit_gen_max);
// esempio di come usare il generatore casuale
// std::uniform_real_distribution<double> dist(1.0, 10.0);

void simulatore::genera_percorsi() const
{

    std::string targa = genera_targa();
    std::time_t 

    int svincolo_entrata = static_cast<int> dist;
}

std::string simulatore::genera_targa() const
{
    std::string targa;
    // + A - 1 perche li voglio maiuscoli
    targa += static_cast<char>(char_dist(mt) + 'A' - 1);
    targa += static_cast<char>(char_dist(mt) + 'A' - 1);

    targa += static_cast<char>(char_dist(mt) + '0');
    targa += static_cast<char>(char_dist(mt) + '0');            
    targa += static_cast<char>(char_dist(mt) + '0');

    targa += static_cast<char>(char_dist(mt) + 'A' - 1);
    targa += static_cast<char>(char_dist(mt) + 'A' - 1);

    return targa;
}