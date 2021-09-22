#include "Player.h"
#include <iostream>
using namespace std;

// Constructors
Player::Player()
{
}

Player::Player(int team, int x, int y, int id, int last_room_index, int health_low_threshold, int health_high_threshold, int ammu_threshold, bool can_attack)
{
	this->health_ = 20;
	this->ammunition_ = 5;
	this->alive_ = true;
	this->can_attack_ = can_attack;
	this->x_ = x;
	this->y_ = y;
	this->team_ = team;
	this->id_ = id;
	this->last_r_index_ = last_room_index;
	this->last_mode_ = 0;
	this->low_health_threshold_ = health_low_threshold;
	this->high_health_threshold_ = health_high_threshold;
	this->ammunition_threshold_ = ammu_threshold;
	cout << "[ADDED PLAYER] Player: " << this->id_ << " | Team: " << ((team == 12) ? 'A' : 'B') << " | Ammunition: " << this->ammunition_ << " | Health: " << this->health_ << " | Can Attack: " << (this->canAttack() ? "Yes" : "No") << endl;
	cout << "[PLAYER " << this->id_ << " STATUS] Health Low Threshold: " << health_low_threshold << " |  Health High Threshold: " << health_high_threshold << " | Ammunition Threshold: " << ammu_threshold << endl;

}

// Destructor
Player::~Player()
{
}

// Getters
int Player::getId()
{
	return this->id_;
}


int Player::getTeam()
{
	return this->team_;
}


int Player::getMode()
{
	int mode = -1;
	string mode_str = "Mode Not Set";
	if (health_ < this->low_health_threshold_) {
		mode = SURVIVE;
		mode_str = "Survival Mode";
	}
	else if (health_ <= this->high_health_threshold_ && ammunition_ <= this->ammunition_threshold_) {
		mode = ATTACK_HEALTH_AMMU;
		mode_str = "Health & Ammunition Mode | Need Ammunition and Health Points";
	}
	else if (!this->canAttack() && health_ >= this->low_health_threshold_ && this->ammunition_ > 0) {
		// Assistance player
		mode = ASSISTANCE;
		mode_str = "Assistance Mode";
	}
	else if (this->ammunition_ == 0) {
		mode = NEED_AMMUNITION;
		mode_str = "No Ammunition. Can't attack! ";
	}
	else if (this->ammunition_ <= this->ammunition_threshold_) {
		mode = ATTACK_AND_AMMU;
		mode_str = "Ammunition Gathering Mode | Need More Ammunition ";
	}
	else if (this->health_ <= this->high_health_threshold_) {
		mode = ATTACK_AND_HEALTH;
		mode_str = "Health Gathering Mode | Need More Health Points ";
	}
	else if (this->canAttack()) {
		mode = ATTACK;
		mode_str = "Attack Mode";
	}
	if (mode != last_mode_)
	{
		cout << "[PLAYER " << this->id_ << " MODE STATUS] Current Mode: " << mode_str << endl;
	}
	last_mode_ = mode;
	return mode;
}

int Player::getX()
{
	return this->x_;
}

int Player::getY()
{
	return this->y_;
}

int Player::getLastRIndex()
{
	return this->last_r_index_;
}

bool Player::isPositionEquals(int x, int y)
{
	return (this->x_ ==  x && this->y_ ==  y);
}

bool Player::isAlive()
{
	return this->alive_;
}


bool Player::hasAmmunition()
{
	return (ammunition_ > 0);
}

bool Player::canAttack()
{
	return this->can_attack_;
}

bool Player::needsAmmunition()
{
	return (this->ammunition_ < this->ammunition_threshold_);
}

bool Player::needsHealth()
{
	return (this->health_ < this->high_health_threshold_);
}

void Player::hit()
{
	if (ammunition_ > 0)
	{
		ammunition_--;
		cout << "[PLAYER " << this->id_ << " STATS] Ammunition Amount: " << ammunition_ << endl;
	}
	else
	{
		cout << "[PLAYER " << this->id_ << " STATUS] No Ammunition. Can't shoot." << endl;
	}
}

bool Player::assist(Player * other)
{
	bool assisted = false;
	if (other->ammunition_ < MAX_AMMUNITION)
	{
		int ammu_needed = MAX_AMMUNITION - other->ammunition_;
		if (this->ammunition_ > 0 && ammu_needed > 0)
		{
			cout << "[PLAYER " << this->getId() << " ASSISTANCE] Assisted Team Player: " << other->getId() << " | Transferred Ammunition: 1" << endl;
			other->loadAmmu(1);
			this->ammunition_ -= 1;
			assisted = true;
		}
	}

	int assist_amount = 0;
	if (other->health_ < MAX_HEALTH)
	{
		int health_needed = MAX_HEALTH - other->health_;
		if (this->health_ >= this->low_health_threshold_ && health_needed > 0)
		{
			assist_amount = this->health_ * 0.9;
			if (assist_amount > health_needed)
			{
				cout << "[PLAYER " << this->getId() << " ASSISTANCE] Assisted Team Player: " << other->getId() << " | Transferred Health: " << health_needed << endl;
				other->loadHealth(health_needed);
				this->health_ -= health_needed;
			}
			else
			{
				cout << "[PLAYER " << this->getId() << " ASSISTANCE] Assisted Team Player: " << other->getId() << " | Transferred Health: " << assist_amount << endl;
				other->loadHealth(assist_amount);
				this->health_ -= assist_amount;
			}
			assisted = true;
		}
	}
	if (assisted) 
	{
		
		cout << "[PLAYER " << this->id_ << " STATS] New Ammunition Amount: " << this->ammunition_ << " | Current Health: " << this->health_ << endl;
		cout << "[PLAYER " << other->id_ << " STATS] New Ammunition Amount: " << other->ammunition_ << " | Current Health: " << other->health_ << endl;
		this->getMode();
	}
	return assisted;
}


void Player::loadAmmu(int amount)
{
	if (ammunition_ + amount >= MAX_AMMUNITION)
	{
		ammunition_ = MAX_AMMUNITION;
		cout << "[PLAYER " << this->id_ << " STATS] Loaded " << amount << " Ammunition | Current Ammunition: " << this->ammunition_ << endl;
		this->getMode();
	}
	else
	{
		ammunition_ += amount;
		cout << "[PLAYER " << this->id_ << " STATS] Loaded " << amount << " Ammunition | Current Ammunition: " << this->ammunition_ << endl;
		this->getMode();
	}
}

void Player::loadHealth(int amount)
{
	if (this->health_ + amount >= MAX_HEALTH)
	{
		this->health_ = MAX_HEALTH;
		cout << "[PLAYER " << this->id_ << " STATS] Loaded " << amount << " Health Points | No need for more Health Points | Current Health: " << this->health_ << endl;
		this->getMode();
	}
	else 
	{
		this->health_ += amount;
		cout << "[PLAYER " << this->id_ << " STATS] Loaded " << amount << " Health Points | Current Health: " << this->health_ << endl;
		this->getMode();
	}
}

void Player::gotHit(int damage)
{
	if (this->health_ - damage < 0)
		this->health_ = 0;
	else
		this->health_ -= damage;
	
	if (this->health_ <= 0)
	{
		cout << "[PLAYER " << this->id_ << " STATUS] Got hit by " << damage << " damage | Player is DEAD"<< endl;
		alive_ = false;
	}
	else
		cout << "[PLAYER " << this->id_ << " STATUS] Got hit by " << damage << " damage | Current Health: " << this->health_ << endl;
}

void Player::move(int x, int y)
{
	this->x_ = x;
	this->y_ = y;
}

void Player::setLastRIndex(int index)
{
	this->last_r_index_ = index;
}
