#include "Griffonkitu.h"

Griffonkitu::Griffonkitu(NYWorld *pWorld, NYVert2Df pos) : IABase(pWorld)
{
	//Init FSM
	Initialize();
	type = GRIFFONKITU;
	// on fait spawner les griffons à 5 unitées de haut
	position = NYVert3Df(pos.X * NYCube::CUBE_SIZE, pos.Y*NYCube::CUBE_SIZE, pWorld->_MatriceHeights[(int)pos.X][(int)pos.Y] * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f + 5 * NYCube::CUBE_SIZE);
	m_s_up = NYVert3Df(0, 0, 1);
	//Taille du cercle éfféctué en mode observation
	m_radius = NYCube::CUBE_SIZE * 3;
	_Time.start();
	_spentTime.start();
	ChangeHuntArea();
	m_speed = 30.0f;
	m_observationMod = false;

}

Griffonkitu::~Griffonkitu()
{

}
//Cette fonction détermine la nature des cube sur la ligne entre le griffon et ça cible.
// Je l'utilise uniquement l'orsqu'on est en mode observation et que l'entité à détéctée une cible potentiel
// si il y a autre chose que de l'air entre les deux je retourne false et le griffon ne prend pas en chasse l'entité
bool Griffonkitu::BresenhamRayCast(int x1, int y1, int z1, int x2, int y2, int z2) {
	int i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
	int point[3];

	point[0] = x1;
	point[1] = y1;
	point[2] = z1;
	dx = x2 - x1;
	dy = y2 - y1;
	dz = z2 - z1;
	x_inc = (dx < 0) ? -1 : 1;
	l = abs(dx);
	y_inc = (dy < 0) ? -1 : 1;
	m = abs(dy);
	z_inc = (dz < 0) ? -1 : 1;
	n = abs(dz);
	dx2 = l << 1;
	dy2 = m << 1;
	dz2 = n << 1;

	if ((l >= m) && (l >= n)) {
		err_1 = dy2 - l;
		err_2 = dz2 - l;
		for (i = 0; i < l; i++) {
			if (m_world->getCube(point[0], point[1], point[2])->_Type != CUBE_AIR)
				return false;
			if (err_1 > 0) {
				point[1] += y_inc;
				err_1 -= dx2;
			}
			if (err_2 > 0) {
				point[2] += z_inc;
				err_2 -= dx2;
			}
			err_1 += dy2;
			err_2 += dz2;
			point[0] += x_inc;
		}
	}
	else if ((m >= l) && (m >= n)) {
		err_1 = dx2 - m;
		err_2 = dz2 - m;
		for (i = 0; i < m; i++) {
			if (m_world->getCube(point[0], point[1], point[2])->_Type != CUBE_AIR)
				return false;
			if (err_1 > 0) {
				point[0] += x_inc;
				err_1 -= dy2;
			}
			if (err_2 > 0) {
				point[2] += z_inc;
				err_2 -= dy2;
			}
			err_1 += dx2;
			err_2 += dz2;
			point[1] += y_inc;
		}
	}
	else {
		err_1 = dy2 - n;
		err_2 = dx2 - n;
		for (i = 0; i < n; i++) {
			if (m_world->getCube(point[0], point[1], point[2])->_Type != CUBE_AIR)
				return false;
			if (err_1 > 0) {
				point[1] += y_inc;
				err_1 -= dz2;
			}
			if (err_2 > 0) {
				point[0] += x_inc;
				err_2 -= dz2;
			}
			err_1 += dy2;
			err_2 += dx2;
			point[2] += z_inc;
		}
	}
	if (m_world->getCube(point[0], point[1], point[2])->_Type != CUBE_AIR)
		return false;
	return true;
}

void Griffonkitu::SetEntities(std::map<eTypeCreature, CreatureVector> * entities)
{
	m_entities = entities;
}

void Griffonkitu::Draw(){

	glPushMatrix();
	glTranslatef(position.X, position.Y, position.Z);
	glColor3f(0, 0, 0);
	if (m_attack)
		glColor3f(200, 0, 0);
	else if (m_obsColor)
		glColor3f(200, 128, 128);

	glutSolidCube(NYCube::CUBE_SIZE / 2.0f);

	glPopMatrix();
}

//On défini le centre de la prochaine zone d'Observation du griffon
void Griffonkitu::ChangeHuntArea() {
	m_obsColor = false;
	int X = rand() % (MAT_SIZE_CUBES - 1);
	int Y = rand() % (MAT_SIZE_CUBES - 1);
	m_huntAreaPoint.X = X*NYCube::CUBE_SIZE;
	m_huntAreaPoint.Y = Y*NYCube::CUBE_SIZE;
	// on s'assure que le centre soit au moins à 5 cube de haut.
	m_huntAreaPoint.Z = m_world->_MatriceHeights[X][Y] * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f + 5 * NYCube::CUBE_SIZE;
}



void Griffonkitu::UpdateIA()
{
	// on set les position de l'entité sur la carte
	m_XWorldMap = position.X / NYCube::CUBE_SIZE;
	m_YWorldMap = position.Y / NYCube::CUBE_SIZE;

	//En cas d'erreur de trajectoire, si le griffon est dans le sol on le remonte violament
	if (position.Z / NYCube::CUBE_SIZE < (m_world->_MatriceHeights[m_XWorldMap][m_YWorldMap]))
		position.Z = (m_world->_MatriceHeights[m_XWorldMap][m_YWorldMap] + 1) * NYCube::CUBE_SIZE;
	Update();//Update the state machine
	//reset du timer
	_Time.start();
}


bool Griffonkitu::States(StateMachineEvent event, MSG_Object *msg, int state)
{
	BeginStateMachine

		State(STATE_Initialize)

		OnEnter{
		PushState(STATE_Move);
	}

		State(STATE_FindPath)

		OnEnter{
	}
	OnUpdate{
	}

	State(STATE_Move)

	OnEnter{
	}
	OnUpdate{
		m_positionFromHuntAreaPoint = position - m_huntAreaPoint;
		// Si la distance à partir du centre de la zone d'observation est supérieur au rayon alors on continu de voler vers la zone
		if (m_positionFromHuntAreaPoint.getSize() > m_radius) {

			if (m_observationMod)
				m_observationMod = false;
			m_direction = m_positionFromHuntAreaPoint.normalize();
			
			float scalar = m_speed *_Time.getElapsedSeconds();
			m_direction.X *= scalar;
			m_direction.Y *= scalar;
			m_direction.Z *= scalar;
			position = position - m_direction;
		} // sinon on fait un cercle autour de la zone d'observation
		else {
			if (!m_observationMod) {
				m_observationMod = true;
				_spentTime.getElapsedSeconds(true);
			}
			m_obsColor = true;
			m_positionFromHuntAreaPoint = m_positionFromHuntAreaPoint.rotate(Griffonkitu::m_s_up, _Time.getElapsedSeconds(false) * 3.0f);
			position = m_huntAreaPoint + m_positionFromHuntAreaPoint;
			float time = _spentTime.getElapsedSeconds(false);
			// si le temps est écoulé, on change de zone d'observation
			if (time > 15.0f)
				ChangeHuntArea();
			else if (time > 5.0f && !m_hasLooked) //sinon on vérifie toute les 5 Seconde si il y a pas de Wastedosaure dans à porté
			{
				for (size_t i = 0; i < (*m_entities)[WASTEDOSAURE].size(); i++)
				{
					NYVert3Df lengthVector = this->position - (*m_entities)[WASTEDOSAURE][i]->position;
					if (lengthVector.getSize() < 50 * NYCube::CUBE_SIZE) {
						setTarget((*m_entities)[WASTEDOSAURE][i]);
						m_path.Clear();
						m_currentIndex = 0;
						// Si il ya un Wastedosaure à porté, on regarde si il est directement visible par le griffon
						if (BresenhamRayCast(m_XWorldMap, m_YWorldMap, position.Z / NYCube::CUBE_SIZE, m_target->position.X / NYCube::CUBE_SIZE, m_target->position.Y / NYCube::CUBE_SIZE, m_target->position.Z / NYCube::CUBE_SIZE))
						{
							PushState(STATE_Attack);
							break;
						}
					}
				}
				m_hasLooked = true;
			}
			else if (time > 10.0f && m_hasLooked) { // idem que au dessus (ou je sais la duplication de code ce n'est pas super mais je me garde ça en deux blocks volontairement)
				for (size_t i = 0; i < (*m_entities)[WASTEDOSAURE].size(); i++)
				{
					NYVert3Df lengthVector = this->position - (*m_entities)[WASTEDOSAURE][i]->position;
					if (lengthVector.getSize() < 50 * NYCube::CUBE_SIZE) {
						setTarget((*m_entities)[WASTEDOSAURE][i]);
						m_currentIndex = 0;
						if (BresenhamRayCast(m_XWorldMap, m_YWorldMap, position.Z / NYCube::CUBE_SIZE, m_target->position.X / NYCube::CUBE_SIZE, m_target->position.Y / NYCube::CUBE_SIZE, m_target->position.Z / NYCube::CUBE_SIZE))
						{
							PushState(STATE_Attack);
							break;
						}
					}
				}
				m_hasLooked = false;
			}

		}
	}

		State(STATE_Attack)
		OnEnter{
		m_attack = true;
		// on augmente la vitesse
		m_speed = 60.0f;
		cout << "Griffonkitu attack !" << endl;
	}
		OnUpdate{
		m_positionTarget = position - m_target->position;
		//si il touche ça cible, il repard
		if (m_positionTarget.getSize() < 5.0f) {
			this->SendMsg(MSG_Attack, m_target->GetID(), new int(1));
			ChangeHuntArea();
			m_obsColor = false;
			m_attack = false;
			m_speed = 30.0f;
			PushState(STATE_Move);
		}
		m_direction = m_positionTarget.normalize();
		float scalar = m_speed *_Time.getElapsedSeconds(false);
		m_direction.X *= scalar;
		m_direction.Y *= scalar;
		m_direction.Z *= scalar;
		position = position - m_direction;
	}
		State(STATE_Dead)

		OnEnter{
		cout << "Griffonkitu is dead !" << endl;
	}
		OnUpdate{
	}

	EndStateMachine
}

void Griffonkitu::setTarget(IABase* target){
	m_target = target;
}
