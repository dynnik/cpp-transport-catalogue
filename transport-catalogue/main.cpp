#include "geo.h"
#include "input_reader.h"
#include "transport_catalogue.h"
#include "stat_reader.h"

int main(){
    catalogue::TransportCatalogue t(InputReader());
    Stat_Reader(t);
    return 0;
}
