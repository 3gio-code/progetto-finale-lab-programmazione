#ifndef TUTOR_H
#define TUTOR_H
#include <string>
#include <map>
#include <fstream>

/**
 * Struct per rappresentare un veicolo in viaggio.
 * Quando un veicolo passa sotto un varco, memorizziamo qui i dati:
 * id varco(che verrà collegato alla sua distanza) e tempo in secondi.
 */
struct CheckpointVeicolo {
    int id_varco;       
    double tempo;       
};

/**
 * Struct per le statistiche di un singolo varco.
 * Vengono memorizzati i seguenti dati corrispettivi ad ogni varco:
 * veicoli totali transitati, somma totale delle velocità,
 * numeri totale di veicoli sanzionati, veicoli con velocitè valida 
 * (questi ultimi due dati verranno utilizzati per calcolare la velocità media dal varco N-1 a N).
 */
struct StatisticheVarco {
    int veicoli_transitati = 0;   
    double somma_velocita = 0.0;  
    int numero_multe = 0;    
    int cont_veicoli_validi = 0;   
};

//inizio della classe principale Tutor
class Tutor {
public:
    /**
     * Costruttore del sistema Tutor.
     * Inizializza il sistema, carica la mappa dell'autostrada e apre il file dei passaggi.
     * lancia std::runtime_error se non riesce ad aprire i file.
     */
    Tutor(const std::string& file_autostrada, const std::string& file_passaggi);

    /**
     * Distruttore.
     * Chiude correttamanete il file stream di lettura passaggi se aperto.
     */
    ~Tutor();

    // Vieta la copia (Costruttore di copia)
    Tutor(const Tutor&) = delete;
   
    // Vieta l'assegnamento (Operatore di assegnamento)
    Tutor& operator=(const Tutor&) = delete; 

    // Comandi richiesti da Tutor

    /**
     * Gestisce il comando "set_time"
     * Legge i passaggi dal file fino al nuovo istante temporale.
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
    void stats() ;


    /**
     * Esegue il comando "reset".
     * - Riporta il tempo a 0.
     * - Cancella la memoria dei veicoli in transito.
     * - Riavvolge il file 'Passages.txt' all'inizio.
     * - Azzera le statistiche.
     */
    void reset();

private:

    //Dati membro

    // Mappa dell'Autostrada 
    // Associa l'ID del varco alla sua posizione in Km 
    // Fondamentale per calcolare il delta dpazio necessario nella velocità.
    std::map<int, double> mappa_varchi;

    // Flotta veicoli in transito 
    // Il valore è l'ultimo CHECKPOINT (dove è passata l'ultima volta).
    // Scelta struttura dati: std::map garantisce ricerca in O(log N) con chiave la targa(univoca).
    std::map<std::string, CheckpointVeicolo> flotta;

    // Statistiche (Accumulatori)
    // Chiave: ID Varco. Valore: Struct con i contatori.
    std::map<int, StatisticheVarco> dati_statistici;

    // Gestione File e Tempo
    std::ifstream stream_passaggi; 
    std::string path_passaggi;     
    double tempo_corrente;         

    //Metodi privati

    /**
     * Carica la struttura statica da Highway.txt ignorando gli svincoli (non servono).
     * Assegna ID progressivi ai varchi in ordine di apparizione e memorizza le loro distanze in km.
     */
    void carica_autostrada(const std::string& file_path);

    //Funzione helper per convertire l'input utente in secondi.
    double parse_input_tempo(const std::string& input) const;

    /**
     * Viene implementata la logica dei controlli di velocità.
     * Viene chiamata per ogni riga letta da Passages.txt.
     * 1. Controlla se la targa è già in 'flotta'.
     * 2. Se sì: calcola velocità media.
     * 3. Se > 130 km/h: stampa multa.
     * 4. Aggiorna la posizione del veicolo o lo inserisce se nuovo.
     */
    void elabora_passaggio(const std::string& targa, int id_varco, double istante);

    
    //Pulisce le stringhe lette dal file (rimuove < e >).

    std::string pulisci_token(const std::string& token);
};

#endif 