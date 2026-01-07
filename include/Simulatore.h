#ifndef SIMULATORE_H
#define SIMULATORE_H

#include <string>
#include <vector>
#include <chrono>
#include <utility> // per std::pair
#include <random>

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
    std::vector<std::pair<int, int>> velocita;
};

class Simulatore
{
public:
    // Costruttore: Inizializza i path e rispetta l'ordine delle variabili private
    Simulatore() : path_highway("data/Highway.txt"),
                   path_runs("data/Runs.txt"),
                   path_passages("data/Passages.txt"),
                   time{0},
                   adesso{std::chrono::system_clock::now()},
                   // macchine, varchi, svincoli inizializzati implicitamente (vector vuoti)
                   mt(std::random_device{}()),
                   vel_dist(vel_min, vel_max),
                   char_dist(1, 26),
                   num_dist(0, 9),
                   tempo_dist(tempo_min, tempo_max),
                   ritardo_dist(rit_gen_min, rit_gen_max) {
                   };

    // Metodi senza parametri (usano le variabili private)
    void scrivi();
    void genera_percorsi();
    void genera_passaggi() const; // const mantenuto perché non modifica lo stato della classe (solo output su file)

    std::string genera_targa();
    bool is_number(const std::string &s) const;
    void leggi_memorizza_autostrada();

private:
    // Variabili per i percorsi dei 3 file (dichiarate per prime)
    std::string path_highway;
    std::string path_runs;
    std::string path_passages;

    double time;
    std::chrono::time_point<std::chrono::system_clock> adesso;

    std::vector<Car> macchine;
    std::vector<double> varchi;
    std::vector<double> svincoli;

    // generatore
    std::mt19937 mt;

    std::uniform_int_distribution<> vel_dist;
    std::uniform_int_distribution<> char_dist;
    std::uniform_int_distribution<> num_dist;
    std::uniform_int_distribution<> tempo_dist;
    std::uniform_real_distribution<> ritardo_dist;
};

#endif