#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"

#define SCREEN_X 16
#define SCREEN_Y 16

#define INIT_PLAYER_X_TILES 4
#define INIT_PLAYER_Y_TILES 0

enum Win
{
	MENU, CONTROLS, CREDITS, LEVELS
};

Scene::Scene()
{
	map = NULL;
	player = NULL;
}

Scene::~Scene()
{
	if(map != NULL)
		delete map;
	if(player != NULL)
		delete player;
}


void Scene::init()
{
	initShaders();

	
	//MENUS INIT
	menu = new Menu;
	menu->init(glm::ivec2(16,16), texProgram);
	menu->changeMenu(MENU);


	//LEVEL INIT
	if (idLevel == 1 || idLevel == 3) {
		map = TileMap::createTileMap("levels/lvl" + std::to_string(idLevel) + ".txt", glm::vec2(16, 16), texProgram);
		bg = Background::createBackground("images/bg" + std::to_string(idLevel) + ".png", glm::vec2(16, 16), texProgram);
		for (int i = 0; i < 16; ++i) {
			bubble = new Bubble();
			if (i == 1) {
				bubble->init(glm::ivec2(16, 16), texProgram, 0, 1.f, 130, activarContadorFreeze);
				bubble->setPosition(glm::vec2((10 + i) * map->getTileSize(), 4 * map->getTileSize()));
				bubble->setTileMap(map);
			}
			bubbles.push_back(bubble);
		}
		for (int i = 0; i < 16; ++i) {
			bubblesActives.push_back(false);
		}
		bubblesActives[1] = true;

		for (int i = 0; i < 16; ++i) {
			bubEx = new BubbleExplosions();
			bubEx->init(glm::ivec2(16, 16), texProgram);
			bubEx->setPosition(glm::vec2((10 + i) * map->getTileSize(), 10 * map->getTileSize()));
			bubEx->setTileMap(map);
			bubExs.push_back(bubEx);
		}
	}

	else if (idLevel == 2) {
		map = TileMap::createTileMap("levels/lvl" + std::to_string(idLevel) + ".txt", glm::vec2(16, 16), texProgram);
		bg = Background::createBackground("images/bg" + std::to_string(idLevel) + ".png", glm::vec2(16, 16), texProgram);
		for (int i = 0; i < 32; ++i) {
			bubble = new Bubble();

			if (i == 2) {
				bubble->init(glm::ivec2(16, 16), texProgram, 0, 1.f, 130, activarContadorFreeze);
				bubble->setPosition(glm::vec2((20 + i) * map->getTileSize(), 12));
				bubble->setTileMap(map);
			}
			else if (i == 3) {
				bubble->init(glm::ivec2(16, 16), texProgram, 0, 1.f, 130, activarContadorFreeze);
				bubble->setPosition(glm::vec2(40 * map->getTileSize(), 10 * map->getTileSize()));
				bubble->setTileMap(map);
			}

			bubbles.push_back(bubble);
		}
		
		for (int i = 0; i < 32; ++i) {
			bubblesActives.push_back(false);
		}
		bubblesActives[2] = true;
		bubblesActives[3] = true;

		for (int i = 0; i < 32; ++i) {
			bubEx = new BubbleExplosions();
			bubEx->init(glm::ivec2(16, 16), texProgram);
			bubEx->setPosition(glm::vec2((10 + i) * map->getTileSize(), 10 * map->getTileSize()));
			bubEx->setTileMap(map);
			bubExs.push_back(bubEx);
		}
	}


	bullet = new Bullet();
	bullet->init(glm::ivec2(16, 16), texProgram);
	bullet->setTileMap(map);

	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), 20 * map->getTileSize()));
	player->setTileMap(map);

	bullet->setPlayer(player);

	projection = glm::ortho(0.f, float(SCREEN_WIDTH), float(SCREEN_HEIGHT), 0.f);

	timeDisp = new Interface();
	timeDisp->init(glm::ivec2(16, 16), texProgram);

	timeDisp->updateScore(score);
	timeDisp->updateStage(idLevel);
	timeDisp->updateLife(lifes);
	contadorMort = contadorFreeze = contadorInvencibilitat = 0;
	activarContadorMort = activarContadorInvencibilitat =  activarContadorFreeze = invAct = ptAct = dinAct = invAplied = fdAct = godMode = false;
}

void Scene::update(int deltaTime)
{
	switch (window)
	{
	case MENU:
		menu->update(deltaTime);
		if (Game::instance().getKey(GLFW_KEY_S) && !buttonPressed) 
		{
			window = LEVELS;
			buttonPressed = true;
			timer = 60;
		}
		else if (Game::instance().getKey(GLFW_KEY_C) && !buttonPressed) 
		{
			window = CONTROLS;
			buttonPressed = true;
			timer = 60;
		}
		break;

	case CONTROLS:
		menu->update(deltaTime);
		if (Game::instance().getKey(GLFW_KEY_S) && !buttonPressed) 
		{
			window = LEVELS;
			buttonPressed = true;
			timer = 60;
		}
		else if (Game::instance().getKey(GLFW_KEY_C) && !buttonPressed) 
		{
			window = MENU;
			buttonPressed = true;
			timer = 60;
		}
		break;

	case CREDITS:
		//idLevel = 1;
		menu->update(deltaTime);
		if (Game::instance().getKey(GLFW_KEY_S) && !buttonPressed) 
		{	
			window = MENU;
			buttonPressed = true;
			timer = 60;
		}
		break;
	default:

		if (timeDisp->getLife() <= 0 || timeDisp->getTime() == 0) {
			perdut = true;
		}
		if (!guanyat) {
			bool algunBubbleActive = false;
			for (int i = 0; i < bubblesActives.size(); ++i) {
				if (bubblesActives[i]) {
					algunBubbleActive = true;
				}
			}
			if (algunBubbleActive) guanyat = false;
			else {
				guanyat = true;
				if (idLevel < 3) ++idLevel;
				else {
					idLevel = 1;
					window = CREDITS;
				}
				int timeScore = timeDisp->getTime() * 100;
				timeDisp->updateScore(timeScore);
				lvlScore += timeScore;
				std::cout << "guanyat!" << endl;
				resetScene();
			}
		}
		if (Game::instance().getKey(GLFW_KEY_G) && !buttonPressed)
		{
			buttonPressed = true;
			timer = 60;
			godMode = !godMode;
			if (godMode) cout << "GOD MODE ON" << endl;
			else cout << "GOD MODE OFF" << endl;
		}

		if ((dinAct && player->interseccio(player->getPos(), 32, 32, din->getPosition(), 16, 16) && !player->getDie()) || Game::instance().getKey(GLFW_KEY_T)) {
			petaTot(bubblesActives, bubbles, bubExs);
			dinAct = false;
		}

		if ((ptAct && player->interseccio(player->getPos(), 32, 32, pt->getPosition(), 16, 16)&& !player->getDie()) || Game::instance().getKey(GLFW_KEY_Y)) {
			contadorFreeze = 0;
			activarContadorFreeze = true;
			for (int i = 0; i < bubbles.size(); ++i) {
				bubbles[i]->freeze = true;
			}
			ptAct = false;
		}
		if (Game::instance().getKey(GLFW_KEY_U)) {
			invAct = true;
			invAplied = true;
			inv = new Invencibility();
			inv->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
			inv->setPosition(player->getPos());
			inv->setTileMap(map);
			inv->setPlayer(player);
		}
		if ((invAct && inv->getGetInv() && player->interseccio(player->getPos(), 32, 32, inv->getPosition(), 16, 16) && !player->getDie())) {
			inv->setApliedTrue();
			inv->setGetInvFalse();
			invAplied = inv->getAplied();
		}
		if (activarContadorInvencibilitat) {
			++contadorInvencibilitat;
			if (contadorInvencibilitat > 120) activarContadorInvencibilitat = false;
		}

		if (fdAct && player->interseccio(player->getPos(), 32, 32, fd->getPosition(), 16, 16) && !player->getDie()) {
			if (fd->getAnim() == 0) {
				timeDisp->updateScore(2000);
				lvlScore += 2000;
			}
			else if (fd->getAnim() == 1) {
				timeDisp->updateScore(4000);
				lvlScore += 4000;
			}
			else if (fd->getAnim() == 2) {
				timeDisp->updateScore(6000);
				lvlScore += 6000;
			}
			else if (fd->getAnim() == 3) {
				timeDisp->updateScore(10000);
				lvlScore += 10000;
			
			}
			fdAct = false;
		}

		if (activarContadorFreeze) {
			++contadorFreeze;
			if (contadorFreeze >= 300) {
				for (int i = 0; i < bubbles.size(); ++i) {
					bubbles[i]->freeze = false;
					activarContadorFreeze = false;
				}
			}
		}

		currentTime += deltaTime;
		for (int i = 0; i < bubblesActives.size(); ++i) {
			if (bubblesActives[i]) {
				bubbles[i]->update(deltaTime);
				if (bullet->shooting() && bubbles[i]->collisionWithBullet(bullet->getPos(), bullet->getHeight(), 4)) {
					peta(i);
					timeDisp->updateScore(calcScore(bubbles[i]->getSize()));
					bullet->stopShooting();
					int r = rand() % 12;
					if (!dinAct && r == 0) {
						dinAct = true;
						din = new Dinamita();
						din->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
						din->setPosition(glm::vec2(bubbles[i]->getPosition().x + 32, bubbles[i]->getPosition().y + 32));
						din->setTileMap(map);
					}
					if (!ptAct && r == 1) {
						ptAct = true;
						pt = new PararTemps();
						pt->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
						pt->setPosition(glm::vec2(bubbles[i]->getPosition().x + 32, bubbles[i]->getPosition().y + 32));
						pt->setTileMap(map);
					}
					if (!invAct && r == 2 || !invAct && r == 3) {
						invAct = true;
						inv = new Invencibility();
						inv->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
						inv->setPosition(glm::vec2(bubbles[i]->getPosition().x + 32, bubbles[i]->getPosition().y + 32));
						inv->setTileMap(map);
						inv->setPlayer(player);
					}
					if (!fdAct && r > 3) {
						fdAct = true;
						fd = new Food();
						int random = rand() % 13;
						if (random < 6) fd->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 0);
						else if (random >= 6 && random < 9) fd->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 1);
						else if (random >= 9 && random <= 11) fd->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 2);
						else if (random == 12) fd->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, 3);
						
						fd->setPosition(glm::vec2(bubbles[i]->getPosition().x + 32, bubbles[i]->getPosition().y + 24));
						fd->setTileMap(map);
					}
				}
				if (bubbles[i]->collisionWithPlayer(player->getPos(), 32, 32)) {
					if (!godMode && !player->getDie() &&  !invAplied && (!activarContadorInvencibilitat || contadorInvencibilitat >= 120)) {
						
						player->setDie();
						lifes -= 1;
						timeDisp->updateLife(lifes);
						activarContadorMort = true;
					}
					else if (activarContadorInvencibilitat) {
						if (invAplied) inv->setApliedFalse();
						invAplied = false;
						activarContadorInvencibilitat = true;
					}
					else {
						if (invAplied) inv->setApliedFalse();
						invAplied = false;
						
						activarContadorInvencibilitat = true;
						contadorInvencibilitat = 0;
					}
				}
			}
		}
		if (activarContadorMort || timeDisp->getTime() == 0) {
			timeDisp->unsetTimeActive();
			++contadorMort; 
			player->setDie();
			if (contadorMort > 120) {
				if (Game::instance().getKey(GLFW_KEY_R)) resetScene();
			}
		}
		for (int i = 0; i < bubExs.size(); ++i) {
			bubExs[i]->update(deltaTime);
		}

		if (map->getTileBroken()) {
			timeDisp->updateScore(500);
			lvlScore += 500;
			map->actTileBroken();
		}

		bullet->update(deltaTime);
		if (invAct) inv->update(deltaTime);
		player->update(deltaTime);
		timeDisp->update(deltaTime);
		if (dinAct) din->update(deltaTime);
		if (ptAct) pt->update(deltaTime);
		if (fdAct)fd->update(deltaTime);
		break;
	}
	//change between levels
	if (Game::instance().getKey(GLFW_KEY_0) && !buttonPressed) 
	{
		window = MENU;
		buttonPressed = true; 
		timer = 30;
		resetScene();
	}
	else if (Game::instance().getKey(GLFW_KEY_1) && !buttonPressed)
	{
		window = LEVELS;
		idLevel = 1;
		buttonPressed = true;
		timer = 30;
		resetScene();
	}
	else if (Game::instance().getKey(GLFW_KEY_2) && !buttonPressed)
	{
		window = LEVELS;
		idLevel = 2;
		buttonPressed = true;
		timer = 30;
		resetScene();
	}
	else if (Game::instance().getKey(GLFW_KEY_3) && !buttonPressed)
	{
		window = LEVELS;
		idLevel = 3;
		buttonPressed = true;
		timer = 30;
		resetScene();
	}
	else if (Game::instance().getKey(GLFW_KEY_4) && !buttonPressed)
	{
		window = CREDITS;
		buttonPressed = true;
		timer = 30;
		resetScene();
	}

	//decrease timer for button cooldown
	if (buttonPressed && timer > 0) --timer;
	else buttonPressed = false;
}


void Scene::render()
{
	glm::mat4 modelview;
	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

	switch (window)
	{
	case MENU:
		menu->changeMenu(MENU);
		menu->render();
		break;
	case CONTROLS:
		menu->changeMenu(CONTROLS);
		menu->render();
		break;
		
	case CREDITS: 
		menu->changeMenu(CREDITS);
		menu->render();
		break;

	default:
		bg->render();
		map->updateArrays(glm::vec2(16, 16), texProgram);
		map->render();
		for (int i = 0; i < bubblesActives.size(); ++i) {
			if (bubblesActives[i]) bubbles[i]->render();
		}

		for (int i = 0; i < bubExs.size(); ++i) {

			bubExs[i]->render();
		}

		bullet->render();
		if (invAct) inv->render();
		player->render();
		timeDisp->render();
		if (dinAct) din->render();
		if (ptAct) pt->render();
		if (fdAct)fd->render();
		break;
	}
}

void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if(!texProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}

void Scene::peta(int i) {
	//for (int i = 0; i < bubblesActives.size(); ++i) {
		if (bubblesActives[i]) {

			bubEx = new BubbleExplosions();
			bubEx->init(glm::ivec2(16, 16), texProgram);
			bubEx->setPosition(glm::ivec2(bubbles[i]->getPosition().x + 16, bubbles[i]->getPosition().y + 16));
			bubEx->setAnimation(bubbles[i]->getSize());
			bubEx->setTileMap(map);
			bubExs[i] = bubEx;

			bubblesActives[i] = false;

			bubExs[i]->setAnimation(bubbles[i]->getSize());

			if ((i * 2) + 1 < bubblesActives.size()) {
				bubble = new Bubble();
				bubble->init(glm::ivec2(16, 16), texProgram, bubbles[i]->getSize() + 1, bubbles[i]->getVelocity(), bubbles[i]->getStartY(), activarContadorFreeze);
				bubble->setPosition(glm::ivec2(bubbles[i] -> getPosition().x + 2, bubbles[i]->getPosition().y));
				bubble->setTileMap(map);
				bubbles[i * 2] = bubble;
				bubblesActives[i * 2] = true;
				
				bubble = new Bubble();
				bubble->init(glm::ivec2(16, 16), texProgram, bubbles[i]->getSize() + 1, -(bubbles[i]->getVelocity()), bubbles[i]->getStartY(), activarContadorFreeze);
				bubble->setPosition(glm::ivec2(bubbles[i]->getPosition().x - 2, bubbles[i]->getPosition().y));
				bubble->setTileMap(map);
				bubbles[i * 2+1] = bubble;
				bubblesActives[i * 2+1] = true;
				//break;
				
			}
			//break;
		}
	//}
}

void Scene::petaTot(vector<bool>& bubblesActives, vector<Bubble*>& bubbles, vector<BubbleExplosions*>& bubExs) {
	for (int i = 0; i < bubblesActives.size(); ++i) {
		if (bubblesActives[i] && bubbles[i]->getSize() != 3) {

			bubEx = new BubbleExplosions();
			bubEx->init(glm::ivec2(16, 16), texProgram);
			bubEx->setPosition(glm::ivec2(bubbles[i]->getPosition().x + 8, bubbles[i]->getPosition().y + 8));
			bubEx->setAnimation(bubbles[i]->getSize());
			bubEx->setTileMap(map);
			bubExs[i] = bubEx;

			bubblesActives[i] = false;
			bubExs[i]->setAnimation(bubbles[i]->getSize());


			if ((i  * 2) + 1 < bubblesActives.size()) {
				bubble = new Bubble();
				bubble->init(glm::ivec2(16, 16), texProgram, bubbles[i]->getSize() + 1, bubbles[i]->getVelocity(), bubbles[i]->getStartY()+10, activarContadorFreeze);
				bubble->setPosition(glm::ivec2(bubbles[i]->getPosition().x + rand() % 10, bubbles[i]->getPosition().y));
				bubble->setTileMap(map);
				bubbles[i * 2] = bubble;
				bubblesActives[i * 2] = true;

				bubble = new Bubble();
				bubble->init(glm::ivec2(16, 16), texProgram, bubbles[i]->getSize() + 1, -(bubbles[i]->getVelocity()), bubbles[i]->getStartY()+10, activarContadorFreeze);
				bubble->setPosition(glm::ivec2(bubbles[i]->getPosition().x - rand()%10, bubbles[i]->getPosition().y));
				bubble->setTileMap(map);
				bubbles[(i * 2)+1] = bubble;
				bubblesActives[(i * 2)+1] = true;
				

			}
		}
	}
}

void Scene::resetScene() {
	// Eliminar los objetos actuales
	if (map != nullptr) {
		delete map;
		map = nullptr;
	}
	if (player != nullptr) {
		delete player;
		player = nullptr;
	}
	for (auto& bubble : bubbles) {
		delete bubble;
	}
	bubbles.clear();
	for (auto& bubEx : bubExs) {
		delete bubEx;
	}
	bubExs.clear();
	delete bullet;
	bullet = nullptr;
	delete fd;
	fd = nullptr;


	if (guanyat) score += lvlScore;
	//else if (perdut) lifes -= 1;

	if (lifes == 0) {
		window = MENU;
		idLevel = 1;
		lifes = 4;
		score = 0;
		idLevel = 1;
	}
		

	delete din;
	din = nullptr;
	delete pt;
	pt = nullptr;
	delete inv;
	inv = nullptr;

	// Reiniciar variables de estado
	guanyat = false;
	perdut = false;
	bubblesActives.clear();
	invAplied = dinAct = ptAct = invAct = activarContadorFreeze = fdAct = activarContadorInvencibilitat =  false;
	contadorFreeze = contadorInvencibilitat =  0;

	// Volver a inicializar la escena
	init();
}

int Scene::calcScore(int size) 
{
	if (size == lastPopped && scoreMult < 8)
		scoreMult *= 2;
	else if (size != lastPopped)
		scoreMult = 1;
	lastPopped = size;
	lvlScore += 50 * (size + 1) * scoreMult;
	return (50 * (size + 1) * scoreMult);
}
