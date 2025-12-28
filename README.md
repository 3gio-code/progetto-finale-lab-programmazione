# progetto-finale-lab-programmazione

#include <iostream>
#include <chrono>
#include <ctime> // Serve per convertire il tempo in formato leggibile
#include <iomanip> // Serve per std::put_time

int main() {
// 1. Salva data e ora attuale in una variabile
// 'tp' sta per Time Point (Punto nel tempo)
auto adesso = std::chrono::system_clock::now();

    // 2. Definisci quanti secondi vuoi aggiungere (es. 3600 secondi = 1 ora)
    int secondiDaAggiungere = 3600;

    // 3. Crea la nuova variabile "futura"
    // Basta fare una semplice somma: data + durata
    auto futuro = adesso + std::chrono::seconds(secondiDaAggiungere);

    // --- FINE LOGICA, ORA STAMPIAMO I RISULTATI ---

    // Per stampare, dobbiamo convertire il "tempo macchina" in "tempo umano" (time_t)
    std::time_t tempoAdesso = std::chrono::system_clock::to_time_t(adesso);
    std::time_t tempoFuturo = std::chrono::system_clock::to_time_t(futuro);

    std::cout << "Orario Attuale: "
              << std::put_time(std::localtime(&tempoAdesso), "%Y-%m-%d %H:%M:%S")
              << std::endl;

    std::cout << "Orario Futuro (+ " << secondiDaAggiungere << " sec): "
              << std::put_time(std::localtime(&tempoFuturo), "%Y-%m-%d %H:%M:%S")
              << std::endl;

    return 0;

}
