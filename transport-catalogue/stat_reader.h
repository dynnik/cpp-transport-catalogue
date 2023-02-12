#pragma once

#include "transport_catalogue.h"

void Stat_Reader(catalogue::TransportCatalogue t);

void PrintBusInfo(catalogue::BusRoute r);

void PrintStopInfo(const catalogue::StopRoutes& s);
