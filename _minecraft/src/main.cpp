//Includes application
#include <conio.h>
#include <vector>
#include <string>
#include <windows.h>

#include "external/gl/glew.h"
#include "external/gl/freeglut.h"

//Moteur
#include "engine/utils/types_3d.h"
#include "engine/timer.h"
#include "engine/log/log_console.h"
#include "engine/render/renderer.h"
#include "engine/gui/screen.h"
#include "engine/gui/screen_manager.h"

#include "world.h"
#include "avatar.h"
#include "IABase.h"
#include "Herbe.h"
#include "Crotte.h"
#include "RessourcesManager.h"

//State Machine
#include "FSM.h"

//State Machine Test
#include "TestStateMachine.h"

//PF
#include "Pathfinding.h"

//Creatures
#include "Wastedosaure.h"
#include "Dahut.h"
#include "Gendamour.h"
#include "Griffonkitu.h"
#include "GlaceGouille.h"
#include "Lemming.h"
#include "Furz.h"
#include "Crabe.h"
#include "Cameleon.h"

NYWorld * g_world;

NYRenderer * g_renderer = NULL;
NYTimer * g_timer = NULL;
int g_nb_frames = 0;
float g_elapsed_fps = 0;
int g_main_window_id;
int g_mouse_btn_gui_state = 0;
bool g_fullscreen = false;
float angleRad;
NYVert3Df lightPostion;
NYAvatar* avatar;
GLuint g_program;

//GUI 
GUIScreenManager * g_screen_manager = NULL;
GUIBouton * BtnParams = NULL;
GUIBouton * BtnClose = NULL;
GUILabel * LabelFps = NULL;
GUILabel * LabelCam = NULL;
GUIScreen * g_screen_params = NULL;
GUIScreen * g_screen_jeu = NULL;
GUISlider * g_slider;

//Soleil
NYVert3Df g_sun_dir;
NYColor g_sun_color;
float g_mn_lever = 6.0f * 60.0f;
float g_mn_coucher = 19.0f * 60.0f;
float g_tweak_time = 0;
bool g_fast_time = false;

//IABase *lapin;
Herbe *herbe;
Herbe *herbe2;

Crotte *crotte;

//State Machine
FSM * master_FSM;

//State Machine Test
TestStateMachine * entityTest1;
TestStateMachine * entityTest2;

//PathFinding
Path returnPath;

//Vecteur de Creatures
//static std::vector<IABase*> g_Creatures;

//Stockage des créatures
typedef std::vector<IABase*> CreatureVector;
std::map<eTypeCreature, CreatureVector> g_CreatureMap;

void spawnCreatures()
{
	//Init creature map
	for(int i = 0; i < (int)CREATURE_NUM; ++i)
	{
		eTypeCreature type = (eTypeCreature) i;
		g_CreatureMap.insert(std::pair<eTypeCreature, CreatureVector>(type, CreatureVector()));
	}

	// === Ajout des créatures ===

	//Ajout des créatures
	//Wastedosaure
	for (int i = 0; i < 6; ++i)
	{
		Wastedosaure * w = new Wastedosaure(g_world, NYVert2Df(50 + 2 * i, 20 + 2 * i));
		w->SetEntities(&g_CreatureMap);
		g_CreatureMap[WASTEDOSAURE].push_back(w);
	}

	//Dahut
	for(int i = 0; i < 10; ++i)
	{
		int x = i % 10 + 10;
		int y = i / 10 + 10;
		g_CreatureMap[DAHUT].push_back(new Dahut(g_world, NYVert2Df(x, y)));
	}

	//Gendamour
	for (int i = 0; i < 10; ++i)
	{
		int x = i % 10 + 10;
		int y = i / 10 + 10;
		g_CreatureMap[GENDAMOUR].push_back(new Gendamour(g_world, NYVert2Df(x, y)));
	}

	//Griffonkitu
	for (int i = 0; i < 15; ++i)
	{
		int x = i % 10 + 10;
		int y = i / 10 + 10;
		Griffonkitu * g = new Griffonkitu(g_world, NYVert2Df(x, y));
		g->SetEntities(&g_CreatureMap);
		g_CreatureMap[GRIFFONKITU].push_back(g);
	}

	//Gendamour
	for (int i = 0; i < 10; ++i)
	{
		int x = i % 10 + 10;
		int y = i / 10 + 10;
		g_CreatureMap[GENDAMOUR].push_back(new Gendamour(g_world, NYVert2Df(x, y)));
	}

	//Glacegouille
	for (int i = 0; i < 10; ++i)
	{
		int x = i % 10 + 10;
		int y = i / 10 + 10;
		g_CreatureMap[GLACEGOUILLE].push_back(new GlaceGouille(g_world, NYVert2Df(x, y)));
	}

	// Lemming
	for (int i = 0; i < 10; ++i)
	{
		Lemming * l = new Lemming(g_world, NYVert2Df(i % 10 + 10, i / 10 + 10));
		l->m_entities = &g_CreatureMap;
		g_CreatureMap[LEMMING].push_back(l);
	}
	// Crabe
	for (int i = 0; i < 10; ++i)
	{
		Crabe * crabe = new Crabe(g_world, NYVert2Df(rand() % 100, rand() % 100));
		crabe->m_entities = &g_CreatureMap;
		g_CreatureMap[CRABE].push_back(crabe);
	}
	//Furz
	g_CreatureMap[FURZ].push_back(new Furz(g_world, NYVert2Df(40,40)));
}

/** === Mise à jour des IA ===
 * 
 * Afin d'éviter que le framerate ne diminue trop, un temps fixe
 * est alloué à la mise à jour des IA. A chaque appel, on effectue 
 * autant de mises à jour dans le temps imparti et on reprend là
 * où s'est arrêté à la frame suivante.
 * Les mises à jour s'arrêtent avant le temps imparti si on a
 * réussi à mettre à jour toutes les créatures.
**/
void creatureUpdate(int computeTimeMS)
{
	//Round robin creature update
	bool looped = false;
	bool timedOut = false;
	static int typeIt = 0;
	static int creatureIt = 0;
	static NYTimer updateTimer;
	int startType = -1;
	int startCreature = -1;

	updateTimer.start();
	while(!looped && !timedOut)
	{
		//Loop through creatures
		creatureIt++;
		while(!looped && creatureIt >= g_CreatureMap[(eTypeCreature)typeIt].size())
		{
			creatureIt = 0;
			typeIt++;
			if(typeIt >= CREATURE_NUM)
			{
				typeIt = 0;
			}

			if(typeIt == startType && creatureIt == startCreature)
			{
				looped = true;
			}
		}

		//Init starting indexes to check for a loop
		if(startType < 0 && startCreature < 0)
		{
			startType = typeIt;
			startCreature = creatureIt;
		}

		//Update creature
		if(!looped)
		{
			g_CreatureMap[(eTypeCreature)typeIt][creatureIt]->UpdateIA();
		}

		//Check allocated time
		if(updateTimer.getElapsedMs() >= computeTimeMS)
		{
			timedOut = true;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// GESTION APPLICATION
//////////////////////////////////////////////////////////////////////////
void update(void)
{
	float elapsed = g_timer->getElapsedSeconds(true);
	static float g_eval_elapsed = 0;

	//Calcul des fps
	g_elapsed_fps += elapsed;
	g_nb_frames++;
	if(g_elapsed_fps > 1.0)
	{
		LabelFps->Text = std::string("FPS : ") + toString(g_nb_frames);
		g_elapsed_fps -= 1.0f;
		g_nb_frames = 0;
	}
	//Rendu
	lightPostion.rotate(NYVert3Df(0,1,0),M_PI_4*elapsed);
	angleRad +=M_PI_4*elapsed;
	g_renderer->render(elapsed);
	avatar->update(elapsed);


	//lapin->UpdateHunger(NYRenderer::_DeltaTime,NYRenderer::_DeltaTimeCumul);
	//herbe->Update(NYRenderer::_DeltaTime);

	//Update State Machine example
	//entityTest1->UpdateIA();
	//entityTest2->UpdateIA();

	//Update ressouces
	RessourcesManager::GetSingleton()->Update();

	//Update creatures (max 5ms)
	creatureUpdate(5);

	/*
	//Update creatures
	for(int i = 0; i < CREATURE_NUM; ++i)
	{
		eTypeCreature type = (eTypeCreature) i;
		for(int j = 0; j < g_CreatureMap[type].size(); ++j)
		{
			g_CreatureMap[type][j]->UpdateIA();
		}
	}*/

	/*for (vector<IABase*>::iterator it = g_Creatures.begin(); it != g_Creatures.end(); ++it)
	{
		(*it)->UpdateIA();
	}*/
}


void render2d(void)
{
	g_screen_manager->render();
}

void renderObjects(void)
{
	/*glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3d(1,0,0);
	glVertex3d(0,0,0);
	glVertex3d(10000,0,0);
	glColor3d(0,1,0);
	glVertex3d(0,0,0);
	glVertex3d(0,10000,0);
	glColor3d(0,0,1);
	glVertex3d(0,0,0);
	glVertex3d(0,0,10000);
	glEnd();*/

	glColor3d(1,1,1);

	//Active la lumière
	glEnable(GL_LIGHTING);
	glShadeModel ( GL_SMOOTH );

	//Rendu du soleil

	//On sauve la matrice
	glPushMatrix();

	//Position du soleil
	glTranslatef(g_renderer->_Camera->_Position.X,g_renderer->_Camera->_Position.Y,g_renderer->_Camera->_Position.Z);
	glTranslatef(g_sun_dir.X*1000,g_sun_dir.Y*1000,g_sun_dir.Z*1000);

	//Material du soleil : de l'emissive
	GLfloat sunEmissionMaterial[] = {0.0, 0.0, 0.0,1.0};
	sunEmissionMaterial[0] = g_sun_color.R;
	sunEmissionMaterial[1] = g_sun_color.V;
	sunEmissionMaterial[2] = g_sun_color.B;
	glMaterialfv(GL_FRONT, GL_EMISSION, sunEmissionMaterial);

	//On dessine un cube pour le soleil
	glutSolidCube(50.0f);

	//On reset le material emissive pour la suite
	sunEmissionMaterial[0] = 0.0f;
	sunEmissionMaterial[1] = 0.0f;
	sunEmissionMaterial[2] = 0.0f;
	glMaterialfv(GL_FRONT, GL_EMISSION, sunEmissionMaterial);

	//Reset de la matrice
	glPopMatrix();

	glUseProgram(g_program);

	GLuint elap = glGetUniformLocation(g_program, "iGlobalTime");
	glUniform1f(elap, NYRenderer::_DeltaTimeCumul);

	GLuint amb = glGetUniformLocation(g_program, "ambientLevel");
	glUniform1f(amb, 0.4);


	GLuint invView = glGetUniformLocation(g_program, "invertView");
	glUniformMatrix4fv(invView,1,true,g_renderer->_Camera->_InvertViewMatrix.Mat.t);

	glPushMatrix();
	//g_world->render_world_old_school();

	g_world->render_world_vbo();

	glPopMatrix();
	glUseProgram(0);
	avatar->render();

	//Render ressources
	RessourcesManager::GetSingleton()->Render();
	
	//herbe->Render();
	//herbe2->Render();

	//crotte->Render();

	returnPath.DrawPath();

	//Render creatures
	for(int i = 0; i < CREATURE_NUM; ++i)
	{
		eTypeCreature type = (eTypeCreature) i;
		for(int j = 0; j < g_CreatureMap[type].size(); ++j)
		{
			g_CreatureMap[type][j]->Draw();
		}
	}

	//Rendu des créatures
	/*for (vector<IABase*>::iterator it = g_Creatures.begin(); it != g_Creatures.end(); ++it)
	{
		(*it)->Draw();
	}*/
}

bool getSunDirection(NYVert3Df & sun, float mnLever, float mnCoucher)
{
	bool nuit = false;

	SYSTEMTIME t;
	GetLocalTime(&t);

	//On borne le tweak time à une journée (cyclique)
	while(g_tweak_time > 60)
		g_tweak_time -= 60;

	//Temps écoulé depuis le début de la journée
	float fTime = (float) (t.wHour*60 + t.wMinute);
	fTime += g_tweak_time;
	while(fTime > 24*60)
		fTime -= 24*60;

	//Si c'est la nuit
	if(fTime < mnLever || fTime > mnCoucher)
	{
		nuit = true;
		if(fTime < mnLever)
			fTime += 24*60;
		fTime -= mnCoucher;
		fTime /= (mnLever+24*60 - mnCoucher);
		fTime *= M_PI;
	}
	else
	{
		//c'est le jour
		nuit = false;
		fTime -= mnLever;
		fTime /= (mnCoucher-mnLever);
		fTime *= M_PI;
	}

	//Position en fonction de la progression dans la journée
	sun.X = cos(fTime);
	sun.Y = 0.2f;
	sun.Z = sin(fTime);
	sun.normalize();

	return nuit;
}

void setLightsBasedOnDayTime(void)
{
	//On active la light 0
	glEnable(GL_LIGHT0);

	//On recup la direciton du soleil
	bool nuit = getSunDirection(g_sun_dir,g_mn_lever,g_mn_coucher);

	//On définit une lumière directionelle (un soleil)
	float position[4] = {g_sun_dir.X,g_sun_dir.Y,g_sun_dir.Z,0}; ///w = 0 donc c'est une position a l'infini
	glLightfv(GL_LIGHT0, GL_POSITION, position );

	//Pendant la journée
	if(!nuit)
	{
		//On definit la couleur
		NYColor sunColor(1,1,0.8,1);
		NYColor skyColor(0,181.f/255.f,221.f/255.f,1);
		NYColor downColor(0.9,0.5,0.1,1);
		sunColor = sunColor.interpolate(downColor,(abs(g_sun_dir.X)));
		skyColor = skyColor.interpolate(downColor,(abs(g_sun_dir.X)));

		g_renderer->setBackgroundColor(skyColor);

		float color[4] = {sunColor.R,sunColor.V,sunColor.B,1};
		glLightfv(GL_LIGHT0, GL_DIFFUSE, color );
		float color2[4] = {sunColor.R,sunColor.V,sunColor.B,1};
		glLightfv(GL_LIGHT0, GL_AMBIENT, color2 );
		g_sun_color = sunColor;
	}
	else
	{
		//La nuit : lune blanche et ciel noir
		NYColor sunColor(1,1,1,1);
		NYColor skyColor(0,0,0,1);
		g_renderer->setBackgroundColor(skyColor);
		float color[4] = {sunColor.R/3.f,sunColor.V/3.f,sunColor.B/3.f,1};
		glLightfv(GL_LIGHT0, GL_DIFFUSE, color );
		float color2[4] = {sunColor.R/7.f,sunColor.V/7.f,sunColor.B/7.f,1};
		glLightfv(GL_LIGHT0, GL_AMBIENT, color2 );
		g_sun_color = sunColor;
	}
	NYColor sunColor(1,1,1,1);
	NYColor skyColor(1,1,1,1);
	g_renderer->setBackgroundColor(skyColor);
	float color[4] = {sunColor.R/3.f,sunColor.V/3.f,sunColor.B/3.f,1};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, color );
	float color2[4] = {sunColor.R/7.f,sunColor.V/7.f,sunColor.B/7.f,1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, color2 );
	g_sun_color = sunColor;
}


void resizeFunction(int width, int height)
{
	glViewport(0, 0, width, height);
	g_renderer->resize(width,height);
}

//////////////////////////////////////////////////////////////////////////
// GESTION CLAVIER SOURIS
//////////////////////////////////////////////////////////////////////////

void specialDownFunction(int key, int p1, int p2)
{
	//On change de mode de camera
	if(key == GLUT_KEY_LEFT)
	{
	}
}

void specialUpFunction(int key, int p1, int p2)
{

}

void keyboardDownFunction(unsigned char key, int p1, int p2)
{
	if(key == VK_ESCAPE)
	{
		glutDestroyWindow(g_main_window_id);	
		exit(0);
	}


	if(key == 'f')
	{
		if(!g_fullscreen){
			glutFullScreen();
			g_fullscreen = true;
		} else if(g_fullscreen){
			glutLeaveGameMode();
			glutLeaveFullScreen();
			glutReshapeWindow(g_renderer->_ScreenWidth, g_renderer->_ScreenWidth);
			glutPositionWindow(0,0);
			g_fullscreen = false;
		}
	}
	NYVert3Df dir = g_renderer->_Camera->_Direction;
	NYVert3Df norm = g_renderer->_Camera->_NormVec;
	NYVert3Df move = NYVert3Df();
	switch(key)
	{
	case 'z' :
		avatar->avance= true;
		break;
	case 's' :
		avatar->recule = true;
		break;
	case 'd':
		avatar->droite= true;
		break;
	case 'q':
		avatar->gauche = true;
		break;
	}
	if(key == ' ')
	{
		avatar->Jump = true;
	}

	
}

void keyboardUpFunction(unsigned char key, int p1, int p2)
{
	switch(key)
	{
	case 'z' :
		avatar->avance= false;
		break;
	case 's' :
		avatar->recule = false;
		break;
	case 'd':
		avatar->droite = false;
		break;
	case 'q':
		avatar->gauche= false;
		break;
	}
	if(key == ' ')
	{
		avatar->Jump = false;
	}

}

void mouseWheelFunction(int wheel, int dir, int x, int y)
{
	g_renderer->_Camera->move(NYVert3Df(0,0,dir));
}

void mouseFunction(int button, int state, int x, int y)
{
	//Gestion de la roulette de la souris
	if((button & 0x07) == 3 && state)
		mouseWheelFunction(button,1,x,y);
	if((button & 0x07) == 4 && state)
		mouseWheelFunction(button,-1,x,y);

	//GUI
	g_mouse_btn_gui_state = 0;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)  {
		g_mouse_btn_gui_state |= GUI_MLBUTTON;
		avatar->Destroying = true;
	}

	bool mouseTraite = false;
	mouseTraite = g_screen_manager->mouseCallback(x,y,g_mouse_btn_gui_state,0,0);


}

void mouseMoveFunction(int x, int y, bool pressed)
{
	static int lastx = -1;
	static int lasty = -1;

	if(!pressed)
	{
		lastx = x;
		lasty = y;
	}
	else
	{
		if(lastx == -1 && lasty == -1)
		{
			lastx = x;
			lasty = y;
		}

		int dx = x-lastx;
		int dy = y-lasty;

		lastx = x;
		lasty = y;

		if(GetKeyState(VK_LCONTROL) & 0x80)
		{
			NYVert3Df strafe = g_renderer->_Camera->_NormVec;
			strafe.Z = 0;
			strafe.normalize();
			strafe *= (float)-dx/50.0f;

			NYVert3Df avance = g_renderer->_Camera->_Direction;
			avance.Z = 0;
			avance.normalize();
			avance *= (float)dy/50.0f;

			g_renderer->_Camera->move(avance + strafe);
		}
		else
		{
			g_renderer->_Camera->rotate((float)-dx/300.0f);
			g_renderer->_Camera->rotateUp((float)-dy/300.0f);
		}

	}



}

void mouseMoveActiveFunction(int x, int y)
{
	mouseMoveFunction(x,y,true);
}

void mouseMovePassiveFunction(int x, int y)
{
	mouseMoveFunction(x,y,false);
}


void clickBtnParams (GUIBouton * bouton)
{
	g_screen_manager->setActiveScreen(g_screen_params);
}

void clickBtnCloseParam (GUIBouton * bouton)
{
	g_screen_manager->setActiveScreen(g_screen_jeu);
}

/**
* POINT D'ENTREE PRINCIPAL
**/
int main(int argc, char* argv[])
{ 
	LogConsole::createInstance();

	int screen_width = 1200;
	int screen_height = 800;

	glutInit(&argc, argv); 
	glutInitContextVersion(3,0);
	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
		);

	glutInitWindowSize(screen_width,screen_height);
	glutInitWindowPosition (0, 40);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE );

	glEnable(GL_MULTISAMPLE);

	Log::log(Log::ENGINE_INFO, (toString(argc) + " arguments en ligne de commande.").c_str());	
	bool gameMode = true;
	for(int i=0;i<argc;i++)
	{
		if(argv[i][0] == 'w')
		{
			Log::log(Log::ENGINE_INFO,"Arg w mode fenetre.\n");
			gameMode = false;
		}
	}

	if(gameMode)
	{
		int width = glutGet(GLUT_SCREEN_WIDTH);
		int height = glutGet(GLUT_SCREEN_HEIGHT);

		char gameModeStr[200];
		sprintf(gameModeStr,"%dx%d:32@60",width,height);
		glutGameModeString(gameModeStr);
		g_main_window_id = glutEnterGameMode();
	}
	else
	{
		g_main_window_id = glutCreateWindow("MyNecraft");
		glutReshapeWindow(screen_width,screen_height);
	}

	if(g_main_window_id < 1) 
	{
		Log::log(Log::ENGINE_ERROR,"Erreur creation de la fenetre.");
		exit(EXIT_FAILURE);
	}

	GLenum glewInitResult = glewInit();

	if (glewInitResult != GLEW_OK)
	{
		Log::log(Log::ENGINE_ERROR,("Erreur init glew " + std::string((char*)glewGetErrorString(glewInitResult))).c_str());
		_cprintf("ERROR : %s",glewGetErrorString(glewInitResult));
		exit(EXIT_FAILURE);
	}

	//Affichage des capacités du système
	Log::log(Log::ENGINE_INFO,("OpenGL Version : " + std::string((char*)glGetString(GL_VERSION))).c_str());

	glutDisplayFunc(update);
	glutReshapeFunc(resizeFunction);
	glutKeyboardFunc(keyboardDownFunction);
	glutKeyboardUpFunc(keyboardUpFunction);
	glutSpecialFunc(specialDownFunction);
	glutSpecialUpFunc(specialUpFunction);
	glutMouseFunc(mouseFunction);
	glutMotionFunc(mouseMoveActiveFunction);
	glutPassiveMotionFunc(mouseMovePassiveFunction);
	glutIgnoreKeyRepeat(0);

	//Initialisation du renderer
	g_renderer = NYRenderer::getInstance();
	g_renderer->setRenderObjectFun(renderObjects);
	g_renderer->setRender2DFun(render2d);
	//A la fin du main, chercher et remplacer setLights par setLightsBasedOnDayTime
	g_renderer->setLightsFun(setLightsBasedOnDayTime);
	g_renderer->setBackgroundColor(NYColor());


	/* SHADER
	*
	*
	*
	*
	*/
	//g_renderer->initialise(true);
	//g_program = g_renderer->createProgram("shaders/psbase.glsl","shaders/vsbase.glsl");


	///

	//On applique la config du renderer
	glViewport(0, 0, g_renderer->_ScreenWidth, g_renderer->_ScreenHeight);
	g_renderer->resize(g_renderer->_ScreenWidth,g_renderer->_ScreenHeight);

	//Ecran de jeu
	uint16 x = 10;
	uint16 y = 10;
	g_screen_jeu = new GUIScreen(); 

	g_screen_manager = new GUIScreenManager();

	//Bouton pour afficher les params
	BtnParams = new GUIBouton();
	BtnParams->Titre = std::string("Params");
	BtnParams->X = x;
	BtnParams->setOnClick(clickBtnParams);
	g_screen_jeu->addElement(BtnParams);

	y += BtnParams->Height + 1;

	LabelFps = new GUILabel();
	LabelFps->Text = "FPS";
	LabelFps->X = x;
	LabelFps->Y = y;
	LabelFps->Visible = true;
	g_screen_jeu->addElement(LabelFps);

	//Ecran de parametrage
	x = 10;
	y = 10;
	g_screen_params = new GUIScreen();

	GUIBouton * btnClose = new GUIBouton();
	btnClose->Titre = std::string("Close");
	btnClose->X = x;
	btnClose->setOnClick(clickBtnCloseParam);
	g_screen_params->addElement(btnClose);

	y += btnClose->Height + 1;
	y+=10;
	x+=10;

	GUILabel * label = new GUILabel();
	label->X = x;
	label->Y = y;
	label->Text = "Param :";
	g_screen_params->addElement(label);

	y += label->Height + 1;

	g_slider = new GUISlider();
	g_slider->setPos(x,y);
	g_slider->setMaxMin(1,0);
	g_slider->Visible = true;
	g_screen_params->addElement(g_slider);

	y += g_slider->Height + 1;
	y+=10;

	//Ecran a rendre
	g_screen_manager->setActiveScreen(g_screen_jeu);

	//Init Camera
	g_renderer->_Camera->setPosition(NYVert3Df(-50,-50,250));
	g_renderer->_Camera->setLookAt(NYVert3Df(0,0,200));



	//On définit une lumière 
	lightPostion.Z = 7;

	g_world = new NYWorld();
	g_world->_FacteurGeneration = 5;
	g_world->init_world();

	//Fin init moteur

	avatar = new NYAvatar(g_renderer->_Camera,g_world);
	avatar->Speed = NYVert3Df(60,60,60);
	//Init application
	//lapin = new IABase();

	RessourcesManager *pRessourceMgr = RessourcesManager::GetSingleton();
	pRessourceMgr->Create(HERBE, NYVert3Df(120,120,g_world->_MatriceHeights[12][12]*10),1000);
	pRessourceMgr->Create(HERBE, NYVert3Df(130, 130, g_world->_MatriceHeights[13][13] * 10), 1000);
	pRessourceMgr->Create(CROTTE, NYVert3Df(110, 160, g_world->_MatriceHeights[12][12] * 10), 1000);

	//Init Timer
	g_timer = new NYTimer();

	//Init FSM
	FSM * master_FSM = FSM::getSingleton();

	//Init Test State Machine
	entityTest1 = new TestStateMachine();
	entityTest2 = new TestStateMachine();

	entityTest1->SetEntityToCommunicateWith(entityTest2->GetID());
	entityTest2->SetEntityToCommunicateWith(entityTest1->GetID());

	//Init PF
	Pathfinding * pf = Pathfinding::GetSingleton();
	pf->SetWorld(g_world);

	//Example Pathfinding
	NYVert3Df depart  = avatar->Position/NYCube::CUBE_SIZE;
	depart.Z += -5;
	NYVert3Df arrivee = avatar->Position/NYCube::CUBE_SIZE;
	arrivee.Z += 3;
	arrivee.X += 60;
	arrivee.Y += 60;

	/*if (pf->FindPath(depart, arrivee, (int)CUBE_AIR | (int)CUBE_TERRE |(int)CUBE_HERBE, returnPath))
	{
		//returnPath.PrintPath();
	}*/

	//if (pf->FindPath(NYVert2Df(depart.X, depart.Y), NYVert2Df(arrivee.X, arrivee.Y), 1 ,returnPath))
	//{
	//	//returnPath.PrintPath();
	//}

	srand(time(NULL));

	spawnCreatures();
	
	//On start
	g_timer->start();

	glutMainLoop(); 

	delete(entityTest1);
	delete(entityTest2);

	return 0;
}

