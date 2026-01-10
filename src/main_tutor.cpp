#include "Tutor.h"
#include <iostream>
//nel caso si trovi qualcosa nel buffer di ingresso
#include <limits> 

int main() {

    // estrapolazione informazioni da file
    const std::string file_autostrada = "data/Highway.txt";
    const std::string file_passaggi = "data/Passages.txt";

    std::cout << "SISTEMA TUTOR AUTOSTRADALE Faggin_Birtele_DallaMura" << std::endl;

    try {
        
        //creazione oggetto della classe Tutor
        Tutor tutor(file_autostrada, file_passaggi);

        //stringa che viene usata per interagire con l'utente
        std::string comando;
        //variabile bool per uscire o fare andare avanti il programma
        bool running = true;

        std::cout << std::endl << "Comandi disponibili:" << std::endl
                  << "  set_time tempo : avanza simulazione (aggiungere m se si intende avanzare di minuti e non secondi)" << std::endl
                  << "  stats           : visualizza statistiche varchi" << std::endl
                  << "  reset           : riavvia il sistema" << std::endl
                  << "  exit            : chiude il programma" << std::endl;

        // ciclo while di interazione con l'utente
        while (running) {
            std::cout << "Inserire comando:  ";
            std::cin >> comando;

            // gestione input utente
            if (comando == "set_time") {
                std::string tempo_input;
                std::cin >> tempo_input; // Legge l'argomento (es. "20m")
                tutor.set_time(tempo_input);
            } 
            else if (comando == "stats") {
                tutor.stats();
            } 
            else if (comando == "reset") {
                tutor.reset();
            } 
            else if (comando == "exit") {
                running = false;
                std::cout << "Fine programma." << std::endl;
            } 
            else {
                std::cout << "Comando non riconosciuto." << std::endl;
                // Pulisce il buffer in caso di input sporco
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "ERRORE CRITICO: " << e.what() << std::endl;
        std::cerr << "Verifica che la cartella 'data' esista e contenga Highway.txt e Passages.txt" << std::endl;
        return 1;
    }

    return 0;
}