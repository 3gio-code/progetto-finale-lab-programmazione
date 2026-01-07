#include "Simulatore.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <cmath>

// --- OPERATOR OVERLOAD ---
template <typename T, typename Y>
std::ostream &operator<<(std::ostream &stream, std::pair<T, Y> const &p)
{
    stream << "<" << p.first << " " << p.second << "> ";
    return stream;
}

// --- METODI SIMULATORE ---

void Simulatore::genera_percorsi()
{
    int k{0};

    while (k < num_macchine)
    {
        std::string targa_temp = genera_targa();
        double secondi_relativi_ingresso = ritardo_dist(mt);
        time += secondi_relativi_ingresso;

        auto futuro = adesso + std::chrono::milliseconds(static_cast<long long>(time * 1000));

        if (svincoli.empty())
            throw std::logic_error("Nessuno svincolo caricato!");

        std::uniform_int_distribution<> svincolo_ingresso_dist(0, svincoli.size() - 2);
        int svincolo_ingresso_idx = svincolo_ingresso_dist(mt);

        std::uniform_int_distribution<> svincolo_uscita_dist(svincolo_ingresso_idx + 1, svincoli.size() - 1);
        int svincolo_uscita_idx = svincolo_uscita_dist(mt);

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
                double distanza_rimanente = distanza_totale - tot_km;
                double tempo_ore_necessario = distanza_rimanente / velocita;
                int tempo_sec_necessario = static_cast<int>(tempo_ore_necessario * 3600);

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

        Car temp;
        temp.targa = targa_temp;
        temp.svincolo_ingresso = svincolo_ingresso_idx;
        temp.svincolo_uscita = svincolo_uscita_idx;
        temp.data_ora_ingresso = futuro;
        temp.velocita = velocita_temp;

        macchine.push_back(temp);
        ++k;
    }

    // Chiama scrivi senza parametri (userà path_runs interno)
    scrivi();
}

void Simulatore::scrivi()
{
    // Usa la variabile privata path_runs
    std::ofstream Runs(path_runs, std::ios::trunc);
    if (!Runs.is_open())
    {
        throw std::runtime_error("Errore di I/O: Impossibile scrivere su " + path_runs);
    }

    for (const auto &c : macchine)
    {
        std::time_t time_val = std::chrono::system_clock::to_time_t(c.data_ora_ingresso);
        std::tm *time_ptr = std::localtime(&time_val);

        Runs << "<" << c.targa << "> "
             << "<" << c.svincolo_ingresso << "> "
             << "<" << c.svincolo_uscita << "> "
             << "<" << std::put_time(time_ptr, "%d-%m-%Y %H:%M:%S") << "> ";

        for (const auto &s : c.velocita)
        {
            Runs << s;
        }
        Runs << "\n";
    }
    Runs.close();
}

void Simulatore::leggi_memorizza_autostrada()
{
    // Usa la variabile privata path_highway
    std::ifstream Highway(path_highway);
    if (!Highway.is_open())
    {
        throw std::runtime_error("Impossibile aprire file: " + path_highway);
    }

    std::string riga;
    std::string simboliValidi{"0123456789.SV<>"};

    while (getline(Highway, riga))
    {
        std::string rigaPulita{""};
        bool allow_insertion{false};

        for (char c : riga)
        {
            if (c == '*')
                break;
            if (c == ' ')
                continue;

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
                case 'S':
                {
                    if (!allow_insertion)
                        throw std::runtime_error("Formattazione sbagliata per Svincolo");

                    if (!is_number(rigaPulita))
                        throw std::runtime_error("Formattazione non numerica per Svincolo");
                    double numero{std::stod(rigaPulita)};
                    svincoli.push_back(numero);
                    rigaPulita = "";
                }
                break;
                case 'V':
                {
                    if (!allow_insertion)
                        throw std::runtime_error("Formattazione sbagliata per Svincolo");

                    if (!is_number(rigaPulita))
                        throw std::runtime_error("Formattazione non numerica per Varco");
                    double numero{std::stod(rigaPulita)};
                    varchi.push_back(numero);
                    rigaPulita = "";
                }
                break;
                default:
                    if (allow_insertion)
                    {
                        rigaPulita += c;
                    }
                    break;
                }
            }
        }
    }

    Highway.close();

    std::sort(varchi.begin(), varchi.end());
    std::sort(svincoli.begin(), svincoli.end());

    if (varchi.size() < 2)
    {
        throw std::logic_error("Numero minimo di varchi non rispettato");
    }

    if (varchi[0] - 1 < svincoli[0])
    {
        throw std::logic_error("struttura autostrada non rispettata (V < S+1)");
    }
    if (varchi.back() + 1 > svincoli.back())
    {
        throw std::logic_error("struttura autostrada non rispettata (V > S-1)");
    }

    for (const auto &v : varchi)
    {
        for (auto s : svincoli)
        {
            if (std::abs(v - s) <= 1.0)
            {
                throw std::logic_error("struttura autostrada non rispettata (sovrapposizione)");
            }
        }
    }
}

bool Simulatore::is_number(const std::string &s) const
{
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
                return false;
            punto_trovato = true;
            continue;
        }
        return false;
    }
    return true;
}

std::string Simulatore::genera_targa()
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

void Simulatore::genera_passaggi() const
{
    // Usa la variabile privata path_passages
    std::ofstream Passages(path_passages, std::ios::trunc);
    if (!Passages.is_open())
    {
        std::cerr << "Errore: Impossibile scrivere su " << path_passages << std::endl;
        return;
    }

    for (const auto &c : macchine)
    {
        int i{0};
        while (i < varchi.size() && varchi[i] <= svincoli[c.svincolo_ingresso])
        {
            ++i;
        }

        int j{i};
        while (j < varchi.size() && varchi[j] < svincoli[c.svincolo_uscita])
        {
            ++j;
        }

        while (i < j)
        {
            double varco = varchi[i];
            double km_tot{0};

            for (int t{0}; t < c.velocita.size(); ++t)
            {
                double velocita_step = static_cast<double>(c.velocita[t].first);
                double tempo_step = static_cast<double>(c.velocita[t].second);
                double km_percorsi_step = (velocita_step * tempo_step) / 3600.0;

                if ((km_tot + svincoli[c.svincolo_ingresso] + km_percorsi_step) < varco)
                {
                    km_tot += km_percorsi_step;
                    continue;
                }

                double momento_passaggio{0};
                for (int k{0}; k < t; ++k)
                {
                    momento_passaggio += c.velocita[k].second;
                }

                double distanza_mancante = varco - (svincoli[c.svincolo_ingresso] + km_tot);
                momento_passaggio += (distanza_mancante / velocita_step) * 3600.0;

                Passages << "<#" << i + 1 << "> <" << c.targa << "> <" << momento_passaggio << ">\n";

                break;
            }
            ++i;
        }
    }
    Passages.close();
}