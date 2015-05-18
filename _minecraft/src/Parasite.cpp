#include "Parasite.h"
#include "RessourcesManager.h"

Parasite::Parasite(NYWorld *pWorld, NYVert3Df pos, bool firstBorn) : IABase(pWorld)
{
	//Init FSM
	Initialize();

	//Init type
	type = PARASITE;

	//D�termine si le virus vient d'une crotte
	m_isFirstBorn = firstBorn;

	//Init position
	positionCube.X = (int)pos.X;
	positionCube.Y = (int)pos.Y;
	positionCube.Z = (int)pWorld->_MatriceHeights[(int)pos.X][(int)pos.Y];
	position.X = positionCube.X*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;
	position.Y = positionCube.Y*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;
	position.Z = positionCube.Z*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;

	//Init timer
	m_lastUpdate.start();
}


Parasite::~Parasite()
{

}

void Parasite::SetEntities(std::map<eTypeCreature, CreatureVector> * entities)
{
	m_entities = entities;
}

float Parasite::getSquarredDistance(NYVert3Df* p1, NYVert3Df* p2)
{
	//Retourne la distance au carr� entre 2 points (pas besoin d'avoir la valeur r�elle, inutile de calculer la racine � chaque fois...)
	return (p1->X - p2->X)*(p1->X - p2->X) + (p1->Y - p2->Y)*(p1->Y - p2->Y) + (p1->Z - p2->Z)*(p1->Z - p2->Z);
}

void Parasite::UpdateIA()
{
	//Update FSM
	Update();

	if (m_isSpawner) {

		if (m_timeCheckCrotte >= m_durationCheckCrotte)
		{
			m_timeCheckCrotte = 0.0f; //Reset du compteur
			checkCrottesSpanw();
		}
		m_timeCheckCrotte += m_lastUpdate.getElapsedSeconds();
	}

	//Start timer
	m_lastUpdate.start();
}

void Parasite::Draw()
{
	if (m_currentState != STATE_Dead) {
		glColor3f(2, 2, 2);
		if (m_timeReproduction != 0) { //Si on est en phase de propagation, changement de la couleur du noir (d�but) vers le violet (fin)
			glColor3f(0, 2-m_timeReproduction / 10.f, 0);
		}
		glPushMatrix();
		glTranslatef(position.X, position.Y, position.Z);
		glScalef(1.5f, 1.5f, 1.5f);
		glutSolidOctahedron();
		glPopMatrix();
	}
}

bool Parasite::States(StateMachineEvent event, MSG_Object *msg, int state)
{
	BeginStateMachine

		State(STATE_Initialize)
		OnEnter
	{
			PushState(STATE_Sleep); //Premier �tat : sommeil du virus
	}
		State(STATE_Sleep)
		OnEnter
	{
		m_timeSleep = 0.0f; //Initialisation du compteur de sommeil du virus
	}
		OnUpdate
	{
		//Si le parasite a germ� sur une crotte, il est infectieux directement
		if (m_isFirstBorn == true) {
			PushState(STATE_Reproduction);
		}
		else {//Sinon, il a une phase de sommeil
			if (m_timeSleep >= m_durationSleep)
			{
				PushState(STATE_Reproduction);
			}
			m_timeSleep += m_lastUpdate.getElapsedSeconds();
		}
		//Le parasite suit les mouvements de sa cible (seulement si il n'est pas n� dans de la crotte)
		if (m_isFirstBorn == false) {
			FollowTarget();
		}
	}

		State(STATE_Reproduction)
		OnEnter
	{
		m_timeReproduction = 0.0f; //Initialisation du compteur de temps de contamination
		m_timeCheckProximity = 0.0f; //Initialisation du compteur de v�rification de cr�atures � proximit�
	}

		OnUpdate
	{
		//Le parasite suit les mouvements de sa cible (=> si il n'est pas n� dans de la crotte)
		if (m_isFirstBorn == false) {
			FollowTarget();
		}

		//V�rification des cr�atures � proximit� � intervalles r�guliers (pas trop souvent pour ne pas trop alourdir)
		if (m_timeCheckProximity >= m_durationCheckProximity)
		{
			m_timeCheckProximity = 0.0f; //Reset du compteur
			//InfectCreaturesInArea(m_areaEffect); //Recherche de cr�atures � proximit�
		}
		m_timeCheckProximity += m_lastUpdate.getElapsedSeconds();

		if (m_timeReproduction >= m_durationReproduction)
		{
			if (m_target != NULL) { m_target->infected = false; } //La cible du parasite n'est plus infect�e (elle pourra dont l'�tre de nouveau).
			PushState(STATE_Dead); //Fin de la dur�e de reproduction, le parasite meurt...en esp�rant que sa race perdure gr�ce � ses fr�res...
		}
		m_timeReproduction += m_lastUpdate.getElapsedSeconds();
	}
		State(STATE_Dead)
		OnEnter
	{
		cout << "One parasite is dead, but the war is not over. His brothers will infect you all\n";
	}
		EndStateMachine
}

void Parasite::InfectCreaturesInArea(float sizeArea) {
	//cout << "Recherche de Contamination dans un rayon de " << sizeArea << endl;

	//Parcourt de la liste des cr�atures existantes
	for (int i = 0; i < CREATURE_NUM; ++i)
	{
		eTypeCreature type = (eTypeCreature)i;
		if (type != eTypeCreature::PARASITE) { //Toutes sauf les parasites qui n'ont pas besoin de se parasiter entre eux...
			for (int j = 0; j < (*m_entities)[type].size(); ++j)
			{
				//Si la cr�ature n'est pas infect�e et qu'elle est dans la zone de proximit� du virus
				if ((*m_entities)[type][j]->infected == false && getSquarredDistance(&this->position, &(*m_entities)[type][j]->position) < (sizeArea * 10 * sizeArea * 10)) {
					//cout << "Contamine " << m_type  << " - Distance : "<< getSquarredDistance(&this->position, &(*m_entities)[type][j]->position)<< endl;
					cout << "/!\\/!\\ Contamination en cours /!\\/!\\ " << endl;
					//Cr�ation d'un parasite fils ayant pour cible la cr�ature en cours
					Parasite * p = new Parasite(m_world, (*m_entities)[type][j]->position, false);
					p->m_target = (*m_entities)[type][j]; //Affectation de la cible
					p->m_target->infected = true; //Contamination de la cr�ature
					(*m_entities)[PARASITE].push_back(p); //Ajout du parasite � la liste des parasites du monde
				}
			}
		}
	}
}

void Parasite::FollowTarget() {
	if (m_target != NULL) {
		this->position = m_target->position;//Le parasite se place � la m�me position que sa cible,
		this->position.Z += 10; //un peu au dessus pour �tre visible
		//Il aurait fallu cr�er une variable taille pour chaque cr�ature plut�t que de mettre une taille directement dans le code...
		//�a m'aurait permis de le placer � une hauteur d�pendante de la taille de la cr�ature et pas juste une valeur arbitraire
	}
}

void Parasite::checkCrottesSpanw() {
	//Get through all eatable resources
	RessourceList *pList = RessourcesManager::GetSingleton()->GetRessourcesByType(CROTTE);
	for (auto crotte = pList->begin(); crotte != pList->end(); ++crotte)
	{
		if ((*crotte)->GetHasParasite() == false) {
			Parasite * p = new Parasite(m_world, (*crotte)->Position, true);
			p->position = (*crotte)->Position;
			(*m_entities)[PARASITE].push_back(p); //Ajout du parasite � la liste des parasites du monde
			cout << "Spawn de Parasite sur une crotte � la position" << (*crotte)->Position.X << "," << (*crotte)->Position.Y << "," << (int)m_world->_MatriceHeights[(int)(*crotte)->Position.X][(int)(*crotte)->Position.Y] << endl;
			(*crotte)->SetHasParasite(true);
		}
	}
}