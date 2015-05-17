#include "Griffonkitu.h"

Griffonkitu::Griffonkitu(NYWorld *pWorld, NYVert2Df pos) : IABase(pWorld)
{
	//Init FSM
	Initialize();
	type = GRIFFONKITU;
	position = NYVert3Df(pos.X * NYCube::CUBE_SIZE, pos.Y*NYCube::CUBE_SIZE, pWorld->_MatriceHeights[(int)pos.X][(int)pos.Y] * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f + 5 * NYCube::CUBE_SIZE);
	m_huntAreaPoint = NYVert3Df();
	m_s_up = NYVert3Df(0, 0, 1);
	m_radius = NYCube::CUBE_SIZE * 3;
	_spentTime.start();
	ChangeHuntArea();
	m_speed = 30.0f;
	m_huntMod = false;

}

Griffonkitu::~Griffonkitu()
{

}

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
	else if (m_redColor)
		glColor3f(200, 128, 128);
	
	glutSolidCube(NYCube::CUBE_SIZE / 2.0f);

	glPopMatrix();
}

void Griffonkitu::ChangeHuntArea() {
	m_redColor = false;
	int X = rand() % (MAT_SIZE_CUBES - 1);
	int Y = rand() % (MAT_SIZE_CUBES - 1);
	m_huntAreaPoint.X = X*NYCube::CUBE_SIZE;
	m_huntAreaPoint.Y = Y*NYCube::CUBE_SIZE;
	m_huntAreaPoint.Z = m_world->_MatriceHeights[X][Y] * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f + 5 * NYCube::CUBE_SIZE;
}



void Griffonkitu::UpdateIA()
{
	Update();//Update the state machine
}


bool Griffonkitu::States(StateMachineEvent event, MSG_Object *msg, int state)
{
	BeginStateMachine

		State(STATE_Initialize)

		OnEnter{
		//cout << " Initialisation of the state machine d'un Griffonkitu" << endl;
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
		if (m_positionFromHuntAreaPoint.getSize() > m_radius) {
			
			if (m_huntMod)
				m_huntMod = false;
			m_direction = m_positionFromHuntAreaPoint.normalize();
			float scalar =  m_speed *NYRenderer::_DeltaTime;
			m_direction.X *= scalar;
			m_direction.Y *= scalar;
			m_direction.Z *= scalar;
			position = position - m_direction;
		}
		else {
			if (!m_huntMod) {
				m_huntMod = true;
				_spentTime.getElapsedSeconds(true);
			}
			m_redColor = true;
			m_positionFromHuntAreaPoint = m_positionFromHuntAreaPoint.rotate(Griffonkitu::m_s_up, NYRenderer::_DeltaTime * 3.0f);
			position = m_huntAreaPoint + m_positionFromHuntAreaPoint;
			float time = _spentTime.getElapsedSeconds(false);
			if ( time > 15.0f)
				ChangeHuntArea();
			else if (time > 5.0f && !m_hasLooked) {
				for (size_t i = 0; i < (*m_entities)[WASTEDOSAURE].size(); i++)
				{
					NYVert3Df lengthVector = this->position - (*m_entities)[WASTEDOSAURE][i]->position;
					if (lengthVector.getSize() < 50 * NYCube::CUBE_SIZE) {
						setTarget((*m_entities)[WASTEDOSAURE][i]);
						m_path.Clear();
						m_currentIndex = 0;
						if (BresenhamRayCast(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE, position.Z / NYCube::CUBE_SIZE, m_target->position.X / NYCube::CUBE_SIZE, m_target->position.Y / NYCube::CUBE_SIZE, m_target->position.Z / NYCube::CUBE_SIZE))
						{
							PushState(STATE_Attack);
							break;
						}
					}
				}
				m_hasLooked = true;
			}
			else if (time > 10.0f && m_hasLooked) {
				for (size_t i = 0; i < (*m_entities)[WASTEDOSAURE].size(); i++)
				{
					NYVert3Df lengthVector = this->position - (*m_entities)[WASTEDOSAURE][i]->position;
					if (lengthVector.getSize() < 50 * NYCube::CUBE_SIZE) {
						setTarget((*m_entities)[WASTEDOSAURE][i]);
						m_currentIndex = 0;
						if (BresenhamRayCast(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE, position.Z / NYCube::CUBE_SIZE, m_target->position.X / NYCube::CUBE_SIZE, m_target->position.Y / NYCube::CUBE_SIZE, m_target->position.Z / NYCube::CUBE_SIZE))
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
		m_speed = 120.0f;
		cout << "Griffonkitu is attack !" << endl;
	}
	OnUpdate{
		m_positionTarget = position - m_target->position;
		if (m_positionTarget.getSize() < 4.0f) {
			this->SendMsg(MSG_Attack, m_target->GetID(), new int(1));
			ChangeHuntArea();
			m_redColor = false;
			m_attack = false;
			m_speed = 30.0f;
			PushState(STATE_Move);
		}
			m_direction = m_positionTarget.normalize();
		float scalar = m_speed *NYRenderer::_DeltaTime;
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
