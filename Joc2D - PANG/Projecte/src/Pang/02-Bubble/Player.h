#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Player
{

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram);
	void update(int deltaTime);
	void render();
	
	void setTileMap(TileMap *tileMap);
	void setPosition(const glm::vec2 &pos);
	glm::ivec2 getPos();
	void setDie();
	bool getDie();
	bool interseccio(glm::ivec2& topLeft1, int width1, int height1,
		glm::ivec2& topLeft2, int width2, int height2) ;

	void dieAnim();
private:
	
	
	bool bJumping, die;
	glm::ivec2 tileMapDispl, posPlayer;
	int jumpAngle, startY, fire_cooldown, shooting, frames;
	Texture spritesheet;
	Sprite* sprite;
	Sprite* spriteDie;
	TileMap *map;

};


#endif // _PLAYER_INCLUDE


