// biblioteca propia de la asignatura (espacio de nombre pel) destinada al
// envío de mensajes con formato a la salida estándar (stdout):
#include "pel_print.hpp"

// ISO C++23:
#include <cstdlib>   // contiene las macros EXIT_SUCCESS y EXIT_FAILURE
#include <algorithm> // contiene algoritmos de ordenación, partición, búsqueda, etc
#include <fstream>   // flujos i/o a ficheros
#include <ranges>    // contienes vistas estándar (e.g., chunk_by)
#include <string>    // string estándar
#include <vector>    // contenedor secuencial estándar recomendado por defecto

// bibliotecas de terceros:
#include <nlohmann/json.hpp> // serialización/deserialización de objetos JSON

std::string const JSON_PATH = "../../roland_garros.jsonl";

struct Target
{
    std::string name;
    std::string country;
    int year;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Target, name, country, year)

// predicado binario para averiguar si dos targets son del mismo tipo (idéntico name):
auto same_name(Target const &t1, Target const &t2) -> bool
{
    return t1.name == t2.name;
}

auto same_country(Target const &t1, Target const &t2) -> bool
{
    return t1.country == t2.country;
}

auto same_year(Target const &t1, Target const &t2) -> bool
{
    return t1.year == t2.year;
}

auto printJsonTarget(const std::vector<Target> &vector)
{
    for (int i = 0; i < (int)vector.size(); i++)
    {
        auto item = vector[i];
        std::cout << item.country << " - " << item.name << " - " << item.year << std::endl;
    }
}

auto main() -> int
{
    // abrimos flujo de entrada (lectura) al fichero JSON LINES:
    auto ifs = std::ifstream{JSON_PATH, std::iostream::binary};
    if (!ifs)
    {
        pel::println("Sorry, unable to open the JSON LINES files");
        return EXIT_FAILURE;
    }

    // vector inicialmente vacío de Targets:
    auto targets = std::vector<Target>{};

    // string auxiliar inicialmente vacío para almacenar líneas del fichero:
    auto ln = std::string{};
    // procesamos cada línea JSON:
    while (std::getline(ifs, ln))
    {
        // deserializamos el objeto JSON contenido en el string ln y obtenemos un
        // Target con la información relevante (name + achieved) en dicha línea:
        auto t = nlohmann::json::parse(ln).get<Target>();
        // guardamos una copia del Target en el vector 'targets':
        targets.push_back(t);
    }

    // cerramos el flujo con el fichero:
    ifs.close();

    // printJsonTarget(targets);

    std::ranges::sort(targets, std::ranges::less{}, &Target::country);

    // bucle for que visita cada subgrupo del vector de enemigos de idéntico tipo
    for (auto target_country : targets | std::views::chunk_by(same_country))
    {
        // tipo de paises contenido en el subgrupo:
        auto const country = std::ranges::begin(target_country)->country;

        pel::println("{}_______________________", country);

        std::ranges::sort(target_country, std::ranges::less{}, &Target::name);
        for (auto target_name : target_country | std::views::chunk_by(same_name))
        {
            auto const name = std::ranges::begin(target_name)->name;
            auto const num_targets = std::ranges::size(target_name);
            pel::println("{:>20} -> {}", name, num_targets);


        }
    }
}