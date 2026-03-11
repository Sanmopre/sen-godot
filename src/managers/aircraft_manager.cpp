#include "aircraft_manager.h"

void AircraftManager::setInterface(sen::Object* interface)
{
    interface_ = dynamic_cast<rpr::AircraftInterface*>(interface);
}
