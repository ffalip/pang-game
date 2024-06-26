#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "Bullet.h"
#include "Bubble.h"
#include "Background.h"
#include "Interface.h"
#include "Sprite.h"
#include "BubbleExplosions.h"
#include "Dinamita.h"
#include "PararTemps.h"
#include "Invencibility.h"
#include "Food.h"
#include "Menu.h"

// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	~Scene();

	void init();
	void update(int deltaTime);
	void render();

private:
	void initShaders();
	void peta(int i);
	void petaTot(vector<bool>& bubblesActives, vector<Bubble*>& bubbles, vector<BubbleExplosions*>& bubExs);
	void resetScene();
	int calcScore(int size);
private:
	Background* bg;
	TileMap *map;
	Player *player;
	Bullet *bullet;
	Bubble *bubble;
	vector<Bubble*> bubbles;
	BubbleExplosions *bubEx;
	vector<BubbleExplosions*> bubExs;
	vector<bool> bubblesActives;
	Dinamita* din;
	PararTemps* pt;
	Invencibility* inv;
	Food* fd;
	ShaderProgram texProgram;
	Interface *timeDisp;
	Menu* menu;
	float currentTime;
	glm::mat4 projection;
	int f;
	int t;
	int idLevel = 1;
	int contadorFreeze, contadorInvencibilitat, contadorMort;
	bool activarContadorFreeze, activarContadorInvencibilitat, activarContadorMort;
	bool dinAct, ptAct, invAct, fdAct;
	bool invAplied;
	bool guanyat;
	bool perdut;
	bool godMode;
	bool buttonPressed;
	int window = 0;
	int timer = 0;
	int lastPopped = -1; 
	int scoreMult = 1;
	int lvlScore = 0;
	int score = 0;
	int lifes = 4;
};


#endif // _SCENE_INCLUDE

