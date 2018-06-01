#pragma once

#include "Actor.h"
#include <vector>
#include "PirateParams.h"

class InvTexture;

class Player : public Actor
{
public:

	Player();
	~Player();

	virtual void update(double elapsed) override;

	virtual void draw() override;

	void drawGui();

	void damage();

	int getAvailableInventory();

	void spawn()
	{
		m_FellOff =false;
		m_Position = (SpawnPoint + InvVectorf2(0.5f, 0.5f)) * TileSize;
	};

	InvVectorf2 SpawnPoint;

private:

	std::vector<int> inventory;

	float getAccel() const;
	float getMoveSpeed() const;

	InvVectorf2 m_Target;

	float invoOffset;
	
	float shieldLife;

	float angle;

	float damageShowTime;

	bool blockBreak;

	int health;
	double dieFade;
};
