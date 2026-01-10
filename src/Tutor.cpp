#include "Tutor.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <stdexcept>
#include <vector>

//COSTRUTTORE E DISTRUTTORE 

Tutor::Tutor(const std::string &file_autostrada, const std::string &file_passaggi)
    : path_passaggi(file_passaggi), tempo_corrente(0.0)
{
    // 1. Carica la configurazione dell'autostrada
    carica_autostrada(file_autostrada);

    // 2. Apre il file dei passaggi
    stream_passaggi.open(path_passaggi);
    if (!stream_passaggi.is_open())
    {
        throw std::runtime_error("Errore: impossibile aprire il file passaggi " + path_passaggi);
    }

    std::cout << "Sistema Tutor inizializzato correttamente." << std::endl;
    std::cout << "Varchi rilevati: " << mappa_varchi.size() << std::endl;
}

Tutor::~Tutor()
{
    if (stream_passaggi.is_open())
    {
        stream_passaggi.close();
    }
}

//FUNZIONI CARICAMENTO FILE

void Tutor::carica_autostrada(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Impossibile leggere file autostrada: " + file_path);
    }

    std::string riga;
    int id_progressivo = 1;

    while (std::getline(file, riga)) {
        // Se la riga è vuota o commentata, saltala
        if (riga.empty()) continue;

        // Sostituisco tutte le < e > con spazi vuoti.
        //C++ ignora gli spazi vuoti quando vengono letti numeri (facilito la lettura)
        std::replace(riga.begin(), riga.end(), '<', ' ');
        std::replace(riga.begin(), riga.end(), '>', ' ');

        std::stringstream ss(riga);
        double km = -1.0;
        char tipo = ' ';

        // Provo a leggere: prima il numero (km), poi il carattere (tipo)
        if (ss >> km >> tipo) {
            // Se ho letto con successo e il tipo è 'V' (Varco)
            if (tipo == 'V') {
                mappa_varchi[id_progressivo] = km;
                
                // Inizializzo le statistiche
                dati_statistici[id_progressivo] = StatisticheVarco();
                
                id_progressivo++;
            }
        }
    }
    file.close();

    // Controllo finale richiesto dalle specifiche
    if (mappa_varchi.size() < 2) {
        throw std::runtime_error("Configurazione non valida: servono almeno 2 varchi (trovati: " + std::to_string(mappa_varchi.size()) + ").");
    }
}

std::string Tutor::pulisci_token(const std::string &token)
{
    std::string pulita;
    for (char c : token)
    {
        // Rimuove <, >, #
        if (c != '<' && c != '>' && c != '#')
        {
            pulita += c;
        }
    }
    return pulita;
}

//Comandi di Tutor

double Tutor::parse_input_tempo(const std::string &input) const
{
    if (input.empty())
        return 0.0;

    std::string temp {input}; 
    bool is_minutes {false};  

    if (temp.back() == 'm')
    {
        is_minutes = true;
        temp.pop_back();
    }

    try
    {
        double val { std::stod(temp) }; 

        if (is_minutes)
        {
            return val * 60.0;
        }
        return val;
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Errore: '" << input << "' non e' un numero valido. Uso 0." << std::endl;
        return 0.0;
    }
    catch (const std::out_of_range& e)
    {
        std::cerr << "Errore: il numero inserito e' fuori scala. Uso 0." << std::endl;
        return 0.0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Errore imprevisto durante la lettura del tempo: " << e.what() << std::endl;
        return 0.0;
    }
}

void Tutor::reset()
{
    tempo_corrente = 0.0;
    flotta.clear();
    dati_statistici.clear();

    // Riavvolge il file
    stream_passaggi.clear(); // Pulisce eventuali flag di errore/EOF
    stream_passaggi.seekg(0, std::ios::beg);

    std::cout << "Sistema resettato allo stato iniziale." << std::endl;
}

void Tutor::set_time(const std::string &input_tempo)
{
    double incremento = parse_input_tempo(input_tempo);
    double target_time = tempo_corrente + incremento;

    std::cout << "Analisi traffico da " << std::fixed << std::setprecision(1)
              << tempo_corrente << "s a " << target_time << "s." << std::endl << std::endl;

    std::string riga;
    std::streampos posizione_precedente;

    // Ciclo di lettura
    while (true)
    {
        // Salviamo la posizione corrente prima di leggere
        posizione_precedente = stream_passaggi.tellg();

        if (!std::getline(stream_passaggi, riga))
        {
            break; // Fine del file
        }

        if (riga.empty())
            continue;

        std::stringstream ss(riga);
        std::string token_varco, token_targa, token_tempo;

        // Legge i tre token: <#KM> <TARGA> <TEMPO>
        if (ss >> token_varco >> token_targa >> token_tempo)
        {

            // Pulisce i dati
            std::string s_varco = pulisci_token(token_varco); 
            std::string targa = pulisci_token(token_targa);  
            std::string s_tempo = pulisci_token(token_tempo); 

            double km_varco = std::stod(s_varco);
            double istante_passaggio = std::stod(s_tempo);

            // controllo temporale
            if (istante_passaggio > target_time)
            {
                // Il passaggio è nel futuro rispetto al target_time.
                // Torniamo indietro nel file e ci fermiamo.
                stream_passaggi.seekg(posizione_precedente);
                break;
            }

        // 1. Leggiamo l'ID direttamente come intero
        int id_varco = std::stoi(s_varco); 

       // 2. Controlliamo se questo ID esiste nella nostra mappa
       if (mappa_varchi.find(id_varco) != mappa_varchi.end())
    {
      // Se esiste, procediamo
      elabora_passaggio(targa, id_varco, istante_passaggio);
    }
    else 
    {
       std::cerr << "Attenzione: trovato ID Varco inesistente: " << id_varco << std::endl;
    }

            if (id_varco != -1)
            {
                elabora_passaggio(targa, id_varco, istante_passaggio);
            }
        }
    }

    // Aggiorniamo il tempo del sistema
    tempo_corrente = target_time;
    std::cout << std::endl << std::endl;
}

void Tutor::elabora_passaggio(const std::string &targa, int id_varco, double istante)
{

    // Aggiorniamo le statistiche base del varco corrente
    dati_statistici[id_varco].veicoli_transitati++;

    // Cerchiamo se il veicolo è già in memoria
    auto it = flotta.find(targa);

    if (it != flotta.end())
    {
        // Veicolo trovato
        CheckpointVeicolo check_prev = it->second;

        // Verifica se stiamo percorrendo una tratta valida (Varco X -> Varco X+1)
        if (id_varco == check_prev.id_varco + 1)
        {

            // Calcoli fisici
            double km_start = mappa_varchi[check_prev.id_varco];
            double km_end = mappa_varchi[id_varco];
            double delta_spazio_km = std::abs(km_end - km_start);

            double delta_tempo_sec = istante - check_prev.tempo;
            double delta_tempo_h = delta_tempo_sec / 3600.0;

            // Velocità media
            double vel_media = 0.0;
            if (delta_tempo_h > 0)
            {
                vel_media = delta_spazio_km / delta_tempo_h;
            }

            // Aggiorna statistiche velocità media varco
            dati_statistici[id_varco].somma_velocita += vel_media;

            //Solo ora abbiamo contato una velocità vera, quindi la contiamo
            dati_statistici[id_varco].cont_veicoli_validi++;

            // CONTROLLO MULTA
            if (vel_media > 130.0)
            {
                // Emettiamo sanzione
                dati_statistici[id_varco].numero_multe++;

                std::cout << "SANZIONE | Targa: " << targa
                          << " | Tratta: Varco " << check_prev.id_varco << " -> " << id_varco
                          << " | Vel: " << std::fixed << std::setprecision(2) << vel_media << " km/h"
                          << " | In: " << std::fixed << std::setprecision(1) << check_prev.tempo
                          << "s Out: " << istante << "s" << std::endl;
            }
        }
    }

    // Aggiorniamo la posizione del veicolo (o lo inseriamo se nuovo)
    CheckpointVeicolo nuovo_check;
    nuovo_check.id_varco = id_varco;
    nuovo_check.tempo = istante;
    flotta[targa] = nuovo_check;
}

void Tutor::stats() 
{
    std::cout << std::endl << "== STATISTICHE SISTEMA (T = " << tempo_corrente << "s) ===" << std::endl;

    // Calcoliamo minuti trascorsi (evitando divisione per 0)
    double minuti_trascorsi = (tempo_corrente > 0) ? (tempo_corrente / 60.0) : 1.0;

    for (const auto &[id, dati] : dati_statistici)
    {
        double media_vel = 0.0;
        // La media è calcolata solo sui veicoli che hanno completato la tratta (non i primi ingressi)
        // Ma per semplicità usiamo somma/totale passaggi se il totale > 0
        // Nota: la somma_velocita viene incrementata solo quando c'è una coppia di varchi.
        // Quindi dobbiamo contare quante tratte sono state chiuse su questo varco?
        // Per semplicità statistica usiamo i veicoli transitati, sapendo che al Varco 1 la media sarà 0.

        // Calcolo più raffinato: se somma > 0, dividiamo per i veicoli che hanno contribuito.
        // Ma qui usiamo una approssimazione basata sul totale transiti per rispettare la consegna semplice.
        // Un modo migliore sarebbe avere un contatore 'tratte_completate' in StatisticheVarco.
        // Dato che somma_velocita è popolata solo in elabora_passaggio quando c'è un PREV,
        // la media sarà corretta solo se dividiamo per (veicoli_transitati - ingressi_nuovi).
        // Tuttavia, il testo chiede solo "velocità media".

        //Usa conteggio delle velocità valide invece di veicoli transitati
        if(dati.cont_veicoli_validi > 0)
        {
            media_vel = dati.somma_velocita / dati.cont_veicoli_validi;
        }

        std::cout << "Varco " << id << " (" << mappa_varchi[id] << " Km):" << std::endl;
        std::cout << "  - Veicoli transitati: " << dati.veicoli_transitati << std::endl;
        std::cout << "  - Flusso: " << std::fixed << std::setprecision(2)
                  << (dati.veicoli_transitati / minuti_trascorsi) << " veicoli/min" << std::endl;

        if (id > 1)
        { // Ha senso parlare di velocità media solo dal secondo varco in poi dati i files di input
            std::cout << "  - Velocita' media rilevata: " << media_vel << " km/h" << std::endl;
            std::cout << "  - Sanzioni emesse: " << dati.numero_multe << std::endl;
        }
        else
        {
            std::cout << "  - (Varco di ingresso iniziale - Nessuna statistica velocita')" << std::endl;
        }
        std::cout << std::endl;
    }
}