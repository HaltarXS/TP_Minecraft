#ifndef __WORLD_H__
#define __WORLD_H__

#include "gl/glew.h"
#include "gl/freeglut.h" 
#include "engine/utils/types_3d.h"
#include "cube.h"
#include "chunk.h"
#include "my_physics.h"
#include "engine/render/graph/tex_manager.h"



typedef uint8 NYCollision;
#define NY_COLLIDE_UP     0x01
#define NY_COLLIDE_BOTTOM 0x02
#define NY_COLLIDE_LEFT   0x04
#define NY_COLLIDE_RIGHT  0x08
#define NY_COLLIDE_FRONT  0x10
#define NY_COLLIDE_BACK   0x20
#define NY_COLLIDE_IN     0x40

typedef uint8 NYAxis;
#define NY_AXIS_X 0x01
#define NY_AXIS_Y 0x02
#define NY_AXIS_Z 0x04

#define MAT_SIZE 1 //en nombre de chunks
#define MAT_HEIGHT 1 //en nombre de chunks
#define MAT_SIZE_CUBES (MAT_SIZE * NYChunk::CHUNK_SIZE)
#define MAT_HEIGHT_CUBES (MAT_HEIGHT * NYChunk::CHUNK_SIZE)


class NYWorld
{
public :
	NYChunk * _Chunks[MAT_SIZE][MAT_SIZE][MAT_HEIGHT];
	int _MatriceHeights[MAT_SIZE_CUBES][MAT_SIZE_CUBES];
	float _FacteurGeneration;

	NYTexFile * _TexGrass;

	NYWorld()
	{
		_FacteurGeneration = 1.0;

		//On crée les chunks
		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
					_Chunks[x][y][z] = new NYChunk();

		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
				{
					NYChunk * cxPrev = NULL;
					if(x > 0)
						cxPrev = _Chunks[x-1][y][z];
					NYChunk * cxNext = NULL;
					if(x < MAT_SIZE-1)
						cxNext = _Chunks[x+1][y][z];

					NYChunk * cyPrev = NULL;
					if(y > 0)
						cyPrev = _Chunks[x][y-1][z];
					NYChunk * cyNext = NULL;
					if(y < MAT_SIZE-1)
						cyNext = _Chunks[x][y+1][z];

					NYChunk * czPrev = NULL;
					if(z > 0)
						czPrev = _Chunks[x][y][z-1];
					NYChunk * czNext = NULL;
					if(z < MAT_HEIGHT-1)
						czNext = _Chunks[x][y][z+1];

					_Chunks[x][y][z]->setVoisins(cxPrev,cxNext,cyPrev,cyNext,czPrev,czNext);
				}

		_TexGrass = NYTexManager::getInstance()->loadTexture(std::string("textures.png"));

	}

	inline NYCube * getCube(int x, int y, int z)
	{	
		if(x < 0)x = 0;
		if(y < 0)y = 0;
		if(z < 0)z = 0;
		if(x >= MAT_SIZE * NYChunk::CHUNK_SIZE) x = (MAT_SIZE * NYChunk::CHUNK_SIZE)-1;
		if(y >= MAT_SIZE * NYChunk::CHUNK_SIZE) y = (MAT_SIZE * NYChunk::CHUNK_SIZE)-1;
		if(z >= MAT_HEIGHT * NYChunk::CHUNK_SIZE) z = (MAT_HEIGHT * NYChunk::CHUNK_SIZE)-1;

		return &(_Chunks[x / NYChunk::CHUNK_SIZE][y / NYChunk::CHUNK_SIZE][z / NYChunk::CHUNK_SIZE]->_Cubes[x % NYChunk::CHUNK_SIZE][y % NYChunk::CHUNK_SIZE][z % NYChunk::CHUNK_SIZE]);
	}

	void updateCube(int x, int y, int z)
	{	
		if(x < 0)x = 0;
		if(y < 0)y = 0;
		if(z < 0)z = 0;
		if(x >= MAT_SIZE * NYChunk::CHUNK_SIZE)x = (MAT_SIZE * NYChunk::CHUNK_SIZE)-1;
		if(y >= MAT_SIZE * NYChunk::CHUNK_SIZE)y = (MAT_SIZE * NYChunk::CHUNK_SIZE)-1;
		if(z >= MAT_HEIGHT * NYChunk::CHUNK_SIZE)z = (MAT_HEIGHT * NYChunk::CHUNK_SIZE)-1;

		NYChunk * chk = _Chunks[x / NYChunk::CHUNK_SIZE][y / NYChunk::CHUNK_SIZE][z / NYChunk::CHUNK_SIZE];

		chk->disableHiddenCubes();
		chk->toVbo();

		for(int i=0;i<6;i++)
			if(chk->Voisins[i])
			{
				chk->Voisins[i]->disableHiddenCubes();
				chk->Voisins[i]->toVbo();
			}
	}

	void deleteCube(int x, int y, int z)
	{
		NYCube * cube = getCube(x,y,z);
		cube->_Draw = false;
		cube->_Type = CUBE_AIR;
		updateCube(x,y,z);	
	}

	//Création d'une pile de cubes
	//only if zero permet de ne générer la  pile que si sa hauteur actuelle est de 0 (et ainsi de ne pas regénérer de piles existantes)
	void load_pile(int x, int y, int height, bool onlyIfZero = true)
	{
		if(height < 1)
			height = 1;
		if(height >= MAT_HEIGHT_CUBES)
			height = MAT_HEIGHT_CUBES-1;

		if(_MatriceHeights[x][y] != 0 && onlyIfZero)
			return;

		for(int z=0;z<height;z++)
		{
			getCube(x,y,z)->_Draw = true;
			if(z>0)
				getCube(x,y,z)->_Type = CUBE_TERRE;
			else
				getCube(x,y,z)->_Type = CUBE_EAU;
		}

		if(height-1>0)
		{
			if(height >= 32)
			{
				getCube(x,y,height-1)->_Draw = true;
				getCube(x,y,height-1)->_Type = CUBE_NEIGE;
			}
			else
			{
				getCube(x,y,height-1)->_Draw = true;
				getCube(x,y,height-1)->_Type = CUBE_HERBE;
			}
		}
		for(int z=height;z<MAT_HEIGHT_CUBES;z++)
		{
			getCube(x,y,z)->_Draw = true;
			getCube(x,y,z)->_Type = CUBE_AIR;
		}

		_MatriceHeights[x][y] = height;
	}

	//Creation du monde entier, en utilisant le mouvement brownien fractionnaire
	void generate_piles(int x1, int y1,
		int x2, int y2, 
		int x3, int y3,
		int x4, int y4, int prof, int profMax = -1)
	{
		int r =4, l=2;
		if(prof == profMax || (x3-x1)<=1 && (y3-y1)<=1)
			return;
		int x21 = (x2 + x1)/2;
		int y21 = (y2 + y1)/2;
		int z21= (( _MatriceHeights[x2][y2] + _MatriceHeights[x1][y1] )/2) +(rand()%r)-l;
		load_pile(x21,y21,z21);

		int x32 = (x3 + x2)/2;
		int y32 = (y3 + y2)/2;
		int z32= (( _MatriceHeights[x3][y3] + _MatriceHeights[x2][y2] )/2) +(rand()%r)-l;
		load_pile(x32,y32,z32);

		int x43 = (x4 + x3)/2;
		int y43 = (y4 + y3)/2;
		int z43= (( _MatriceHeights[x4][y4] + _MatriceHeights[x3][y3] )/2) +(rand()%r)-l;
		load_pile(x43,y43,z43);

		int x14 = (x1 + x4)/2;
		int y14 = (y1 + y4)/2;
		int z14= (( _MatriceHeights[x1][y1] + _MatriceHeights[x4][y4] )/2) +(rand()%r)-l;
		load_pile(x14,y14,z14);

		int xmid = (x21+x32+x43+x14)/4;
		int ymid = (y21+y32+y43+y14)/4;
		int zmid = (( _MatriceHeights[x14][y14] + _MatriceHeights[x43][y43] + _MatriceHeights[x32][y32]+ _MatriceHeights[x21][y21])/4) +(rand()%r)-l;
		if(prof == 1)
		{
			zmid= 0;
		}
		load_pile(xmid,ymid,zmid);

		generate_piles(x1,y1,x21,y21,xmid,ymid,x14,y14,prof+1,profMax);
		generate_piles(x21,y21,x2,y2,x32,y32,xmid,ymid,prof+1,profMax);
		generate_piles(x14,y14,xmid,ymid,x43,y43,x4,y4,prof+1,profMax);
		generate_piles(xmid,ymid,x32,y32,x3,y3,x43,y43,prof+1,profMax);
	}


	void lisse(void)
	{
		int _MatriceHeightsTmp[MAT_SIZE_CUBES][MAT_SIZE_CUBES];
		int sizeWidow = 4;
		memset(_MatriceHeightsTmp,0x00,sizeof(int)*MAT_SIZE_CUBES*MAT_SIZE_CUBES);
		for(int x=0;x<MAT_SIZE_CUBES;x++)
		{
			for(int y=0;y<MAT_SIZE_CUBES;y++)
			{
				//on moyenne sur une distance
				int nb = 0;
				for(int i=(x-sizeWidow < 0 ? 0 : x-sizeWidow) ; 
					i < (x+sizeWidow >= MAT_SIZE_CUBES ? MAT_SIZE_CUBES-1 : x+sizeWidow) ; i++)
				{
					for(int j=(y-sizeWidow < 0 ? 0 : y-sizeWidow) ;
						j <( y+sizeWidow >= MAT_SIZE_CUBES ? MAT_SIZE_CUBES-1 : y+sizeWidow) ; j++)
					{
						_MatriceHeightsTmp[x][y] += _MatriceHeights[i][j];
						nb++;
					}
				}
				if(nb)
					_MatriceHeightsTmp[x][y] /= nb;
			}
		}

		//On reset les piles
		for(int x=0;x<MAT_SIZE_CUBES;x++)
		{
			for(int y=0;y<MAT_SIZE_CUBES;y++)
			{
				load_pile(x,y,_MatriceHeightsTmp[x][y],false);
			}
		}

		/*for(int x=1;x<MAT_SIZE_CUBES-1;x++)
		for(int y=1;y<MAT_SIZE_CUBES-1;y++)
		{
		int height = 0;
		height += _MatriceHeights[x-1][y-1];
		height += _MatriceHeights[x-1][y];
		height += _MatriceHeights[x-1][y+1];
		height += _MatriceHeights[x][y-1];
		height += _MatriceHeights[x][y];
		height += _MatriceHeights[x][y+1];
		height += _MatriceHeights[x+1][y-1];
		height += _MatriceHeights[x+1][y];
		height += _MatriceHeights[x+1][y+1];
		load_pile(x,y,height/9,false);
		}*/
	}




	void init_world(int profmax = -1)
	{
		_cprintf("Creation du monde %f \n",_FacteurGeneration);

		srand(6665);

		//Reset du monde
		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
					_Chunks[x][y][z]->reset();
		memset(_MatriceHeights,0x00,MAT_SIZE_CUBES*MAT_SIZE_CUBES*sizeof(int));

		//On charge les 4 coins
		load_pile(0,0,MAT_HEIGHT_CUBES/2);
		load_pile(MAT_SIZE_CUBES-1,0,MAT_HEIGHT_CUBES/2);
		load_pile(MAT_SIZE_CUBES-1,MAT_SIZE_CUBES-1,MAT_HEIGHT_CUBES/2);	
		load_pile(0,MAT_SIZE_CUBES-1,MAT_HEIGHT_CUBES/2);

		//On génère a partir des 4 coins
		generate_piles(0,0,
			MAT_SIZE_CUBES-1,0,
			MAT_SIZE_CUBES-1,MAT_SIZE_CUBES-1,
			0,MAT_SIZE_CUBES-1,1,profmax);	

		lisse();
		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
					_Chunks[x][y][z]->disableHiddenCubes();
		add_world_to_vbo();


	}

	NYCube * pick(NYVert3Df  pos, NYVert3Df  dir, NYPoint3D * point)
	{
		return NULL;
	}

	//Boites de collisions plus petites que deux cubes
	NYAxis getMinCol(NYVert3Df pos, float width, float height, float & valueColMin, int i)
	{

		int x = (int)(pos.X / NYCube::CUBE_SIZE);
		int y = (int)(pos.Y / NYCube::CUBE_SIZE);
		int z = (int)(pos.Z / NYCube::CUBE_SIZE);

		int xNext = (int)((pos.X + width / 2.0f) / NYCube::CUBE_SIZE);
		int yNext = (int)((pos.Y + width / 2.0f) / NYCube::CUBE_SIZE);
		int zNext = (int)((pos.Z + height / 2.0f) / NYCube::CUBE_SIZE);

		int xPrev = (int)((pos.X - width / 2.0f) / NYCube::CUBE_SIZE);
		int yPrev = (int)((pos.Y - width / 2.0f) / NYCube::CUBE_SIZE);
		int zPrev = (int)((pos.Z - height / 2.0f) / NYCube::CUBE_SIZE);

		if (x < 0)	x = 0;
		if (y < 0)	y = 0;
		if (z < 0)	z = 0;

		if (xPrev < 0)	xPrev = 0;
		if (yPrev < 0)	yPrev = 0;
		if (zPrev < 0)	zPrev = 0;

		if (xNext < 0)	xNext = 0;
		if (yNext < 0)	yNext = 0;
		if (zNext < 0)	zNext = 0;

		if (x >= MAT_SIZE_CUBES)	x = MAT_SIZE_CUBES - 1;
		if (y >= MAT_SIZE_CUBES)	y = MAT_SIZE_CUBES - 1;
		if (z >= MAT_HEIGHT_CUBES)	z = MAT_HEIGHT_CUBES - 1;

		if (xPrev >= MAT_SIZE_CUBES)	xPrev = MAT_SIZE_CUBES - 1;
		if (yPrev >= MAT_SIZE_CUBES)	yPrev = MAT_SIZE_CUBES - 1;
		if (zPrev >= MAT_HEIGHT_CUBES)	zPrev = MAT_HEIGHT_CUBES - 1;

		if (xNext >= MAT_SIZE_CUBES)	xNext = MAT_SIZE_CUBES - 1;
		if (yNext >= MAT_SIZE_CUBES)	yNext = MAT_SIZE_CUBES - 1;
		if (zNext >= MAT_HEIGHT_CUBES)	zNext = MAT_HEIGHT_CUBES - 1;

		//On fait chaque axe
		NYAxis axis = 0x00;
		valueColMin = 10000.0f;

		//On verif tout les 4 angles de gauche
		if (getCube(xPrev, yPrev, zPrev)->isSolid() ||
			getCube(xPrev, yPrev, zNext)->isSolid() ||
			getCube(xPrev, yNext, zPrev)->isSolid() ||
			getCube(xPrev, yNext, zNext)->isSolid())
		{
			float depassement = ((xPrev + 1) * NYCube::CUBE_SIZE) - (pos.X - width / 2.0f);
			if (abs(depassement) < abs(valueColMin))
			{
				valueColMin = depassement;
				axis = NY_AXIS_X;
			}
		}

		float depassementx2 = (xNext * NYCube::CUBE_SIZE) - (pos.X + width / 2.0f);

		//On verif tout les 4 angles de droite
		if (getCube(xNext, yPrev, zPrev)->isSolid() ||
			getCube(xNext, yPrev, zNext)->isSolid() ||
			getCube(xNext, yNext, zPrev)->isSolid() ||
			getCube(xNext, yNext, zNext)->isSolid())
		{
			float depassement = (xNext * NYCube::CUBE_SIZE) - (pos.X + width / 2.0f);
			if (abs(depassement) < abs(valueColMin))
			{
				valueColMin = depassement;
				axis = NY_AXIS_X;
			}
		}

		float depassementy1 = (yNext * NYCube::CUBE_SIZE) - (pos.Y + width / 2.0f);

		//On verif tout les 4 angles de devant
		if (getCube(xPrev, yNext, zPrev)->isSolid() ||
			getCube(xPrev, yNext, zNext)->isSolid() ||
			getCube(xNext, yNext, zPrev)->isSolid() ||
			getCube(xNext, yNext, zNext)->isSolid())
		{
			float depassement = (yNext * NYCube::CUBE_SIZE) - (pos.Y + width / 2.0f);
			if (abs(depassement) < abs(valueColMin))
			{
				valueColMin = depassement;
				axis = NY_AXIS_Y;
			}
		}

		float depassementy2 = ((yPrev + 1) * NYCube::CUBE_SIZE) - (pos.Y - width / 2.0f);

		//On verif tout les 4 angles de derriere
		if (getCube(xPrev, yPrev, zPrev)->isSolid() ||
			getCube(xPrev, yPrev, zNext)->isSolid() ||
			getCube(xNext, yPrev, zPrev)->isSolid() ||
			getCube(xNext, yPrev, zNext)->isSolid())
		{
			float depassement = ((yPrev+1) * NYCube::CUBE_SIZE) - (pos.Y - width / 2.0f);
			if (abs(depassement) < abs(valueColMin))
			{
				valueColMin = depassement;
				axis = NY_AXIS_Y;
			}
		}

		//On verif tout les 4 angles du haut
		if (getCube(xPrev, yPrev, zNext)->isSolid() ||
			getCube(xPrev, yNext, zNext)->isSolid() ||
			getCube(xNext, yPrev, zNext)->isSolid() ||
			getCube(xNext, yNext, zNext)->isSolid())
		{
			float depassement = (zNext * NYCube::CUBE_SIZE) - (pos.Z + height / 2.0f);
			if (abs(depassement) < abs(valueColMin))
			{
				valueColMin = depassement;
				axis = NY_AXIS_Z;
			}
		}

		//On verif tout les 4 angles du bas
		if (getCube(xPrev, yPrev, zPrev)->isSolid() ||
			getCube(xPrev, yNext, zPrev)->isSolid() ||
			getCube(xNext, yPrev, zPrev)->isSolid() ||
			getCube(xNext, yNext, zPrev)->isSolid())
		{
			float depassement = ((zPrev+1) * NYCube::CUBE_SIZE) - (pos.Z - height / 2.0f);
			if (abs(depassement) < abs(valueColMin))
			{
				valueColMin = depassement;
				axis = NY_AXIS_Z;
			}
		}

		return axis;
	}

	//Boites de collisions plus petites que deux cubes
	NYCollision collide_with_world(NYVert3Df pos, float width, float height, NYCollision & collisionPrincipale)
	{
		NYCollision collision = 0x00;		

		int x = (int)(pos.X / NYCube::CUBE_SIZE);
		int y = (int)(pos.Y / NYCube::CUBE_SIZE);
		int z = (int)(pos.Z / NYCube::CUBE_SIZE);

		int xNext = (int)((pos.X + width/2.0f) / NYCube::CUBE_SIZE);
		int yNext = (int)((pos.Y + width/2.0f) / NYCube::CUBE_SIZE);
		int zNext = (int)((pos.Z + height/2.0f)/ NYCube::CUBE_SIZE);

		int xPrev = (int)((pos.X - width/2.0f) / NYCube::CUBE_SIZE);
		int yPrev = (int)((pos.Y - width/2.0f) / NYCube::CUBE_SIZE);
		int zPrev = (int)((pos.Z - height/2.0f)/ NYCube::CUBE_SIZE);

		//De combien je dépasse dans les autres blocs
		float xDepNext = 0;
		if(xNext != x)
			xDepNext = abs((xNext * NYCube::CUBE_SIZE) - (pos.X + width/2.0f));
		float xDepPrev = 0;
		if(xPrev != x)
			xDepPrev = abs((xPrev * NYCube::CUBE_SIZE) - (pos.X + width/2.0f));


		float yDepNext = 0;
		if(yNext != y)
			yDepNext = abs((yNext * NYCube::CUBE_SIZE) - (pos.Y + width/2.0f));
		float yDepPrev = 0;
		if(yPrev != y)
			yDepPrev = abs((yPrev * NYCube::CUBE_SIZE) - (pos.Y + width/2.0f));


		float zDepNext = 0;
		if(zNext != z)
			zDepNext = abs((zNext * NYCube::CUBE_SIZE) - (pos.Z + height/2.0f));
		float zDepPrev = 0;
		if(zPrev != z)
			zDepPrev = abs((zPrev * NYCube::CUBE_SIZE) - (pos.Z + height/2.0f));

		if(x < 0)
			x = 0;
		if(y < 0)
			y = 0;
		if(z < 0)
			z = 0;

		if(xPrev < 0)
			xPrev = 0;
		if(yPrev < 0)
			yPrev = 0;
		if(zPrev < 0)
			zPrev = 0;

		if(xNext < 0)
			xNext = 0;
		if(yNext < 0)
			yNext = 0;
		if(zNext < 0)
			zNext = 0;

		if(x >= MAT_SIZE_CUBES)
			x = MAT_SIZE_CUBES-1;
		if(y >= MAT_SIZE_CUBES)
			y = MAT_SIZE_CUBES-1;
		if(z >= MAT_HEIGHT_CUBES)
			z = MAT_HEIGHT_CUBES-1;

		if(xPrev >= MAT_SIZE_CUBES)
			xPrev = MAT_SIZE_CUBES-1;
		if(yPrev >= MAT_SIZE_CUBES)
			yPrev = MAT_SIZE_CUBES-1;
		if(zPrev >= MAT_HEIGHT_CUBES)
			zPrev = MAT_HEIGHT_CUBES-1;

		if(xNext >= MAT_SIZE_CUBES)
			xNext = MAT_SIZE_CUBES-1;
		if(yNext >= MAT_SIZE_CUBES)
			yNext = MAT_SIZE_CUBES-1;
		if(zNext >= MAT_HEIGHT_CUBES)
			zNext = MAT_HEIGHT_CUBES-1;

		//Est on dans un cube plein ?
		if(getCube(x,y,z)->_Draw)
			collision |= NY_COLLIDE_IN; 

		//Collisions droite et gauche
		//On checke ou se trouvent les sommets de la box
		//On checke les coins top et bottom en meme temps

		//Sommets arrières gauches
		if(getCube(xPrev,yPrev,zPrev)->isSolid() ||
			getCube(xPrev,yPrev,zNext)->isSolid())
		{
			collision |= NY_COLLIDE_LEFT;
			collision |= NY_COLLIDE_BACK;
			if(xDepPrev > yDepPrev)
				collisionPrincipale |= NY_COLLIDE_LEFT;
			else 
				collisionPrincipale |= NY_COLLIDE_BACK;
		}

		//Sommets avants gauches
		if(getCube(xPrev,yNext,zPrev)->isSolid() ||
			getCube(xPrev,yNext,zNext)->isSolid())
		{
			collision |= NY_COLLIDE_LEFT;
			collision |= NY_COLLIDE_FRONT;
			if(xDepPrev > yDepNext)
				collisionPrincipale |= NY_COLLIDE_LEFT;
			else 
				collisionPrincipale |= NY_COLLIDE_FRONT;
		}

		//Sommets arrière droits
		if(getCube(xNext,yPrev,zPrev)->isSolid() ||
			getCube(xNext,yPrev,zNext)->isSolid())
		{
			collision |= NY_COLLIDE_RIGHT;
			collision |= NY_COLLIDE_BACK;
			if(xDepNext > yDepPrev)
				collisionPrincipale |= NY_COLLIDE_RIGHT;
			else 
				collisionPrincipale |= NY_COLLIDE_BACK;
		}

		//Sommets avant droits
		if(getCube(xNext,yNext,zPrev)->isSolid() ||
			getCube(xNext,yNext,zNext)->isSolid())
		{
			collision |= NY_COLLIDE_RIGHT;
			collision |= NY_COLLIDE_FRONT;
			if(xDepNext > yDepNext)
				collisionPrincipale |= NY_COLLIDE_RIGHT;
			else 
				collisionPrincipale |= NY_COLLIDE_FRONT;
		}

		//Pour le bottom on checke tout, meme le milieu 
		if(getCube(xPrev,y    ,zPrev)->isSolid() ||
			getCube(xPrev,yPrev,zPrev)->isSolid() ||
			getCube(xPrev,yNext,zPrev)->isSolid() ||
			getCube(xNext,y    ,zPrev)->isSolid() ||
			getCube(xNext,yPrev,zPrev)->isSolid() ||
			getCube(xNext,yNext,zPrev)->isSolid() ||
			getCube(x    ,y    ,zPrev)->isSolid() ||
			getCube(x    ,yPrev,zPrev)->isSolid() ||
			getCube(x    ,yNext,zPrev)->isSolid())
			collision |= NY_COLLIDE_BOTTOM;

		//Pour le up on checke tout, meme le milieu 
		if(getCube(xPrev,y    ,zNext)->isSolid() ||
			getCube(xPrev,yPrev,zNext)->isSolid() ||
			getCube(xPrev,yNext,zNext)->isSolid() ||
			getCube(xNext,y    ,zNext)->isSolid() ||
			getCube(xNext,yPrev,zNext)->isSolid() ||
			getCube(xNext,yNext,zNext)->isSolid() ||
			getCube(x    ,y    ,zNext)->isSolid() ||
			getCube(x    ,yPrev,zNext)->isSolid() ||
			getCube(x    ,yNext,zNext)->isSolid())
			collision |= NY_COLLIDE_UP;

		return collision;
	}

	void render_world_vbo(void)
	{
		//GLuint texLoc = glGetUniformLocation(prog, "Texture0");
		//glUniform1i(texLoc, 0);
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _TexGrass->Texture);

		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
				{
					glPushMatrix();
					glTranslatef((float)(x*NYChunk::CHUNK_SIZE*NYCube::CUBE_SIZE),(float)(y*NYChunk::CHUNK_SIZE*NYCube::CUBE_SIZE),(float)(z*NYChunk::CHUNK_SIZE*NYCube::CUBE_SIZE));
					_Chunks[x][y][z]->render();	
					glPopMatrix();
				}

	}

	void add_world_to_vbo(void)
	{
		int totalNbVertices = 0;

		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
				{
					_Chunks[x][y][z]->toVbo();
					totalNbVertices += _Chunks[x][y][z]->_NbVertices;
				}

				Log::log(Log::ENGINE_INFO,(toString(totalNbVertices) + " vertices in VBO").c_str());
	}

	void render_world_old_school(void)
	{
		for(int x=0;x<MAT_SIZE_CUBES;x++)
			for(int y=0;y<MAT_SIZE_CUBES;y++)
				for(int z=0;z<MAT_HEIGHT_CUBES;z++)
				{
					glPushMatrix();
					NYCube* c = getCube(x,y,z);
					if(c->_Draw)
					{
						glTranslatef((float)(x)*10,(float)(y)*10,(float)(z)*10);
						GLfloat materialDiffuse[] = {139.f/255.f,69.f/255.f, 19.f/255.f,1.0};
						GLfloat materialAmbient[] = {1, 1, 1, 0.2};
						switch(c->_Type)
						{
						case CUBE_TERRE :
							materialDiffuse[0] = materialAmbient[0]= 139.f/255.f;
							materialDiffuse[1] = materialAmbient[1]= 69.f/255.f;
							materialDiffuse[2] = materialAmbient[2]= 19.f/255.f;
							glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
							break;
						case CUBE_EAU :
							materialDiffuse[0] = materialAmbient[0]= 0.f;
							materialDiffuse[1] = materialAmbient[1]= 0.f;
							materialDiffuse[2] = materialAmbient[2]= 130.f/255.f;
							materialDiffuse[3] = 0.5f;
							break;
						case CUBE_HERBE :
							materialDiffuse[0] = materialAmbient[0]= 0.f;
							materialDiffuse[1] = materialAmbient[1]= 130.f/255.f;
							materialDiffuse[2] = materialAmbient[2]= 0.f;
							break;
						}

						glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
						if(c->_Type != CUBE_AIR)
							glutSolidCube(10);
					}
					glPopMatrix();
				}

	}	

	/**
	* De meme cette fonction peut être grandement opitimisée, on a priviligié la clarté
	*/
	bool getRayCollisionWithCube(NYVert3Df & debSegment, NYVert3Df & finSegment,
		int x,int y,int z,
		NYVert3Df & inter)
	{


		float minDist = -1;
		NYVert3Df interTemp;

		//Face1
		if(intersecDroiteCubeFace(debSegment,finSegment,
			NYVert3Df((x+0)*NYCube::CUBE_SIZE,(y+0)*NYCube::CUBE_SIZE,(z+0)*NYCube::CUBE_SIZE),
			NYVert3Df((x+1)*NYCube::CUBE_SIZE,(y+0)*NYCube::CUBE_SIZE,(z+0)*NYCube::CUBE_SIZE),
			NYVert3Df((x+1)*NYCube::CUBE_SIZE,(y+0)*NYCube::CUBE_SIZE,(z+1)*NYCube::CUBE_SIZE),
			NYVert3Df((x+0)*NYCube::CUBE_SIZE,(y+0)*NYCube::CUBE_SIZE,(z+1)*NYCube::CUBE_SIZE),
			interTemp))
		{
			if((interTemp-debSegment).getMagnitude() < minDist || minDist == -1)
			{
				minDist = (interTemp-debSegment).getMagnitude();
				inter = interTemp;
			}
		}

		//Face2
		if(intersecDroiteCubeFace(debSegment,finSegment,
			NYVert3Df((x+0)*NYCube::CUBE_SIZE,(y+1)*NYCube::CUBE_SIZE,(z+0)*NYCube::CUBE_SIZE),
			NYVert3Df((x+1)*NYCube::CUBE_SIZE,(y+1)*NYCube::CUBE_SIZE,(z+0)*NYCube::CUBE_SIZE),
			NYVert3Df((x+1)*NYCube::CUBE_SIZE,(y+1)*NYCube::CUBE_SIZE,(z+1)*NYCube::CUBE_SIZE),
			NYVert3Df((x+0)*NYCube::CUBE_SIZE,(y+1)*NYCube::CUBE_SIZE,(z+1)*NYCube::CUBE_SIZE),
			interTemp))
		{
			if((interTemp-debSegment).getMagnitude() < minDist || minDist == -1)
			{
				minDist = (interTemp-debSegment).getMagnitude();
				inter = interTemp;
			}
		}

		//Face3
		if(intersecDroiteCubeFace(debSegment,finSegment,
			NYVert3Df((x+0)*NYCube::CUBE_SIZE,(y+0)*NYCube::CUBE_SIZE,(z+0)*NYCube::CUBE_SIZE),
			NYVert3Df((x+1)*NYCube::CUBE_SIZE,(y+0)*NYCube::CUBE_SIZE,(z+0)*NYCube::CUBE_SIZE),
			NYVert3Df((x+1)*NYCube::CUBE_SIZE,(y+1)*NYCube::CUBE_SIZE,(z+0)*NYCube::CUBE_SIZE),
			NYVert3Df((x+0)*NYCube::CUBE_SIZE,(y+1)*NYCube::CUBE_SIZE,(z+0)*NYCube::CUBE_SIZE),
			interTemp))
		{
			if((interTemp-debSegment).getMagnitude() < minDist || minDist == -1)
			{
				minDist = (interTemp-debSegment).getMagnitude();
				inter = interTemp;
			}
		}

		//Face4
		if(intersecDroiteCubeFace(debSegment,finSegment,
			NYVert3Df((x+0)*NYCube::CUBE_SIZE,(y+0)*NYCube::CUBE_SIZE,(z+1)*NYCube::CUBE_SIZE),
			NYVert3Df((x+1)*NYCube::CUBE_SIZE,(y+0)*NYCube::CUBE_SIZE,(z+1)*NYCube::CUBE_SIZE),
			NYVert3Df((x+1)*NYCube::CUBE_SIZE,(y+1)*NYCube::CUBE_SIZE,(z+1)*NYCube::CUBE_SIZE),
			NYVert3Df((x+0)*NYCube::CUBE_SIZE,(y+1)*NYCube::CUBE_SIZE,(z+1)*NYCube::CUBE_SIZE),
			interTemp))
		{
			if((interTemp-debSegment).getMagnitude() < minDist || minDist == -1)
			{
				minDist = (interTemp-debSegment).getMagnitude();
				inter = interTemp;
			}
		}

		//Face5
		if(intersecDroiteCubeFace(debSegment,finSegment,
			NYVert3Df((x+0)*NYCube::CUBE_SIZE,(y+0)*NYCube::CUBE_SIZE,(z+0)*NYCube::CUBE_SIZE),
			NYVert3Df((x+0)*NYCube::CUBE_SIZE,(y+1)*NYCube::CUBE_SIZE,(z+0)*NYCube::CUBE_SIZE),
			NYVert3Df((x+0)*NYCube::CUBE_SIZE,(y+1)*NYCube::CUBE_SIZE,(z+1)*NYCube::CUBE_SIZE),
			NYVert3Df((x+0)*NYCube::CUBE_SIZE,(y+0)*NYCube::CUBE_SIZE,(z+1)*NYCube::CUBE_SIZE),
			interTemp))
		{
			if((interTemp-debSegment).getMagnitude() < minDist || minDist == -1)
			{
				minDist = (interTemp-debSegment).getMagnitude();
				inter = interTemp;
			}
		}

		//Face6
		if(intersecDroiteCubeFace(debSegment,finSegment,
			NYVert3Df((x+1)*NYCube::CUBE_SIZE,(y+0)*NYCube::CUBE_SIZE,(z+0)*NYCube::CUBE_SIZE),
			NYVert3Df((x+1)*NYCube::CUBE_SIZE,(y+1)*NYCube::CUBE_SIZE,(z+0)*NYCube::CUBE_SIZE),
			NYVert3Df((x+1)*NYCube::CUBE_SIZE,(y+1)*NYCube::CUBE_SIZE,(z+1)*NYCube::CUBE_SIZE),
			NYVert3Df((x+1)*NYCube::CUBE_SIZE,(y+0)*NYCube::CUBE_SIZE,(z+1)*NYCube::CUBE_SIZE),
			interTemp))
		{
			if((interTemp-debSegment).getMagnitude() < minDist || minDist == -1)
			{
				minDist = (interTemp-debSegment).getMagnitude();
				inter = interTemp;
			}
		}


		if (minDist < 0)
			return false;

		return true;
	}

	bool getRayCollision(NYVert3Df & debSegment, NYVert3Df & finSegment,
		NYVert3Df & inter,
		int &xCube, int&yCube, int&zCube)
	{
		float len = (finSegment - debSegment).getSize();

		int x = (int)(debSegment.X/ NYCube::CUBE_SIZE);
		int y = (int)(debSegment.Y/ NYCube::CUBE_SIZE);
		int z = (int)(debSegment.Z/ NYCube::CUBE_SIZE);

		int l = (int)(len/ NYCube::CUBE_SIZE)+1; 

		int xDeb = x - l;
		int yDeb = y - l;
		int zDeb = z - l;

		int xFin = x + l;
		int yFin = y + l;
		int zFin = z + l;

		if(xDeb < 0)
			xDeb = 0;
		if(yDeb < 0)
			yDeb = 0;
		if(zDeb < 0)
			zDeb = 0;

		if(xFin >= MAT_SIZE_CUBES)
			xFin = MAT_SIZE_CUBES-1;
		if(yFin >= MAT_SIZE_CUBES)
			yFin = MAT_SIZE_CUBES-1;
		if(zFin >= MAT_HEIGHT_CUBES)
			zFin = MAT_HEIGHT_CUBES-1;

		float minDist = -1;
		NYVert3Df interTmp;
		for(x=xDeb;x<=xFin;x++)
			for(y=yDeb;y<=yFin;y++)
				for(z=zDeb;z<=zFin;z++)
				{
					if(getCube(x,y,z)->isSolid())
					{
						if(getRayCollisionWithCube(debSegment,finSegment,x,y,z,interTmp))
						{
							if((debSegment-interTmp).getMagnitude() < minDist || minDist == -1)
							{
								minDist = (debSegment-interTmp).getMagnitude();
								inter = interTmp;
								xCube = x;
								yCube = y;
								zCube = z;

							}
						}
					}
				}

				if(minDist != -1)
					return true;

				return false;

	}


};



#endif