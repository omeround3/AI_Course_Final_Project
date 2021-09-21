#pragma once
#include <string>

// Constants
const int SURVIVE = 100;
const int ASSISTANCE = 600;
const int ATTACK = 500;
const int ATTACK_HEALTH_AMMU = 490;
const int ATTACK_AND_AMMU = 470;
const int ATTACK_AND_HEALTH = 450;
const int NEED_AMMUNITION = 150;
const int MAX_AMMUNITION = 10;
const int MAX_HEALTH = 100;

class Player
{
public:
	// Constructors
	Player();
	Player(int team, int x, int y, int id, int last_room_index, int health_low_threshold, int health_high_threshold, int ammu_threshold, bool can_attack);

	// Destructor
	~Player();

	// Getters
	int getId();
	int getTeam();
	int getMode();
	int getX();
	int getY();
	int getLastRIndex();
	bool isPositionEquals(int x, int y);
	bool hasAmmunition();
	bool isAlive();
	bool canAttack();
	bool needsAmmunition();
	bool needsHealth();

	// Setters
	void hit();
	void loadAmmu(int amount);
	void loadHealth(int amount);
	void gotHit(int damage);
	void move(int x, int y);
	void setLastRIndex(int index);
	bool assist(Player *other);

private:
	int health_, ammunition_, team_, x_, y_, id_;
	int last_r_index_, last_mode_, low_health_threshold_, high_health_threshold_, ammunition_threshold_;
	bool alive_, can_attack_;
};

