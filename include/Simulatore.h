#ifndef SIMULATORE_H
#define SIMULATORE_H

#include <string>
#include <vector>
#include <chrono>
#include <utility> // per std::pair

// Parametri costanti
constexpr int vel_min = 80;
constexpr int vel_max = 190;
constexpr double tempo_min = 300; // in secondi
constexpr int tempo_max = 900;
constexpr int num_macchine = 10000;
constexpr double rit_gen_min = 0.5;
constexpr double rit_gen_max = 10;

struct Car
{
    std::string targa;
    int svincolo_ingresso;
    int svincolo_uscita;
    std::chrono::time_point<std::chrono::system_clock> data_ora_ingresso;
    // Cambiato map in vector<pair> per preservare l'ordine e permettere velocità duplicate nel tempo
    std::vector<std::pair<int, int>> velocita;
};

class Simulatore
{
public:
    Simulatore() : time{0}, adesso{std::chrono::system_clock::now()}, file_path("../data/Highway.txt") {};

    void scrivi();
    void leggi_memorizza_autostrada(const std::string &file_path);
    void genera_percorsi();
    std::string genera_targa() const;
    bool is_number(const std::string &s) const;

private:
    std::string file_path;
    double time;
    // Corretto: auto non è permesso qui, serve il tipo esplicito
    std::chrono::time_point<std::chrono::system_clock> adesso;
    std::vector<Car> macchine;
    std::vector<double> varchi;
    std::vector<double> svincoli;
};

#endif