#pragma once

#include <stdio.h>

enum NYCubeType
{
	CUBE_HERBE,
	CUBE_TERRE,
	CUBE_NEIGE,
	CUBE_EAU,
	CUBE_AIR
};

class NYCube
{
public:
	bool _Draw;
	NYCubeType _Type;
	static const int CUBE_SIZE = 10;


	NYCube()
	{
		_Draw = true;
		_Type = CUBE_AIR;
	}

	bool isSolid(void)
	{
		return (_Type != CUBE_AIR);
	}

	void saveToFile(FILE * fs)
	{
		fputc(_Draw ? 1 : 0, fs);
		fputc(_Type, fs);
	}

	void loadFromFile(FILE * fe)
	{
		_Draw = fgetc(fe) ? true : false;
		_Type = (NYCubeType)fgetc(fe);
	}

	float* getVertices(int _ox, int _oy, int _oz, int _width, float* vertices, int start, int& verts) {
		float offset = _width;
		if(_Type == CUBE_EAU)
		{
			////glNormal3f (.5, -.5, .5;
			vertices[start] = _ox + offset;
			vertices[start + 1] = _oy;
			vertices[start + 2] = _oz + offset;
			////glNormal3f (.5, .5, .5;
			vertices[start + 3] = _ox + offset;
			vertices[start + 4] = _oy + offset;
			vertices[start + 5] = _oz + offset;
			//glNormal3f (-.5, .5, .5;
			vertices[start + 6] = _ox;
			vertices[start + 7] = _oy + offset;
			vertices[start + 8] = _oz + offset;

			vertices[start + 9] = _ox + offset;
			vertices[start + 10] = _oy ;
			vertices[start + 11] = _oz + offset;
			//glNormal3f (-.5, -.5, .5;
			vertices[start + 12] = _ox;
			vertices[start + 13] = _oy + offset;
			vertices[start + 14] = _oz + offset;
			//glNormal3f (.5, -.5, .5;
			vertices[start + 15] = _ox ;
			vertices[start + 16] = _oy;
			vertices[start + 17] = _oz + offset;
			verts = 6;
			return vertices;
		}
		////glNormal3f (.5, -.5, -.5;
		vertices[start + 0] = _ox + offset;
		vertices[start + 1] = _oy;
		vertices[start + 2] = _oz;
		////glNormal3f (.5, .5, -.5;
		vertices[start + 3] = _ox + offset;
		vertices[start + 4] = _oy + offset;
		vertices[start + 5] = _oz;
		////glNormal3f (.5, .5, .5;
		vertices[start + 6] = _ox + offset;
		vertices[start + 7] = _oy + offset;
		vertices[start + 8] = _oz + offset;
		////glNormal3f (.5, .5, .5;
		vertices[start + 9] = _ox + offset;
		vertices[start + 10] = _oy ;
		vertices[start + 11] = _oz ;
		////glNormal3f (.5, -.5, .5;
		vertices[start + 12] = _ox + offset;
		vertices[start + 13] = _oy + offset;
		vertices[start + 14] = _oz + offset;
		////glNormal3f (.5, -.5, -.5;
		vertices[start + 15] = _ox + offset;
		vertices[start + 16] = _oy;
		vertices[start + 17] = _oz + offset;

		//Face 2
		////glNormal3f (0, 1, 0;

		////glNormal3f (.5, .5, -.5;
		vertices[start + 18] = _ox + offset;
		vertices[start + 19] = _oy + offset;
		vertices[start + 20] = _oz;
		////glNormal3f (-.5, .5, -.5;
		vertices[start + 21] = _ox ;
		vertices[start + 22] = _oy + offset;
		vertices[start + 23] = _oz;
		////glNormal3f (-.5, .5, .5;
		vertices[start + 24] = _ox ;
		vertices[start + 25] = _oy + offset;
		vertices[start + 26] = _oz + offset;

		////glNormal3f (-.5, .5, .5;
		vertices[start + 27] = _ox + offset;
		vertices[start + 28] = _oy + offset;
		vertices[start + 29] = _oz;
		////glNormal3f (.5, .5, .5;
		vertices[start + 30] = _ox ;
		vertices[start + 31] = _oy + offset;
		vertices[start + 32] = _oz + offset;
		////glNormal3f (.5, .5, -.5;
		vertices[start + 33] = _ox + offset;
		vertices[start + 34] = _oy + offset;
		vertices[start + 35] = _oz + offset;

		//Face 3
		////glNormal3f (0, 0, 1;
		//glColor3d(0, 0, .5;

		////glNormal3f (.5, -.5, .5;
		vertices[start + 36] = _ox + offset;
		vertices[start + 37] = _oy;
		vertices[start + 38] = _oz + offset;
		////glNormal3f (.5, .5, .5;
		vertices[start + 39] = _ox + offset;
		vertices[start + 40] = _oy + offset;
		vertices[start + 41] = _oz + offset;
		//glNormal3f (-.5, .5, .5;
		vertices[start + 42] = _ox;
		vertices[start + 43] = _oy + offset;
		vertices[start + 44] = _oz + offset;

		vertices[start + 45] = _ox + offset;
		vertices[start + 46] = _oy ;
		vertices[start + 47] = _oz + offset;
		//glNormal3f (-.5, -.5, .5;
		vertices[start + 48] = _ox;
		vertices[start + 49] = _oy + offset;
		vertices[start + 50] = _oz + offset;
		//glNormal3f (.5, -.5, .5;
		vertices[start + 51] = _ox ;
		vertices[start + 52] = _oy;
		vertices[start + 53] = _oz + offset;

		//Face 4
		vertices[start + 54] = _ox;
		vertices[start + 55] = _oy + offset;
		vertices[start + 56] = _oz ;
		//glNormal3f(-.5, .5, -.5;
		vertices[start + 57] = _ox;
		vertices[start + 58] = _oy;
		vertices[start + 59] = _oz;
		//glNormal3f(-.5, -.5, -.5;
		vertices[start + 60] = _ox;
		vertices[start + 61] = _oy;
		vertices[start + 62] = _oz + offset;

		vertices[start + 63] = _ox;
		vertices[start + 64] = _oy + offset;
		vertices[start + 65] = _oz;
		//glNormal3f(-.5, -.5, .5;
		vertices[start + 66] = _ox;
		vertices[start + 67] = _oy;
		vertices[start + 68] = _oz + offset;
		//glNormal3f(-.5, .5, .5;
		vertices[start + 69] = _ox;
		vertices[start + 70] = _oy + offset;
		vertices[start + 71] = _oz + offset;

		//Face 5
		vertices[start + 72] = _ox;
		vertices[start + 73] = _oy;
		vertices[start + 74] = _oz;
		//glNormal3f(-.5, -.5, -.5;
		vertices[start + 75] = _ox + offset;
		vertices[start + 76] = _oy;
		vertices[start + 77] = _oz;
		//glNormal3f(.5, -.5, -.5;
		vertices[start + 78] = _ox + offset;
		vertices[start + 79] = _oy;
		vertices[start + 80] = _oz + offset;

		vertices[start + 81] = _ox;
		vertices[start + 82] = _oy;
		vertices[start + 83] = _oz;
		//glNormal3f(.5, -.5, .5;
		vertices[start + 84] = _ox+ offset;
		vertices[start + 85] = _oy;
		vertices[start + 86] = _oz + offset;
		//glNormal3f(-.5, -.5, .5;
		vertices[start + 87] = _ox;
		vertices[start + 88] = _oy;
		vertices[start + 89] = _oz + offset;

		//Face 6
		//glNormal3f(0, 0, -1;
		//glColor3d(.5, 0, .5;
		//glNormal3f(-.5, .5, -.5;
		vertices[start + 90] = _ox + offset;
		vertices[start + 91] = _oy + offset;
		vertices[start + 92] = _oz;
		//glNormal3f(.5, .5, -.5;
		vertices[start + 93] = _ox + offset;
		vertices[start + 94] = _oy ;
		vertices[start + 95] = _oz;
		//glNormal3f(.5, -.5, -.5;
		vertices[start + 96] = _ox;
		vertices[start + 97] = _oy;
		vertices[start + 98] = _oz;

		vertices[start + 99] = _ox + offset;
		vertices[start + 100] = _oy + offset;
		vertices[start + 101] = _oz;
		//glNormal3f(-.5, -.5, -.5;
		vertices[start + 102] = _ox;
		vertices[start + 103] = _oy;
		vertices[start + 104] = _oz;
		//glNormal3f(-.5, .5, -.5;
		vertices[start + 105] = _ox;
		vertices[start + 106] = _oy + offset;
		vertices[start + 107] = _oz;
		verts = 36;
		return vertices;
	}

	float* getNormals(float* normals, int start) {


		if(_Type == CUBE_EAU)
		{
			for (int i = 0; i < 18; i += 3)
			{
				normals[start + i] = 0;
				normals[start + i + 1] = 0;
				normals[start + i + 2] = 1;
			}
			return normals;
		}

		for (int i = 0; i < 18; i += 3)
		{
			normals[start + i] = 1;
			normals[start + i + 1] = 0;
			normals[start + i + 2] = 0;
		}
		for (int i = 18; i < 36; i += 3)
		{
			normals[start + i] = 0;
			normals[start + i + 1] = 1;
			normals[start + i + 2] = 0;
		}
		for (int i = 36; i < 54; i += 3)
		{
			normals[start + i] = 0;
			normals[start + i + 1] = 0;
			normals[start + i + 2] = 1;
		}
		for (int i = 54; i < 72; i += 3)
		{
			normals[start + i] = -1;
			normals[start + i + 1] = 0;
			normals[start + i + 2] = 0;
		}
		for (int i = 72; i < 90; i += 3)
		{
			normals[start + i] = 0;
			normals[start + i + 1] = -1;
			normals[start + i + 2] = 0;
		}
		for (int i = 90; i < 108; i += 3)
		{
			normals[start + i] = 0;
			normals[start + i + 1] = 0;
			normals[start + i + 2] = -1;
		}
		return normals;

		//Face1

		normals[start + 1 - 1] = .5;
		normals[start + 2 - 1] = -.5;
		normals[start + 3 - 1] = -.5;
		////glNormal3f (
		normals[start + 4 - 1] = .5;
		normals[start + 5 - 1] = .5;
		normals[start + 6 - 1] = -.5;
		////glNormal3f (
		normals[start + 7 - 1] = .5;
		normals[start + 8 - 1] = .5;
		normals[start + 9 - 1] = .5;
		////glNormal3f (
		normals[start + 10 - 1] = .5;
		normals[start + 11 - 1] = .5;
		normals[start + 12 - 1] = .5;
		////glNormal3f (
		normals[start + 13 - 1] = .5;
		normals[start + 14 - 1] = -.5;
		normals[start + 15 - 1] = .5;
		////glNormal3f (
		normals[start + 16 - 1] = .5;
		normals[start + 17 - 1] = -.5;
		normals[start + 18 - 1] = -.5;

		//Face 2
		////glNormal3f (
		normals[start + 19 - 1] = .5;
		normals[start + 20 - 1] = .5;
		normals[start + 21 - 1] = -.5;
		////glNormal3f (
		normals[start + 22 - 1] = -.5;
		normals[start + 23 - 1] = .5;
		normals[start + 24 - 1] = -.5;
		////glNormal3f (
		normals[start + 25 - 1] = -.5;
		normals[start + 26 - 1] = .5;
		normals[start + 27 - 1] = .5;
		////glNormal3f (
		normals[start + 28 - 1] = -.5;
		normals[start + 29 - 1] = .5;
		normals[start + 30 - 1] = .5;
		////glNormal3f (
		normals[start + 31 - 1] = .5;
		normals[start + 32 - 1] = .5;
		normals[start + 33 - 1] = .5;
		////glNormal3f (
		normals[start + 34 - 1] = .5;
		normals[start + 35 - 1] = .5;
		normals[start + 36 - 1] = -.5;

		//Face 3
		////glNormal3f (
		normals[start + 37 - 1] = .5;
		normals[start + 38 - 1] = -.5;
		normals[start + 39 - 1] = .5;
		////glNormal3f (
		normals[start + 40 - 1] = .5;
		normals[start + 41 - 1] = .5;
		normals[start + 42 - 1] = .5;
		//glNormal3f (
		normals[start + 43 - 1] = -.5;
		normals[start + 44 - 1] = .5;
		normals[start + 45 - 1] = .5;
		//glNormal3f (
		normals[start + 46 - 1] = -.5;
		normals[start + 47 - 1] = .5;
		normals[start + 48 - 1] = .5;
		//glNormal3f (
		normals[start + 49 - 1] = -.5;
		normals[start + 50 - 1] = -.5;
		normals[start + 51 - 1] = .5;
		//glNormal3f (
		normals[start + 52 - 1] = .5;
		normals[start + 53 - 1] = -.5;
		normals[start + 54 - 1] = .5;

		//Face 4
		normals[start + 54] = -.5;
		normals[start + 55] = .5;
		normals[start + 56] = .5;
		normals[start + 57] = -.5;
		normals[start + 58] = .5;
		normals[start + 59] = -.5;
		normals[start + 60] = -.5;
		normals[start + 61] = -.5;
		normals[start + 62] = -.5;
		normals[start + 63] = -.5;
		normals[start + 64] = -.5;
		normals[start + 65] = -.5;
		normals[start + 66] = -.5;
		normals[start + 67] = -.5;
		normals[start + 68] = .5;
		normals[start + 69] = -.5;
		normals[start + 70] = .5;
		normals[start + 71] = .5;

		//Face 5
		normals[start + 72] = -.5;
		normals[start + 73] = -.5;
		normals[start + 74] = .5;
		normals[start + 75] = -.5;
		normals[start + 76] = -.5;
		normals[start + 77] = -.5;
		normals[start + 78] = .5;
		normals[start + 79] = -.5;
		normals[start + 80] = -.5;
		normals[start + 81] = .5;
		normals[start + 82] = -.5;
		normals[start + 83] = -.5;
		normals[start + 84] = .5;
		normals[start + 85] = -.5;
		normals[start + 86] = .5;
		normals[start + 87] = -.5;
		normals[start + 88] = -.5;
		normals[start + 89] = .5;

		//Face 6
		normals[start + 90] = -.5;
		normals[start + 91] = .5;
		normals[start + 92] = -.5;
		normals[start + 93] = .5;
		normals[start + 94] = .5;
		normals[start + 95] = -.5;
		normals[start + 96] = .5;
		normals[start + 97] = -.5;
		normals[start + 98] = -.5;
		normals[start + 99] = .5;
		normals[start + 100] = -.5;
		normals[start + 101] = -.5;
		normals[start + 102] = -.5;
		normals[start + 103] = -.5;
		normals[start + 104] = -.5;
		normals[start + 105] = -.5;
		normals[start + 106] = .5;
		normals[start + 107] = -.5;

		return normals;
	}

	float* getTextures(float *textures, int start)
	{
		
		float coef = 0.25*_Type;
		if(_Type == CUBE_EAU)
		{
			textures[start ] = coef + 0;
			textures[start + 1] = 0;
			textures[start + 2] = coef + 0.25;
			textures[start + 3] = 0;
			textures[start + 4] = coef + 0.25;
			textures[start + 5] = 0.333;
			//vert2
			textures[start + 6] = coef + 0;
			textures[start + 7] = 0;
			textures[start + 8] = coef + 0.25;
			textures[start + 9] = 0.333;
			textures[start + 10] = coef + 0;
			textures[start + 11] = 0.333;
			return textures;
		}

		//face1
		//vert1
		textures[start]     = coef + 0;
		textures[start+1]   = 0.666;
		textures[start + 2] = coef + 0.25;
		textures[start + 3] = 0.666;
		textures[start + 4] = coef + 0.25;
		textures[start + 5] = 0.333;
		//vert2
		textures[start + 6] = coef + 0;
		textures[start + 7] = 0.666;
		textures[start + 8] = coef + 0.25;
		textures[start + 9] = 0.333;
		textures[start + 10] = coef +  0;
		textures[start + 11] = 0.333;
		//Face 2
		//vert1
		textures[start+12]  = coef + 0;
		textures[start+13]  = 0.666;
		textures[start + 14]= coef + 0.25;
		textures[start + 15]= 0.666;
		textures[start + 16]= coef + 0.25;
		textures[start + 17]= 0.333;
		//vert2
		textures[start + 18]= coef +  0;
		textures[start + 19]= 0.666;
		textures[start + 20]= coef + 0.25;
		textures[start + 21]= 0.333;
		textures[start + 22] =coef +  0;
		textures[start + 23] = 0.333;

		//face3
		//vert1
		textures[start + 24] = coef + 0;
		textures[start + 25] = 0;
		textures[start + 26] = coef + 0.25;
		textures[start + 27] = 0;
		textures[start + 28] = coef + 0.25;
		textures[start + 29] = 0.333;
		//vert2
		textures[start + 30] = coef + 0;
		textures[start + 31] = 0;
		textures[start + 32] = coef + 0.25;
		textures[start + 33] = 0.333;
		textures[start + 34] = coef + 0;
		textures[start + 35] = 0.333;

		//Face 4
		//vert1
		textures[start + 36]= coef + 0;
		textures[start + 37]= 0.666;
		textures[start + 38]= coef + 0.25;
		textures[start + 39]= 0.666;
		textures[start + 40]= coef + 0.25;
		textures[start + 41]= 0.333;
		//vert2
		textures[start + 42]= coef + 0;
		textures[start + 43]= 0.666;
		textures[start + 44]= coef + 0.25;
		textures[start + 45]= 0.333;
		textures[start + 46] = coef + 0;
		textures[start + 47] = 0.333;
		
		//face5
		//vert1
		textures[start + 48]= coef + 0;
		textures[start + 49]= 0.666;
		textures[start + 50]= coef + 0.25;
		textures[start + 51]= 0.666;
		textures[start + 52]= coef + 0.25;
		textures[start + 53]= 0.333;
		//vert2
		textures[start + 54]= coef + 0;
		textures[start + 55]= 0.666;
		textures[start + 56]= coef + 0.25;
		textures[start + 57]= 0.333;
		textures[start + 58] = coef + 0;
		textures[start + 59] = 0.333;

		//face6
		//vert1
		textures[start + 60] = coef + 0;
		textures[start + 61] = 0.666;
		textures[start + 62] = coef + 0.25;
		textures[start + 63] = 0.666;
		textures[start + 64] = coef + 0.25;
		textures[start + 65] = 1;
		//vert2
		textures[start + 66] = coef + 0;
		textures[start + 67] = 0.666;
		textures[start + 68] = coef + 0.25;
		textures[start + 69] = 1;
		textures[start + 70] = coef + 0;
		textures[start + 71] = 1;

		return textures;
	}


	float* getColors(float* colors, int start)
	{
		int ra = 20, la = 10;
		float r = 1.f, g = 1.f, b = 1.f;
		switch (_Type)
		{
		case CUBE_EAU:
			r = 0 / 255.f;
			g = 0 / 255.f;
			b = (160 + rand() % ra - la) / 255.f;
			break;
		case CUBE_HERBE:
			r = 0 / 255.f;
			g = (130 + rand() % ra - la) / 255.f;
			b = 0 / 255.f;
			break;
		case CUBE_TERRE:
			r = (125 + rand() % ra - la) / 255.f;
			g = (40 + rand() % ra - la) / 255.f;
			b = 0 / 255.f;
			break;
		case CUBE_NEIGE:
			r = (200 + rand() % ra - la) / 255.f;
			g = (200 + rand() % ra - la) / 255.f;
			b = (200 + rand() % ra - la) / 255.f;
			break;
		}

		if(_Type == CUBE_EAU)
		{
			for (int i = 0; i < 18; i += 3)
			{
				colors[start + i] = 1;
				colors[start + i + 1] = 1;
				colors[start + i + 2] = 1;
			}
		}
		else{
			for (int i = 0; i < 108; i += 3)
			{
				colors[start + i] = 1;
				colors[start + i + 1] = 1;
				colors[start + i + 2] = 1;
			}
		}
		return colors;
	}
};