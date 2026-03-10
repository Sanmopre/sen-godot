//
// Created by sanmopre on 10/3/26.
//

#include "aircraft_manager.h"

void AircraftManager::setInterface(sen::Object* interface)
{
    interface_ = dynamic_cast<std_fom::AircraftInterface*>(interface);
}
