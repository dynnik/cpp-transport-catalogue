#pragma once

#include "transport_catalogue.h"

void Stat_Reader(std::istream& in, catalogue::TransportCatalogue t);

void PrintBusInfo(std::ostream& out, catalogue::BusRoute& r);

void PrintStopInfo(std::ostream& out, const catalogue::StopRoutes& s);
