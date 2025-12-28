#include "Simulatore.h"
#include <iostream>
#include <fstream>
#include <random>
#include <stdexcept>

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
    double secondi_relativi_ingresso = ritardo_dist(mt);
    time += secondi_relativi_ingresso;
}

void leggi_memorizza_autostrada(const std::string &file_path)
{
    std::ifstream Highway(file_path);
    if (!Highway.is_open())
    {
        throw std::runtime_error("Impossibile aprire file");
    }
    std::string riga;

    // Definisci qui i simboli specifici che vuoi togliere (oltre agli spazi)
    std::string simboliValidi = "0123456789SV<>";
    bool allow_insertion = false;
    std::string rigaPulita = "";
    while (getline(Highway, line))
    {
        rigaPulita = ""; // Qui costruiremo la nuova riga

        // 2. IL FOR PER I CARATTERI
        for (char c : riga)
        {
            // righe con commenti
            if (c == '*')
            {
                break;
            }

            // A. Controllo Spazi: Se è uno spazio, saltalo (continue)
            if (c == ' ')
            {
                continue;
            }

            // B. Controllo Simboli: Verifica se 'c' è nella lista dei vietati
            // string::npos significa "non trovato", quindi se != npos, l'ha trovato
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
                    try
                    {
                        int numero = static_cast<int>(rigaPulita);
                    }
                    catch (std::runtime_error e)
                    {
                        throw e;
                    }
                    svincoli.push_back(numero);
                }
                break;
                case 'V':
                {
                    try
                    {
                        int numero = static_cast<int>(rigaPulita);
                    }
                    catch (std::runtime_error e)
                    {
                        throw e;
                    }
                    varchi.push_back(numero);
                }
                break;
                default:
                {
                    if (allow_insertion == false)
                    {
                        throw std::runtime_error("Formattazione file di ingresso sbagliata");
                    }
                    rigaPulita += c;
                }
                break;
                }

                continue;
            }

            // Se siamo arrivati qui, il carattere è valido: aggiungilo
            throw std::runtime_error("Formattazione file di ingresso sbagliata");
        }

        // Stampa o usa la riga pulita
        if (allow_insertion == true)
        {
            throw std::runtime_error("Formattazione file di ingresso sbagliata");
        }
    }

    Highway.close();
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