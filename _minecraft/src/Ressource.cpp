#include "Ressource.h"

Ressource::Ressource()
{
	this->Type = TypeRessource::NONE;
}
Ressource::Ressource(NYVert3Df position, int maxQuantity)
{
	this->Type = TypeRessource::NONE;
	this->MaxQuantity = maxQuantity;
	this->Quantity = maxQuantity;
	this->Position = position;
	this->CubePostion = position / 10;
}


Ressource::~Ressource()
{
}

int Ressource::Use(int neededQuantity)
{
	int retQ = neededQuantity;
	if (this->Quantity != ILLIMITED_QUANTITY)
	{
		retQ = min(neededQuantity, this->Quantity);
		this->Quantity = max(0, this->Quantity - neededQuantity);
	}
	return retQ;
}
bool Ressource::GetHasParasite()
{
	return m_hasParasite;
}
void Ressource::SetHasParasite(bool val)
{
	m_hasParasite = val;
}
