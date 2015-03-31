#include "Ressource.h"


Ressource::Ressource(NYVert3Df position, int maxQuantity)
{
	this->Type = TypeRessource::None;
	this->MaxQuantity = maxQuantity;
	this->Quantity = maxQuantity;
	this->Position = position;
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
