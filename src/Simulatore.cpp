#include "../include/Simulatore.h" // Assicurati che il percorso sia corretto rispetto al tuo progetto
#include <iostream>
#include <fstream>
#include <random>
#include <stdexcept>
#include <algorithm>
#include <iomanip>
#include <cmath> // per std::round

// Variabili globali per il random (ok per questo livello, meglio se statiche o membri)
std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<> vel_dist(vel_min, vel_max);
std::uniform_int_distribution<> char_dist(1, 26);
std::uniform_int_distribution<> num_dist(0, 9); // Corretto da 1-9 a 0-9
std::uniform_int_distribution<> tempo_dist(tempo_min, tempo_max);
std::uniform_real_distribution<> ritardo_dist(rit_gen_min, rit_gen_max);

// Aggiunto scope Simulatore::
void Simulatore::genera_percorsi()
{
    int k{0};

    while (k < num_macchine)
    {
        // Genera dati iniziali
        std::string targa_temp = genera_targa();
        double secondi_relativi_ingresso = ritardo_dist(mt);
        time += secondi_relativi_ingresso; // Aggiorna il tempo globale della simulazione

        // cast necessario per aggiungere double (secondi) a time_point
        auto futuro = adesso + std::chrono::milliseconds(static_cast<long long>(time * 1000));

        // Logica svincoli
        if (svincoli.empty())
            throw std::runtime_error("Nessuno svincolo caricato!");

        std::uniform_int_distribution<> svincolo_ingresso_dist(0, svincoli.size() - 2); // -2 perché deve esserci almeno 1 uscita dopo
        int svincolo_ingresso_idx = svincolo_ingresso_dist(mt);

        std::uniform_int_distribution<> svincolo_uscita_dist(svincolo_ingresso_idx + 1, svincoli.size() - 1);
        int svincolo_uscita_idx = svincolo_uscita_dist(mt);

        // Calcolo distanza target
        // Assumo che svincoli contenga i KM progressivi (es. 0, 10.5, 20.0)
        double km_ingresso = svincoli[svincolo_ingresso_idx];
        double km_uscita = svincoli[svincolo_uscita_idx];
        double distanza_totale = km_uscita - km_ingresso;

        double tot_km = 0.0;
        std::vector<std::pair<int, int>> velocita_temp;

        while (tot_km < distanza_totale)
        {
            int velocita = vel_dist(mt);
            int tempo_sec = tempo_dist(mt);

            double tempo_ore = static_cast<double>(tempo_sec) / 3600.0;
            double distanza_step = velocita * tempo_ore;

            if (tot_km + distanza_step >= distanza_totale)
            {
                // Calcola quanto manca esattamente
                double distanza_rimanente = distanza_totale - tot_km;
                double tempo_ore_necessario = distanza_rimanente / velocita;
                int tempo_sec_necessario = static_cast<int>(tempo_ore_necessario * 3600);

                // Evita tempi negativi o zero se siamo al limite
                if (tempo_sec_necessario > 0)
                    velocita_temp.push_back({velocita, tempo_sec_necessario});

                tot_km += distanza_rimanente;
                break;
            }
            else
            {
                velocita_temp.push_back({velocita, tempo_sec});
                tot_km += distanza_step;
            }
        }

        // Inizializzazione corretta della Struct
        Car temp;
        temp.targa = targa_temp;
        temp.svincolo_ingresso = svincolo_ingresso_idx;
        temp.svincolo_uscita = svincolo_uscita_idx;
        temp.data_ora_ingresso = futuro;
        temp.velocita = velocita_temp;

        macchine.push_back(temp);
        ++k;
    }
    scrivi();
}

void Simulatore::scrivi()
{
    // Nota: Aprire e chiudere il file ogni volta sovrascrive tutto se non usi std::ios::app
    // Qui ho lasciato std::ofstream default che tronca il file, come nel tuo codice originale.
    std::ofstream Runs("../data/Runs.txt");

    // Corretto il loop for con const auto&
    for (const auto &c : macchine)
    {
        Runs << "<" << c.targa << ">" << std::endl;
        // Puoi aggiungere qui la stampa delle velocità se vuoi
    }
    Runs.close(); // Aggiunto punto e virgola
}

void Simulatore::leggi_memorizza_autostrada(const std::string &file_path)
{
    std::ifstream Highway(file_path);
    if (!Highway.is_open())
    {
        throw std::runtime_error("Impossibile aprire file: " + file_path);
    }

    std::string riga;
    std::string simboliValidi = "0123456789.SV<>";

    // Corretto 'line' in 'riga'
    while (getline(Highway, riga))
    {
        std::string rigaPulita = "";
        bool allow_insertion = false; // Corretto assegnazione

        for (char c : riga)
        {
            if (c == '*')
                break; // Commento
            if (c == ' ')
                continue; // Spazio

            if (simboliValidi.find(c) != std::string::npos)
            {
                switch (c)
                {
                case '<':
                    allow_insertion = true;
                    break;
                case '>':
                    allow_insertion = false;
                    break;
                // RIMOSSO IL CASE '.' : il punto deve essere parte del numero in default
                case 'S':
                {
                    if (!is_number(rigaPulita))
                        throw std::runtime_error("Formattazione non numerica per Svincolo");
                    double numero = std::stod(rigaPulita);
                    svincoli.push_back(numero);
                    rigaPulita = ""; // Reset buffer
                }
                break;
                case 'V':
                {
                    if (!is_number(rigaPulita))
                        throw std::runtime_error("Formattazione non numerica per Varco");
                    double numero = std::stod(rigaPulita);
                    varchi.push_back(numero);
                    rigaPulita = ""; // Reset buffer
                }
                break;
                default:
                    // Qui accumuliamo cifre E IL PUNTO decimale
                    if (allow_insertion)
                    {
                        rigaPulita += c;
                    }
                    break;
                }
            }
            else
            {
                // Carattere non valido trovato
            }
        }
    }

    Highway.close();

    std::sort(varchi.begin(), varchi.end());
    std::sort(svincoli.begin(), svincoli.end());

    if (varchi.size() < 2)
    {
        throw std::runtime_error("Numero minimo di varchi non rispettato");
    }
    if (varchi[0] - 1 < svincoli[0])
    {
        throw std::runtime_error("struttura autostrada non rispettata");
    }
    if (varchi.back() + 1 > svincoli.back())
    {
        throw std::runtime_error("struttura autostrada non rispettata");
    }

    for (auto v : varchi)
    {
        for (auto s : svincoli)
        {
            if (std::abs(v - s) <= 1.0)
            {
                throw std::runtime_error("struttura autostrada non rispettata (sovrapposizione)");
            }
        }
    }
}

bool Simulatore::is_number(const std::string &s) const
{
    // Permette cifre e un singolo punto decimale
    bool punto_trovato = false;
    if (s.empty())
        return false;
    for (char c : s)
    {
        if (std::isdigit(c))
            continue;
        if (c == '.')
        {
            if (punto_trovato)
                return false; // due punti non validi
            punto_trovato = true;
            continue;
        }
        return false;
    }
    return true;
}

std::string Simulatore::genera_targa() const
{
    std::string targa;
    targa += static_cast<char>(char_dist(mt) + 'A' - 1);
    targa += static_cast<char>(char_dist(mt) + 'A' - 1);

    targa += std::to_string(num_dist(mt));
    targa += std::to_string(num_dist(mt));
    targa += std::to_string(num_dist(mt));

    targa += static_cast<char>(char_dist(mt) + 'A' - 1);
    targa += static_cast<char>(char_dist(mt) + 'A' - 1);

    return targa;
}