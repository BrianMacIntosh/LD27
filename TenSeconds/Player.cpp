#include "stdafx.h"
#include "Player.h"
#include "InvControls.h"
#include "InvTexture.h"
#include "PirateParams.h"
#include "EngineScreen.h"
#include "Map.h"
#include "ItemManager.h"
#include "RasterLineIterator.h"
#include "InvScreenManagement.h"
#include "GameOverScreen.h"
#include <SDL_mixer.h>

static const float PlayerBaseMoveSpeed = 70.0f;
static const float PlayerBaseAccel = 260.0f;

static const float PlayerRadius = 10;
static const float PlayerHeight = 10;

static const float WorkRange = 64;
static const int PlayerInventorySpace = 5;

static const float PerceptionRange = 1.2f;
static const float FlashlightRange = 7.3f;
static const float FlashlightArc = 0.9f;

static const float ShieldRadius = 128;
static const float ShieldLife = 20.0f;

static const float DamageEffectTime = 1.0f;

static const int MaxHealth = 4;


static bool init = false;
static InvTexture* m_BaseTexture;
static InvTexture* m_ArmLF;
static InvTexture* m_ArmLB;
static InvTexture* m_ArmRF;
static InvTexture* m_ArmRB;
static InvTexture* m_Breakbar;
static InvTexture* m_Breakback;
static InvTexture* m_ItemFrame;
static InvTexture* m_ItemFrameBad;
static InvTexture* m_ItemFrameExcess;
static InvTexture* m_ItemTabNext;
static InvTexture* m_Shield;
static InvTexture* m_ShieldCenter;
static InvTexture* m_DamageEffect;
static InvTexture* m_DeadPlayer;
static InvTexture* m_HealthTag;
static InvTexture* m_HealthIcon;

static Mix_Chunk* m_ShieldsUp;
static Mix_Chunk* m_BuildSfx;


Player::Player()
	:Actor()
{
	health = MaxHealth;
	dieFade = 0;

	m_HipsTexture = new InvTexture("Resource/Player/player_hips.png");
	m_LegRF = new InvTexture("Resource/Player/player_legrf.png");
	m_LegLF = new InvTexture("Resource/Player/player_leglf.png");

	if (!init)
	{
		init = true;
		m_BaseTexture = new InvTexture("Resource/Player/player_base.png");

		m_ArmLB = new InvTexture("Resource/Player/player_armlb.png");
		m_ArmLF = new InvTexture("Resource/Player/player_armlf.png");
		m_ArmRB = new InvTexture("Resource/Player/player_armrb.png");
		m_ArmRF = new InvTexture("Resource/Player/player_armrf.png");

		m_Breakbar = new InvTexture("Resource/Ui/progbar_green.png");
		m_Breakback = new InvTexture("Resource/Ui/progbar_back.png");

		m_Shield = new InvTexture("Resource/shield.png");
		m_ShieldCenter = new InvTexture("Resource/shield_center.png");

		m_ItemFrame = new InvTexture("Resource/itemframe.png");
		m_ItemFrameBad = new InvTexture("Resource/itemframe_full.png");
		m_ItemFrameExcess = new InvTexture("Resource/itemframe_excess.png");
		m_ItemTabNext = new InvTexture("Resource/itemframe_tabnext.png");

		m_DamageEffect = new InvTexture("Resource/Ui/damageeffect.png");

		m_DeadPlayer = new InvTexture("Resource/Player/dead.png");
		m_HealthTag = new InvTexture("Resource/Ui/healthtag.png");
		m_HealthIcon = new InvTexture("Resource/Ui/healthicon.png");

		m_ShieldsUp = Mix_LoadWAV("Resource/Audio/shield.wav");
		m_BuildSfx = Mix_LoadWAV("Resource/Audio/build.wav");
		Mix_VolumeChunk(m_BuildSfx, 60);
	}

	invoOffset = 0;
	blockBreak = false;

	shieldLife = 0;

	m_Radius = PlayerRadius;

	inventory.push_back(ITEM_FLASHLIGHT);
}

Player::~Player()
{
	delete m_LegLF;
	delete m_LegRF;
	delete m_HipsTexture;
}

void Player::damage()
{
	if (health > 0)
	{
		health--;
		damageShowTime = DamageEffectTime;
	}
}

void Player::update(double elapsed)
{
	if (damageShowTime > 0)
	{
		damageShowTime -= elapsed;
	}

	//Shield
	if (shieldLife > 0)
	{
		m_FellOff = false;
		posZ = 0;
		velZ = 0;
		shieldLife -= elapsed;
		if (shieldLife < 0)
			shieldLife = 0;
	}

	//Die
	if (health <= 0
		|| posZ < -50)
	{
		dieFade += elapsed;
		if (dieFade >= 1)
		{
			addGameScreen(new GameOverScreen());
			getInstance()->close();
		}
	}

	if (!m_FellOff && health > 0)
	{
		InvVectorf2 target;
		target.x = floor(getInstance()->mouseWorldPos.x / TileSize);
		target.y = floor(getInstance()->mouseWorldPos.y / TileSize);

		if (getInstance()->m_Map->coordValid(target)
			&& (m_Position - getInstance()->mouseWorldPos).lengthSquared() <= WorkRange * WorkRange)
			m_Target = target;
		else
			m_Target = InvVectorf2(-1,-1);

		//Uproot structures
		if (mouseDown(3) && !blockBreak)
		{
			if (getInstance()->m_Map->coordValid(m_Target))
			{
				int targettype = getInstance()->m_Map->m_Map[(int)m_Target.x][(int)m_Target.y].TileType;
		
				if (getTileBase(targettype).breakablility > 0)
				{
					getInstance()->m_Map->m_Map[(int)m_Target.x][(int)m_Target.y].condition -= elapsed;
				}
			}
		}

		if (mouseUp(3))
			blockBreak = false;

		//Drop items
		if (mousePressed(2))
		{
			if (inventory.size() > 0
				&& getInstance()->m_Map->coordValid(m_Target)
				&& getInstance()->m_Map->getTile(m_Target).ItemType == ITEM_NONE)
			{
				getInstance()->m_Map->m_Map[(int)m_Target.x][(int)m_Target.y].ItemType = inventory[0];
				inventory.erase(inventory.begin());
			}
		}

		//Pick up items
		if (mousePressed(1))
		{
			if (getInstance()->m_Map->coordValid(m_Target))
			{
				int targettype = getInstance()->m_Map->m_Map[(int)m_Target.x][(int)m_Target.y].ItemType;
		
				if (targettype != ITEM_NONE)
				{
					//Take
					if (getItemBase(targettype).weight <= getAvailableInventory())
					{
						if (getItemBase(targettype).consumed)
						{
							if (getItemBase(targettype).shield)
							{
								Mix_PlayChannel(0, m_ShieldsUp, 0);
								shieldLife = ShieldLife;
								getInstance()->spawnBread();
							}
						}
						else
							inventory.push_back(targettype);
						getInstance()->m_Map->m_Map[(int)m_Target.x][(int)m_Target.y].ItemType = ITEM_NONE;
					}
				}
				else
				{
					//Place
					if (inventory.size() > 0
						&& getTileBase(getInstance()->m_Map->getTile(m_Target).TileType).buildable
						&& getItemBase(inventory[0]).buildsTile >= 0)
					{
						blockBreak = true;
						getInstance()->m_Map->m_Map[(int)m_Target.x][(int)m_Target.y].Build(inventory[0]);
						inventory.erase(inventory.begin());
						Mix_PlayChannel(0, m_BuildSfx, 0);
					}
				}
			}
		}

		angle = atan2f(
			getInstance()->mouseWorldPos.x - m_Position.x,
			getInstance()->mouseWorldPos.y - m_Position.y) * (180.0f / PI);

		//Input
		InvVectorf2 mvmt;
		if (keyDown(SDLK_w))
			mvmt.y--;
		if (keyDown(SDLK_s))
			mvmt.y++;
		if (keyDown(SDLK_a))
			mvmt.x--;
		if (keyDown(SDLK_d))
			mvmt.x++;

		if (mvmt.x != 0 || mvmt.y != 0)
		{
			m_Velocity += mvmt * getAccel() * elapsed;
			float maxspeed = getMoveSpeed();
			if (m_Velocity.lengthSquared() > maxspeed * maxspeed)
				m_Velocity = m_Velocity.normalize() * maxspeed;
		}
		else
		{
			InvVectorf2 prevSigns = m_Velocity.getSigns();
			m_Velocity -= m_Velocity.normalize() * getAccel() * elapsed;
			if (prevSigns != m_Velocity.getSigns())
				m_Velocity = InvVectorf2();
		}
	}
	else
	{
		//Fell off, accelerate away
		m_Velocity += m_Velocity.normalize() * elapsed * 300;
	}

	//Emit light
	getInstance()->m_Map->unfogInRadius(m_Position, shieldLife > 0 ? ShieldRadius / TileSize + 0.05f : PerceptionRange);
	if (inventory.size() > 0 && getItemBase(inventory[0]).flashlight)
	{
		InvVectorf2 facing = InvVectorf2(
			sinf(angle * (PI / 180)),
			cosf(angle * (PI / 180)));
		getInstance()->m_Map->unfogInCone(
			m_Position - facing.normalize() * 50.0f, FlashlightRange, angle * (PI/180.0f), FlashlightArc);
	}

	//Move and do collision
	Actor::update(elapsed);

	//Push enemies away
	if (shieldLife > 0)
	{
		for (auto it = getInstance()->m_Enemies.begin(); it != getInstance()->m_Enemies.end(); it++)
		{
			InvVectorf2 disp = (*it)->getPosition() - m_Position;
			if (disp.lengthSquared() < ShieldRadius * ShieldRadius)
				(*it)->setPosition(m_Position + disp.normalize() * ShieldRadius);
		}
	}

	//Animate inventory
	if (invoOffset > 0)
	{
		invoOffset -= elapsed / 0.2f;
		if (invoOffset < 0)
			invoOffset = 0;
	}

	//Shuffle inventory
	int invdir = 0;
	if (keyPressed(SDLK_TAB))
	{
		if (keyDown(SDLK_LSHIFT))
			invdir = -1;
		else
			invdir = 1;
	}
	if (invdir > 0 && inventory.size() > 0)
	{
		if (invdir > 0)
		{
			int item = inventory[0];
			for (int c = 0; c < inventory.size() - 1; c++)
				inventory[c] = inventory[c+1];
			inventory[inventory.size()-1] = item;
		}
		else
		{
			int item = inventory[inventory.size()-1];
			for (int c = inventory.size() - 1; c > 0; c--)
				inventory[c] = inventory[c-1];
			inventory[0] = item;
		}
		invoOffset = invdir;
	}
}

void Player::draw()
{
	InvVectorf2 facing = InvVectorf2(
		sinf(angle * (PI / 180)),
		cosf(angle * (PI / 180)));

	if (health <= 0)
	{
		//Dead body
		m_DeadPlayer->draw(
			InvVectorf3(m_Position.x, m_Position.y, 2),
			InvVectorf2(m_DeadPlayer->getWidth() / 2, m_DeadPlayer->getHeight()),
			angle);

		return;
	}

	InvTexture* larm = m_ArmLB;
	InvTexture* rarm = m_ArmRF;
	float scale = sinf(armOsc);
	if (armOsc > PI)
	{
		larm = m_ArmLF;
		rarm = m_ArmRB;
		scale *= -1;
	}
	float armscale = -scale * facing.normalize().dotProduct(m_Velocity.normalize());
	armscale = signum(armscale) * (1 - (1-abs(armscale)) * (1-abs(armscale)));

	float armwid = larm->getWidth() / 2;
	float armht = larm->getHeight() / 2;
	larm->draw(
		InvVectorf3(-armwid, -armscale * armht, PlayerHeight - 2 + posZ),
		InvVectorf3( armwid, -armscale * armht, PlayerHeight - 2 + posZ),
		InvVectorf3( armwid,  armscale * armht, PlayerHeight - 2 + posZ),
		InvVectorf3(-armwid,  armscale * armht, PlayerHeight - 2 + posZ),
		m_Position,
		angle);
	rarm->draw(
		InvVectorf3(-armwid, -armscale * armht, PlayerHeight - 2 + posZ),
		InvVectorf3( armwid, -armscale * armht, PlayerHeight - 2 + posZ),
		InvVectorf3( armwid,  armscale * armht, PlayerHeight - 2 + posZ),
		InvVectorf3(-armwid,  armscale * armht, PlayerHeight - 2 + posZ),
		m_Position,
		angle);

	m_BaseTexture->draw(
		InvVectorf3(m_Position.x, m_Position.y, PlayerHeight + posZ),
		InvVectorf2(m_BaseTexture->getWidth() / 2, m_BaseTexture->getHeight() / 2),
		angle);

	Actor::draw();

	//Draw shield
	if (shieldLife > 0)
	{
		m_Shield->draw(
			InvVectorf3(m_Position.x, m_Position.y, 5),
			InvVectorf2(m_Shield->getWidth(), m_Shield->getHeight()) / 2);
		m_ShieldCenter->draw(
			InvVectorf3(m_Position.x, m_Position.y, 0),
			InvVectorf2(m_ShieldCenter->getWidth(), m_ShieldCenter->getHeight()) / 2);
	}
}

void Player::drawGui()
{
	//Inventory
	for (int slot = 0; slot < PlayerInventorySpace; slot++)
	{
		if (slot < inventory.size())
			getItemBase(inventory[slot]).texture->draw(
				InvVectorf3(50 + (slot + invoOffset) * 50, 50, 0),
				InvVectorf2(getItemBase(inventory[slot]).texture->getWidth(), getItemBase(inventory[slot]).texture->getHeight()) / 2, 0);
		InvTexture* frametex = m_ItemFrameExcess;
		if (slot == 0)
		{
			if (getAvailableInventory() <= 0)
				frametex = m_ItemFrameBad;
			else
				frametex = m_ItemFrame;
		}
		else if (slot == 1)
		{
			m_ItemTabNext->draw(
				InvVectorf3(165, 50, 0),
				InvVectorf2(20, 20), 0);
		}
		frametex->draw(
			InvVectorf3(50 + slot * 50, 50, 0),
			InvVectorf2(20, 20), 0);
	}

	//Health
	m_HealthTag->draw(
		InvVectorf3(screenWidth - 20, 20, 0),
		InvVectorf2(m_HealthTag->getWidth(), 0));
	for (int c = 0; c < health; c++)
		m_HealthIcon->draw(
		InvVectorf3(screenWidth - 20 + 20 * c, 20, 0),
		InvVectorf2(m_HealthIcon->getWidth(), 0));

	//Break bar
	if (getInstance()->m_Map->coordValid(m_Target))
	{
		int targettype = getInstance()->m_Map->m_Map[(int)m_Target.x][(int)m_Target.y].TileType;
		if (getTileBase(targettype).breakablility > 0)
		{
			InvVectorf2 dpos = getInstance()->project(InvVectorf3(m_Target.x + 0.5f, m_Target.y + 0.5f, 0) * TileSize);
			m_Breakback->draw(
				InvVectorf3(dpos.x - m_Breakback->getWidth() / 2, dpos.y - m_Breakback->getHeight() / 2, 0),
				InvVectorf3(dpos.x + m_Breakback->getWidth() / 2, dpos.y - m_Breakback->getHeight() / 2, 0),
				InvVectorf3(dpos.x + m_Breakback->getWidth() / 2, dpos.y + m_Breakback->getHeight() / 2, 0),
				InvVectorf3(dpos.x - m_Breakback->getWidth() / 2, dpos.y + m_Breakback->getHeight() / 2, 0));
			float rat = getInstance()->m_Map->m_Map[(int)m_Target.x][(int)m_Target.y].condition / 
				getTileBase(targettype).breakablility;
			m_Breakbar->draw(
				InvVectorf3(dpos.x - m_Breakback->getWidth() / 2, dpos.y - m_Breakback->getHeight() / 2,            0.05),
				InvVectorf3(dpos.x + (rat - 0.5f) * m_Breakback->getWidth(), dpos.y - m_Breakback->getHeight() / 2, 0.05),
				InvVectorf3(dpos.x + (rat - 0.5f) * m_Breakback->getWidth(), dpos.y + m_Breakback->getHeight() / 2, 0.05),
				InvVectorf3(dpos.x - m_Breakback->getWidth() / 2, dpos.y + m_Breakback->getHeight() / 2,            0.05));
		}
	}

	//Damage
	if (damageShowTime > 0)
	{
		glColor4f(1, 1, 1, damageShowTime / DamageEffectTime);
		m_DamageEffect->draw(
			InvVectorf3(0,           0,            -0.9f),
			InvVectorf3(screenWidth, 0,            -0.9f),
			InvVectorf3(screenWidth, screenHeight, -0.9f),
			InvVectorf3(0,           screenHeight, -0.9f));
		glColor4f(1, 1, 1, 1);
	}

	//Fader
	if (dieFade > 0)
	{
		glColor4f(1, 1, 1, dieFade);
		getInstance()->m_Black->draw(
			InvVectorf3(0,           0,            -1),
			InvVectorf3(screenWidth, 0,            -1),
			InvVectorf3(screenWidth, screenHeight, -1),
			InvVectorf3(0,           screenHeight, -1));
		glColor4f(1, 1, 1, 1);
	}
}

int Player::getAvailableInventory()
{
	int used = 0;
	for (auto it = inventory.begin(); it != inventory.end(); it++)
		used += getItemBase(*it).weight;
	return PlayerInventorySpace - used;
}

float Player::getAccel() const
{
	return PlayerBaseAccel;
}

float Player::getMoveSpeed() const
{
	return PlayerBaseMoveSpeed;
}