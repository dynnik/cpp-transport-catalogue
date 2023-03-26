#include "geo.h"
#include "transport_catalogue.h"
#include "stat_reader.h"
#include <iostream>

int main() {
    InputResult res;
    InputReader(std::cin, res);
    catalogue::TransportCatalogue t(res);
    Stat_Reader(std::cin, t);
    return 0;
}
