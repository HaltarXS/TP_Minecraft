#include "Gevaulol.h"

std::map<eTypeCreature, std::vector<IABase*>>* Gevaulol::creatureMap;

Gevaulol::Gevaulol(NYWorld *pWorld, NYVert2Df pos) : IABase(pWorld){

}

void Gevaulol::UpdateIA(){

}
void Gevaulol::Draw(){

}
bool Gevaulol::States(StateMachineEvent event, MSG_Object *msg, int state){
	return false;
}