#pragma once

#include "Actor.h"
#include "InvSpacialData.h"

class Enemy : public Actor
{
public:

	Enemy(InvVectorf2 position);

	virtual void update(double elapsed) override;

	virtual void draw() override;

	float attackCooldown;

	float attackSpeed;
};