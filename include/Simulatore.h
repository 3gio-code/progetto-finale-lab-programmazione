#ifndef SIMULATORE_H
#define SIMULATORE_H

#include <ctime>
#include <string>
#include <vector>

// paramentri range velocita
constexpr int vel_min = 80;
constexpr int vel_max = 190;

// paramentri tempo di cambio velocita
constexpr double tempo_min = 300;
constexpr int tempo_max = 900;

// numero macchine
constexpr int num_macchine = 10000;

constexpr double rit_gen_min = 0.5;
constexpr double rit_gen_max = 10;

struct Car
{
    std::string targa;
    int svincolo_ingresso;
    int svincolo_uscita;
    std::time_t data_ora_ingresso;
    std::map<int, int> velocita;
};

class Simulatore
{
public:
    Simulatore() : time{0} {};
    void leggi_memorizza_autostrada(const std::string &file_path);

    std::string genera_targa() const;

    void genera_percorsi() const;
    void genera_passaggio() const;

private:
    std::string file_path;
    double time;
    std::vector<Car> macchine;
    std::map<int, int> varchi;
    std::map<int, int> svincoli;
};

#endif