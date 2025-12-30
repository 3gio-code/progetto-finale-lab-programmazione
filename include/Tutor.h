#ifndef TUTOR_H
#define TUTOR_H

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

// ============================================================================
// STRUTTURE DATI DI SUPPORTO
// ============================================================================

/**
 * Rappresenta lo stato di un veicolo "in viaggio".
 * Quando un veicolo passa sotto un varco, memorizziamo qui i dati.
 * Ci serve per confrontarli con il passaggio al varco successivo.
 * Utilizzato come valore nella mappa 'flotta_in_transito'.
 */
struct CheckpointVeicolo {
    int id_varco;       // ID numerico del varco (1, 2, 3...)
    double tempo;       // Istante del passaggio in secondi dall'inizio simulazione
};

/**
 * Contenitore per le statistiche di un singolo varco.
 * Serve per rispondere al comando 'stats' richiesto dal progetto.
 */
struct StatisticheVarco {
    int veicoli_transitati = 0;   // Contatore totale passaggi
    double somma_velocita = 0.0;  // Somma velocità (per calcolare la media)
    int numero_multe = 0;         // Numero di veicoli sanzionati in questa tratta
    
    // Nota: La velocità media del varco N si riferisce alla tratta (N-1 -> N)
};

// ============================================================================
// CLASSE PRINCIPALE
// ============================================================================

class Tutor {
public:
    /**
     * Costruttore del sistema Tutor.
     * Inizializza il sistema, carica la mappa dell'autostrada e apre il file dei passaggi.
     * file_autostrada Percorso al file 'Highway.txt' (es. "../Data/Highway.txt")
     * file_passaggi Percorso al file 'Passages.txt' (es. "../Data/Passages.txt")
     * lancia std::runtime_error Se non riesce ad aprire i file.
     */
    Tutor(const std::string& file_autostrada, const std::string& file_passaggi);

    /**
     * Distruttore.
     * Chiude correttamanete il file stream di lettura passaggi se aperto.
     */
    ~Tutor();

    // --- INTERFACCIA UTENTE (Comandi richiesti) ---

    /**
     * Esegue il comando "set_time"
     * * Legge i passaggi dal file fino al nuovo istante temporale.
     * Gestisce input in secondi ("20") o minuti ("20m").
     * input_tempo Stringa contenente il tempo (es. "120" o "10m").
     */
    void set_time(const std::string& input_tempo);

    /**
     * Esegue il comando "stats".
     * Stampa a video le statistiche per ogni varco:
     * - Numero veicoli
     * - Velocità media
     * - Multe emesse
     */
    void stats();

    /**
     * Esegue il comando "reset".
     * - Riporta il tempo a 0.
     * - Cancella la memoria dei veicoli in transito.
     * - Riavvolge il file 'Passages.txt' all'inizio.
     * - Azzera le statistiche.
     */
    void reset();

private:
    // ========================================================================
    // DATI MEMBRO (LO STATO DEL SISTEMA)
    // ========================================================================

    // 1. Mappa dell'Autostrada (Statica)
    // Associa l'ID sequenziale del varco (1, 2...) alla sua posizione in Km (10.5, 20.0...)
    // Fondamentale per calcolare il Delta Spazio.
    std::map<int, double> mappa_varchi;

    // 2. Flotta veicoli in transito (Dinamica)
    // La chiave è la TARGA (stringa univoca).
    // Il valore è l'ultimo CHECKPOINT (dove l'abbiamo vista l'ultima volta).
    // Scelta struttura dati: std::map garantisce ricerca in O(log N).
    // Essenziale per accoppiare Varco A -> Varco B.
    std::map<std::string, CheckpointVeicolo> flotta;

    // 3. Statistiche (Accumulatori)
    // Chiave: ID Varco. Valore: Struct con i contatori.
    std::map<int, StatisticheVarco> dati_statistici;

    // 4. Gestione File e Tempo
    std::ifstream stream_passaggi; // Stream di lettura aperto su Passages.txt
    std::string path_passaggi;     // Salviamo il percorso per poterlo riaprire al reset
    double tempo_corrente;         // Orologio interno del sistema (in secondi)

    // ========================================================================
    // METODI PRIVATI (LOGICA INTERNA)
    // ========================================================================

    /**
     * Carica la struttura statica da Highway.txt.
     * *Legge il file riga per riga. Ignora gli svincoli (non servono al Tutor).
     * Assegna ID progressivi (1, 2, 3...) ai varchi in ordine di apparizione
     * e memorizza le loro distanze chilometriche.
     */
    void carica_autostrada(const std::string& file_path);

    /**
     * Helper per convertire l'input utente in secondi.
     * input Stringa come "100" o "5m".
     * double Valore convertito in secondi (es. "5m" -> 300.0).
     */
    double parse_input_tempo(const std::string& input) const;

    /**
     * Core Business Logic: elabora un singolo transito.
     * Viene chiamata per ogni riga letta da Passages.txt.
     * 1. Controlla se la targa è già in 'flotta'.
     * 2. Se sì: calcola velocità media (Delta S / Delta T).
     * 3. Se > 130 km/h: stampa multa.
     * 4. Aggiorna la posizione del veicolo o lo inserisce se nuovo.
     */
    void elabora_passaggio(const std::string& targa, int id_varco, double istante);

    /**
     * Pulisce le stringhe lette dal file (rimuove < e >).
     */
    std::string pulisci_token(const std::string& token);
};

#endif // TUTOR_H