#ifndef __AVATAR__
#define __AVATAR__

#include "engine/utils/types_3d.h"
#include "engine/render/camera.h"
#include "world.h"

class NYAvatar
{
public :
	NYVert3Df Position;
	NYVert3Df Speed;

	NYVert3Df MoveDir;
	bool Move;
	bool Jump;
	float Height;
	float Width;
	bool avance;
	bool recule;
	bool gauche;
	bool droite;
	bool Standing;
	bool Destroying;

	NYCamera * Cam;
	NYWorld * World;

	NYAvatar(NYCamera * cam,NYWorld * world)
	{
		Position = NYVert3Df(110,110,(world->_MatriceHeights[11][11]*NYCube::CUBE_SIZE));
		Height = 10;
		Width = 3;
		Cam = cam;
		avance = false;
		recule = false;
		gauche = false;
		droite = false;
		Standing = false;
		Jump = false;
		World = world;
	}


	void render(void)
	{
		glPushMatrix();
		glTranslatef(Position.X,Position.Y,Position.Z);
		//glutSolidCube(Width/2);
		glPopMatrix();

		////glDisable(GL_LIGHTING);
		////glBegin(GL_LINES);
		//////glColor3d(1,0,0);
		////NYVert3Df B = Position;
		////B.X += Cam->_Direction.X*NYCube::CUBE_SIZE*8;
		////B.Y += Cam->_Direction.Y*NYCube::CUBE_SIZE*8;
		////B.Z += Cam->_Direction.Z*NYCube::CUBE_SIZE*8;
		//////glVertex3d(Position.X,Position.Y,Position.Z);
		//////glVertex3d(B.X-5,B.Y,B.Z-5);
		////glEnd();
		//glEnable(GL_LIGHTING);
		//if (Destroying) {
		//	NYVert3Df sp = NYVert3Df(0, 0, 0);
		//	int x, y, z;
		//	if (World->getRayCollision(Position, B, sp, x, y, z))
		//	{
		//		World->deleteCube(x, y, z);
		//		World->updateCube(x,y,z);
		//	}
		//}
	}

	void update(float elapsed)
	{
		Destroying = false;
		//Par defaut, on applique la gravit� (-100 sur Z)
		NYVert3Df force = NYVert3Df(0,0,-1) * 100.0f;

		//Si l'avatar n'est pas au sol, alors il ne peut pas sauter
		if(!Standing)
			Jump = false;


		//Si il est au sol, on applique les controles "ground"
		if(Standing)	
		{
			if(avance)
				force += Cam->_Direction * 400;
			if(recule)
				force += Cam->_Direction * -400;
			if(gauche)
				force += Cam->_NormVec * -400;
			if(droite)
				force += Cam->_NormVec * 400;	
		}
		else //Si il est en l'air, c'est du air control
		{
			if(avance)
				force += Cam->_Direction * 50;
			if(recule)
				force += Cam->_Direction * -50;
			if(gauche)
				force += Cam->_NormVec * -50;
			if(droite)
				force += Cam->_NormVec * 50;
		}

		//On applique le jump
		if(Jump)
		{
			force += NYVert3Df(0,0,1) * 50.0f / elapsed; //(impulsion, pas fonction du temps)
			Jump = false;
		}

		//On applique les forces en fonction du temps �coul�
		Speed += force * elapsed;

		//On met une limite a sa vitesse horizontale
		NYVert3Df horSpeed = Speed;
		horSpeed.Z = 0;
		if(horSpeed.getSize() > 70.0f)
		{
			horSpeed.normalize();
			horSpeed *= 70.0f;
			Speed.X = horSpeed.X;
			Speed.Y = horSpeed.Y;
		}

		//On le d�place, en sauvegardant son ancienne position
		NYVert3Df oldPosition = Position;
		Position += (Speed * elapsed);

		//On recup la collision a la nouvelle position
		NYCollision collidePrinc = 0x00;
		NYCollision collide = World->collide_with_world(Position,Width,Height,collidePrinc);
		if(collide & NY_COLLIDE_BOTTOM && Speed.Z < 0)
		{
			Position.Z = oldPosition.Z;
			Speed *= pow(0.01f,elapsed);
			Speed.Z = 0;
			Standing = true;
		}
		else
			Standing = false;

		if(collide & NY_COLLIDE_UP && !Standing && Speed.Z > 0)
		{
			Position.Z = oldPosition.Z;
			Speed.Z = 0;
		}

		//On a regle le probleme du bottom et up, on g�re les collision sur le plan (x,y)
		collide = World->collide_with_world(Position,Width,Height,collidePrinc);

		//En fonction des cot�s, on annule une partie des d�placements
		if(collide & NY_COLLIDE_BACK && collide & NY_COLLIDE_RIGHT && collide & NY_COLLIDE_LEFT)
		{
			Position.Y = oldPosition.Y;
			Speed.Y = 0;
		}

		if(collide & NY_COLLIDE_FRONT && collide & NY_COLLIDE_RIGHT && collide & NY_COLLIDE_LEFT)
		{
			Position.Y = oldPosition.Y;
			Speed.Y = 0;
		}

		if(collide & NY_COLLIDE_RIGHT && collide & NY_COLLIDE_FRONT && collide & NY_COLLIDE_BACK)
		{
			Position.X = oldPosition.X;
			Speed.X = 0;
		}

		if(collide & NY_COLLIDE_LEFT && collide & NY_COLLIDE_FRONT && collide & NY_COLLIDE_BACK)
		{
			Position.X = oldPosition.X;
			Speed.X = 0;
		}

		//Si je collide sur un angle
		if(!(collide & NY_COLLIDE_BACK && collide & NY_COLLIDE_FRONT) && !(collide & NY_COLLIDE_LEFT && collide & NY_COLLIDE_RIGHT))
			if(collide & (NY_COLLIDE_BACK | NY_COLLIDE_FRONT | NY_COLLIDE_RIGHT | NY_COLLIDE_LEFT))
			{
				Position.Y = oldPosition.Y;
				Position.X = oldPosition.X;
			}
		NYVert3Df posCam = Position;
		posCam.Z +=NYCube::CUBE_SIZE/2;
		
		Cam->moveTo(posCam);

		//cout << "Current Cube : " << Position.X / NYCube::CUBE_SIZE << "," << Position.Y / NYCube::CUBE_SIZE << "," << Position.Z / NYCube::CUBE_SIZE << endl;
	}
};

#endif