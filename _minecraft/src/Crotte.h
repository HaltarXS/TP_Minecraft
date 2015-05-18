#pragma once
#include "Ressource.h"
#include "cube.h"

class Crotte : public Ressource
{
	private:
		bool _hasFly;
	public:
		static const int CROTTE_SIZE = NYCube::CUBE_SIZE / 3.0f;

		Crotte(NYVert3Df position, int maxQuantity);
		~Crotte();

		virtual void Update(float deltaTime);
		virtual void Render();

		bool Crotte::GetHasFly();
		void Crotte::SetHasFly(bool val);

		virtual bool Crotte::GetHasParasite();
		virtual void Crotte::SetHasParasite(bool val);
};

