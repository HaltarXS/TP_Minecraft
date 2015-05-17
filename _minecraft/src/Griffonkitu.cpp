#include "Griffonkitu.h"

Griffonkitu::Griffonkitu(NYWorld *pWorld, NYVert2Df pos) : IABase(pWorld)
{
	//Init FSM
	Initialize();

	type = GRIFFONKITU;
	position = NYVert3Df(pos.X * NYCube::CUBE_SIZE, pos.Y*NYCube::CUBE_SIZE, pWorld->_MatriceHeights[(int)pos.X][(int)pos.Y] * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f + 5 * NYCube::CUBE_SIZE);
	m_huntAreaPoint = NYVert3Df();
	m_s_up = NYVert3Df(0, 0, 1);
	m_radius = NYCube::CUBE_SIZE * 10;
	_spentTime.start();
	ChangeHuntArea();
	m_speed = 30.0f;
	m_huntMod = false;

}

Griffonkitu::~Griffonkitu()
{

}

void Griffonkitu::SetEntities(std::map<eTypeCreature, CreatureVector> * entities)
{
	m_entities = entities;
}

void Griffonkitu::Draw(){

	glColor3f(128, 128, 128);
	glPushMatrix();

	glTranslatef(position.X, position.Y, position.Z);
	glutSolidCube(NYCube::CUBE_SIZE / 2.0f);

	glPopMatrix();
}

void Griffonkitu::ChangeHuntArea() {
	m_huntAreaPoint.X = (rand() % MAT_SIZE_CUBES)*NYCube::CUBE_SIZE;
	m_huntAreaPoint.Y = (rand() % MAT_SIZE_CUBES)*NYCube::CUBE_SIZE;
	m_huntAreaPoint.Z = position.Z;
}



void Griffonkitu::UpdateIA()
{
	Update();//Update the state machine
}

void Griffonkitu::LookTarget() {
	/*
	for (size_t i = 0; i < (*m_entities)[WASTEDOSAURE].size(); i++)
	{
		NYVert3Df lengthVector = this->position - (*m_entities)[WASTEDOSAURE][i]->position;

	}
	*/
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
		if (m_positionFromHuntAreaPoint.getMagnitude() > m_radius) {
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
			
			m_positionFromHuntAreaPoint = m_positionFromHuntAreaPoint.rotate(Griffonkitu::m_s_up, NYRenderer::_DeltaTime * 3.0f);
			position = m_huntAreaPoint + m_positionFromHuntAreaPoint;
			float time = _spentTime.getElapsedSeconds(false);
			if ( time > 15.0f)
				ChangeHuntArea();
			else if (time > 5.0f) {
				LookTarget();
			}
			else if (time > 10.0f) {
				LookTarget();
			}

		}
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
	_target = target;
}
