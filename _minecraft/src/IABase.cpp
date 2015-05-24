#include "IABase.h"


IABase::IABase(NYWorld * world):
StateMachine()
{
	A = 5;
	B = 5;
	t = 0;
	saciete = 0;
	faim = 0;
	position = NYVert3Df(0, 0, 0);
	positionCube = NYVert3Df(0, 0, 0);
	direction = NYVert3Df(0, 0, 0);
	speed = NYVert3Df(0, 0, 0);

	type = BASE;

	m_world = world;
}


IABase::~IABase()
{
}

void IABase::UpdateHunger(float elapsed, float totalTime)
{
	if (saciete > 0.0)
	{
		saciete -= elapsed;
	}
	else
	{
		t += elapsed;
	}
	this->faim = A*t + B*t*(cos(totalTime) + 1);
}

void IABase::Manger()
{
	saciete = m_saciete_Time;
	t = 0;
}

const char* IABase::GetName() const{
	static char WASTEDOSAURE_NAME[] = "Wastedosaure";
	static char	DAHUT_NAME[] = "Dahut";
	static char CAMELEON_NAME[] = "Cameleon";
	static char	MOUCHE_NAME[] = "Mouche";
	static char	LEMMING_NAME[] = "Lemming";
	static char	GLACEGOUILLE_NAME[] = "Glacegouille";
	static char	GENDAMOUR_NAME[] = "Gendamour";
	static char	SNAKE_NAME[] = "Snake";
	static char	NEON_NAME[] = "Neon";
	static char	GRIFFONKITU_NAME[] = "Griffonkitu";
	static char	FURZ_NAME[] = "Furz";
	static char	CRABE_NAME[] = "Crabe";
	static char	PARASITE_NAME[] = "Parasite";
	static char	VAUTOUR_NAME[] = "Vautour";
	static char	GEVAULOL_NAME[] = "Gevaulol";
	static char	NEON_BLEU_NAME[] = "Neon Bleu";
	static char	BIXI_NAME[] = "Bixi";
	static char	NOKIA_NAME[] = "Nokia";
	static char	YETI_NAME[] = "Yeti";
	static char	RADIATOSAURE_NAME[] = "Radiatosaure";
	static char UNKNOWN[] = "Unknown";
	switch (type){
	case WASTEDOSAURE: return WASTEDOSAURE_NAME;
	case DAHUT: return DAHUT_NAME;
	case CAMELEON: return CAMELEON_NAME;
	case MOUCHE: return MOUCHE_NAME;
	case LEMMING: return LEMMING_NAME;
	case GLACEGOUILLE: return GLACEGOUILLE_NAME;
	case GENDAMOUR: return GENDAMOUR_NAME;
	case SNAKE: return SNAKE_NAME;
	case NEON: return NEON_NAME;
	case GRIFFONKITU: return GRIFFONKITU_NAME;
	case FURZ: return FURZ_NAME;
	case CRABE: return CRABE_NAME;
	case PARASITE: return PARASITE_NAME;
	case VAUTOUR: return VAUTOUR_NAME;
	case GEVAULOL: return GEVAULOL_NAME;
	case NEON_BLEU: return NEON_BLEU_NAME;
	case BIXI: return BIXI_NAME;
	case NOKIA: return NOKIA_NAME;
	case YETI: return YETI_NAME;
	case RADIATOSAURE: return RADIATOSAURE_NAME;
	default: return UNKNOWN;
	}
}
