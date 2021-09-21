/*
	Contributors
		- Omer Lev-Ron 204573067
		- Tal Hagag
	A dungeon project as part of the AI Course.
	Game Description
		- There are 2 teams made up of 3 players: 2 warriors and 1 armor bearer
		- Each team goal is to eliminate the other team
		- The players are moving inside a dungeon built from different rooms with transitions between them
		- Health Points and a Ammunition is randomly added to the rooms
		- Each player gets a starting amount of ammunition and health points
	Players Goal:
		- Fight against the opponents of the other team (search for them)
		- Survive, which means to stop fighting if the the health points level is low
		- Look for ammunition if the amount is under a specific threshold
		- Increase health points if necessary (undert a certain threshold)
*/

// System and builtin includes
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <time.h>
#include <math.h>

// Projects Includes
#include "glut.h"
#include "Cell.h"
#include "Room.h"
#include "CompareCells.h"
#include "CompareCellsF.h"
#include "Player.h"
#include "Bullet.h"
#include "Grenade.h"
#include "Definitions.h"
#include "main.h"

using namespace std;


/* Declare global variables */
Cell maze[MSZ][MSZ];	// SPACE is defined 0
double security_map[MSZ][MSZ] = { 0 };	// This is the danger map, the value 0 means there is no danger
Room rooms[NUM_ROOMS]; // An array to hold all the rooms

Bullet* pb = nullptr;
Grenade* pg = nullptr;;

bool run_bfs = false;
const string MAP_KEY_ROOM_TRG = "roomsTrg";
const string MAP_KEY_HIT_TRG = "hit";

int exisiting_rooms = 0;
int team_1_alive_count = 0;
int team_2_alive_count = 0;
int num_players_alive = 1;
int health_points = 10;
int ammunition_points = 10;

queue <Player*> player_steps;
queue <Point2D*> health;
queue <Point2D*> ammunition;

bool defineTeams = false;
bool startPlay = false;
bool resume = false;

Point2D* source = new Point2D(0, 0);


/* Functions Signatures */
double calculateDistance(int x, int y, int xx, int yy);

vector <Cell> gray;
Point2D* getPathToTarget(Point2D* curPos, Point2D* tarPos);
vector<Point2D*> getNeighbors(Point2D* p);

Point2D* EnemysHit(int posTeam, int x1, int y1, int x2, int y2);
vector<Point2D*> getWalkableNeighbors(Point2D* point);

string HitCellsDirection(int target1, int target2, int direction1, int direction2);
Player* SetPlayerLocation(int location1, int location2);

void CreateSecurityMap();
void InitDungeon();
int CalculateDamage(double length);

struct PointsThatCloseToSrc
{
	bool operator ()(Point2D* point1, Point2D* point2)
	{
		return calculateDistance(point1->getCol(), point1->getRow(), source->getCol(), source->getRow()) > calculateDistance(point2->getCol(), point2->getRow(), source->getCol(), source->getRow());
	}
};

void init()
{
	glClearColor(1, 1, 1, 0);// color of window background
	glOrtho(-1, 1, -1, 1, -1, 1);

	srand(time(0));

	InitDungeon();

}

Room InitRoom(int roomIndex)
{
	int i, j;
	int width, height, row, col;
	bool overlap = true;
	Room* room = nullptr;

	do
	{
		overlap = false;
		row = 8 + MIN_ROOM_HEIGHT / 2 + rand() % (MSZ - MIN_ROOM_HEIGHT - 16);
		col = 8 + MIN_ROOM_WIDTH / 2 + rand() % (MSZ - MIN_ROOM_WIDTH - 16);

		height = MIN_ROOM_HEIGHT + rand() % 20;	// in the range 8 to 20
		width = MIN_ROOM_WIDTH + rand() % 20;	// in the range 8 to 20

		room = new Room(row, col, width, height);

		Point2D leftop = room->getLeftTop();
		Point2D rightbottom = room->getRightBottom();

		// to delete ?
		/*if (rightbottom.getCol() == 0 || rightbottom.getRow() == 0 || rightbottom.getRow() >= MSZ - 1 || rightbottom.getCol() >= MSZ - 1 ||
			leftop.getCol() == 0 || leftop.getRow() == 0 || leftop.getRow() >= MSZ - 1 || leftop.getCol() >= MSZ - 1)
			overlap = true;*/

			// Verify the room is not overlapping another exisiting room
		for (j = 0; j < exisiting_rooms && !overlap; j++)
		{
			if (rooms[j].IsOverlap(room))
				overlap = true;
		}
	} while (overlap);

	/* Initalize room - Fill room with spaces */
	for (i = room->getLeftTop().getRow(); i <= room->getRightBottom().getRow(); i++) {
		for (j = room->getLeftTop().getCol(); j <= room->getRightBottom().getCol(); j++) {
			maze[i][j].setIdentity(SPACE);
			maze[i][j].setRoomIndex(roomIndex);
		}
	}
	return *room;
}

void AddCell(int row, int col, Cell* parent, vector<Cell>& grays, vector<Cell>& blacks,
	priority_queue <Cell*, vector<Cell*>, CompareCells>& pq)
{
	Point2D p;
	vector<Cell>::iterator it_gray;
	vector<Cell>::iterator it_black;
	Cell* pc;
	double space = 0;

	p.setRow(row);
	p.setCol(col);

	if (maze[row][col].getIdentity() == WALL)
		space = 3;
	else if (maze[row][col].getIdentity() == SPACE)
		space = 0.1;

	pc = new Cell(p, parent->getTarget(), maze[p.getRow()][p.getCol()].getIdentity(), parent->getG() + space, parent);

	it_black = find(blacks.begin(), blacks.end(), *pc);
	it_gray = find(grays.begin(), grays.end(), *pc);

	if (it_gray == grays.end() && it_black == blacks.end()) // it is not black and not gray!
	{
		pq.push(pc);
		grays.push_back(*pc);
	}
}

void CheckNeighbours(Cell* point_neighbour, vector<Cell>& grays, vector<Cell>& blacks,
	priority_queue <Cell*, vector<Cell*>, CompareCells>& pq)
{
	if (point_neighbour->getPoint().getRow() > 0)
		AddCell(point_neighbour->getPoint().getRow() - 1, point_neighbour->getPoint().getCol(), point_neighbour, grays, blacks, pq);
	if (point_neighbour->getPoint().getRow() < MSZ - 1)
		AddCell(point_neighbour->getPoint().getRow() + 1, point_neighbour->getPoint().getCol(), point_neighbour, grays, blacks, pq);
	if (point_neighbour->getPoint().getCol() < MSZ - 1)
		AddCell(point_neighbour->getPoint().getRow(), point_neighbour->getPoint().getCol() + 1, point_neighbour, grays, blacks, pq);
	if (point_neighbour->getPoint().getCol() > 0)
		AddCell(point_neighbour->getPoint().getRow(), point_neighbour->getPoint().getCol() - 1, point_neighbour, grays, blacks, pq);
}

void DigPath(Cell*& pcurrent, Point2D& start)
{
	while (!(pcurrent->getPoint() == start))
	{
		maze[pcurrent->getPoint().getRow()][pcurrent->getPoint().getCol()].setIdentity(SPACE);
		pcurrent = pcurrent->getParent();
	}
}

/* Create a path from a starting point to a target point using A* Algorithm */
void PaveWay(Point2D start, Point2D target)
{
	vector<Cell> grays;
	vector<Cell> blacks;
	priority_queue <Cell*, vector<Cell*>, CompareCells> pq;

	vector<Cell>::iterator it_gray;		// Iterator for drawing grey

	Cell* start_cell;
	Cell* pcurrent;
	bool go_on = false;

	vector<Cell>::iterator it_black;	// Iterator for drawing black

	double wall_cost = 3, space_cost = 0.1;

	// Starting cell is the center of the room at the start point, target cell is the center of the room at the target
	start_cell = new Cell(start, &target, maze[start.getRow()][start.getCol()].getIdentity(), 0, nullptr);

	pq.push(start_cell);
	grays.push_back(*start_cell);

	while (!pq.empty() && !go_on)
	{
		pcurrent = pq.top();	// Pick the best cell
		pq.pop();		// Remove the cell from pq

		// pcurrent can be the TARGET. This is in case when we didn't stop when we first found the target
		// Remove it from grays and paint it black
		if (!(pcurrent->getPoint() == target))
		{
			it_gray = find(grays.begin(), grays.end(), *pcurrent);
			if (it_gray != grays.end())
				grays.erase(it_gray);
			blacks.push_back(*pcurrent);
			// Now check the neighbors
			CheckNeighbours(pcurrent, grays, blacks, pq);
		}
		else
		{
			go_on = true;
			DigPath(pcurrent, start);
			return;
		}
	}
}

/* Creat paths / transitions between the rooms */
void DigTunnels()
{
	int i, j;

	for (i = 0; i < NUM_ROOMS; i++)
	{
		for (j = i + 1; j < NUM_ROOMS; j++)
		{
			PaveWay(rooms[i].getCenter(), rooms[j].getCenter());
			cout << "[ROOM " << i << "]" << " Creating Path to Room: " << j << endl;
		}
	}
}

void DeployHealthPoints()
{
	int rand_room_num, row, col;
	rand_room_num = rand() % NUM_ROOMS;
	Room rand_room = rooms[rand_room_num];
	Point2D left_top = rand_room.getLeftTop();
	Point2D right_bottom = rand_room.getRightBottom();
	do
	{
		row = rand() % (right_bottom.getRow() - left_top.getRow() + 1) + left_top.getRow();
		col = rand() % (right_bottom.getCol() - left_top.getCol() + 1) + left_top.getCol();
	} while (maze[row][col].getIdentity() != SPACE);
	maze[row][col].setIdentity(HEALTH);
	health.push(new Point2D(row, col));
}

void DeployAmmunition()
{
	int rand_room_num, row, col;
	rand_room_num = rand() % NUM_ROOMS;
	Room rand_room = rooms[rand_room_num];
	Point2D left_top = rand_room.getLeftTop();
	Point2D right_bottom = rand_room.getRightBottom();
	do
	{
		row = rand() % (right_bottom.getRow() - left_top.getRow() + 1) + left_top.getRow();
		col = rand() % (right_bottom.getCol() - left_top.getCol() + 1) + left_top.getCol();
	} while (maze[row][col].getIdentity() != SPACE);
	maze[row][col].setIdentity(AMMUNITION);
	ammunition.push(new Point2D(row, col));
}

void InitDungeon()
{
	int i, j, k;

	// Fill Dugneon with WALLs
	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
			maze[i][j].setIdentity(WALL);

	// Create rooms without overlapping
	for (exisiting_rooms = 0; exisiting_rooms < NUM_ROOMS; exisiting_rooms++)
		rooms[exisiting_rooms] = InitRoom(exisiting_rooms);

	// Randomly add WALLs in the Dungeon. Used as obstacles in the rooms.
	for (k = 0; k < 200; k++)
	{
		i = rand() % MSZ;
		j = rand() % MSZ;
		maze[i][j].setIdentity(WALL);
	}

	DigTunnels();

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
			maze[i][j].setPoint(j, i);

	for (int i = 0; i < ammunition_points; i++)
		DeployAmmunition();

	for (int i = 0; i < health_points; i++)
		DeployHealthPoints();
}

Point2D* Astar(Point2D* source, map<string, string> target_search, bool(*target)(Point2D* current, Point2D* source, map<string, string> args))
{
	Cell* current;
	queue<Cell*> q;
	Point2D* target_location = new Point2D(-1, -1);
	priority_queue <Cell*, vector<Cell*>, CompareCellsF> pq;
	bool is_found = false;

	pq.push(new Cell(maze[source->getRow()][source->getCol()]));

	while (!pq.empty() && !is_found)
	{
		current = pq.top();
		pq.pop();

		if (maze[current->getPoint().getRow()][current->getPoint().getCol()].getSearchIdentity() == BLACK)
		{
			continue;
		}
		maze[current->getPoint().getRow()][current->getPoint().getCol()].setSearchIdentity(BLACK);
		q.push(current);
		if (target(new Point2D(current->getPoint()), source, target_search))
		{
			is_found = true;
			target_location = new Point2D(current->getPoint());
			while (!pq.empty())
			{
				Cell* t = pq.top();
				maze[t->getPoint().getRow()][t->getPoint().getCol()].setSearchIdentity(SPACE);
				maze[t->getPoint().getRow()][t->getPoint().getCol()].setFVal(0);
				pq.pop();
			}
		}
		else {
			for each (Point2D * point in getNeighbors(&current->getPoint()))
			{
				double tmp = maze[current->getPoint().getRow()][current->getPoint().getCol()].getFVal() + 1;
				if (maze[point->getRow()][point->getCol()].getSearchIdentity() == BLACK)
					continue;
				if (maze[point->getRow()][point->getCol()].getSearchIdentity() == SPACE)
				{
					maze[point->getRow()][point->getCol()].setFVal(tmp);
				}
				else {
					if (tmp < maze[point->getRow()][point->getCol()].getFVal())
					{
						maze[point->getRow()][point->getCol()].setFVal(tmp);
					}
				}
				maze[point->getRow()][point->getCol()].setSearchIdentity(GRAY);
				pq.push(new Cell(maze[point->getRow()][point->getCol()]));
			}
		}
	}
	while (!q.empty())
	{
		Cell* t = q.front();
		maze[t->getPoint().getRow()][t->getPoint().getCol()].setSearchIdentity(SPACE);
		maze[t->getPoint().getRow()][t->getPoint().getCol()].setFVal(0);
		q.pop();
	}
	return target_location;
}

Point2D* MoveToTarget(Point2D* source, Point2D* target, double(*costFunction)(int x, int y))
{
	int row = source->getRow();
	int col = source->getCol();
	Point2D* next = new Point2D(-1, -1);
	int count = 0;

	if (source->equals(target))
	{
		return next;
	}

	Cell* current;
	priority_queue <Cell*, vector<Cell*>, CompareCellsF> pq;
	queue<Cell*> q;

	pq.push(new Cell(maze[row][col]));
	bool is_found = false;
	while (!pq.empty() && !is_found)
	{
		current = pq.top();
		count++;
		pq.pop();
		if (current->getPoint().equals(target))
		{
			next = getPathToTarget(new Point2D(current->getPoint()), source);
			while (!pq.empty())
			{
				Cell* t = pq.top();
				maze[t->getPoint().getRow()][t->getPoint().getCol()].setParent(NULL);
				maze[t->getPoint().getRow()][t->getPoint().getCol()].setFVal(0);
				maze[t->getPoint().getRow()][t->getPoint().getCol()].setSearchIdentity(SPACE);
				pq.pop();
			}
			is_found = true;
		}
		maze[current->getPoint().getRow()][current->getPoint().getCol()].setSearchIdentity(BLACK);
		q.push(current);
		if (!is_found) {
			for each (Point2D * neigbor in getNeighbors(&current->getPoint()))
			{
				if (maze[neigbor->getRow()][neigbor->getCol()].getSearchIdentity() == SPACE)
				{
					double distance = calculateDistance(neigbor->getCol(), neigbor->getRow(), target->getCol(), target->getRow());
					maze[neigbor->getRow()][neigbor->getCol()].setFVal(distance + costFunction(neigbor->getCol(), neigbor->getRow()));
					maze[neigbor->getRow()][neigbor->getCol()].setSearchIdentity(GRAY);
					maze[neigbor->getRow()][neigbor->getCol()].setParent(current);
					pq.push(new Cell(&maze[neigbor->getRow()][neigbor->getCol()]));

				}
			}
		}
	}
	while (!q.empty())
	{
		Cell* t = q.front();
		maze[t->getPoint().getRow()][t->getPoint().getCol()].setFVal(0);
		maze[t->getPoint().getRow()][t->getPoint().getCol()].setParent(NULL);
		maze[t->getPoint().getRow()][t->getPoint().getCol()].setSearchIdentity(SPACE);
		q.pop();
	}
	return next;
}

double calculateDistance(int x, int y, int xx, int yy)
{
	return sqrt(pow(xx - x, 2) + pow(yy - y, 2));
}

/* The functions gets a point and returns a vector with the neighbors there are not WALL */
vector<Point2D*> getNeighbors(Point2D* p)
{
	int row = p->getRow();
	int col = p->getCol();
	vector<Point2D*> neigbor;

	if (maze[row][col + 1].getIdentity() != WALL)	// Check the cell to the right
		neigbor.push_back(new Point2D(col + 1, row));
	if (maze[row][col - 1].getIdentity() != WALL)	// Check the cell to the left
		neigbor.push_back(new Point2D(col - 1, row));
	if (maze[row + 1][col].getIdentity() != WALL)	// Check the cell above
		neigbor.push_back(new Point2D(col, row + 1));
	if (maze[row - 1][col].getIdentity() != WALL)	// Check the cell below
		neigbor.push_back(new Point2D(col, row - 1));

	return neigbor;
}

/* The functions gets a point and returns a vector with the neighbors there are not WALL and a player */
vector<Point2D*> getWalkableNeighbors(Point2D* p)
{
	int row = p->getRow();
	int col = p->getCol();
	vector<Point2D*> neigbor;

	if (maze[row][col + 1].Walkable())
		neigbor.push_back(new Point2D(col + 1, row));
	if (maze[row][col - 1].Walkable())
		neigbor.push_back(new Point2D(col - 1, row));
	if (maze[row + 1][col].Walkable())
		neigbor.push_back(new Point2D(col, row + 1));
	if (maze[row - 1][col].Walkable())
		neigbor.push_back(new Point2D(col, row - 1));

	return neigbor;
}

void deletePoint(Point2D* point)
{
	int row = point->getRow();
	int col = point->getCol();
	bool temp = false;
	queue <Point2D*> tempV;
	while (!ammunition.empty())
	{
		if (!ammunition.front()->equals(point))
			tempV.push(ammunition.front());
		ammunition.pop();
	}
	while (!tempV.empty())
	{
		ammunition.push(tempV.front());
		tempV.pop();
	}
	maze[row][col].setIdentity(SPACE);
} 

void deleteHealthPoint(Point2D* point)
{
	int row = point->getRow();
	int col = point->getCol();
	bool temp = false;
	queue <Point2D*> tempV;
	while (!health.empty())
	{
		if (!health.front()->equals(point))
			tempV.push(health.front());
		health.pop();
	}

	while (!tempV.empty())
	{
		health.push(tempV.front());
		tempV.pop();
	}
	maze[row][col].setIdentity(SPACE);
}

/* This functions add a player to the game and randomly initliaze its health and ammunition threshold and amounts */
void AddPlayer(int room_index, int identity, bool can_attack)
{
	Room room = rooms[room_index];
	int row = room.getRightBottom().getCol();
	int col = room.getRightBottom().getRow();
	while (maze[col][row].getIdentity() != SPACE)
	{
		row--;
		col--;
	}
	maze[col][row].setIdentity(identity);
	Player* p = nullptr;
	int ammu = rand() % 5 + 1;	// Random ammunition amount in range 1 to 5
	int high_health = rand() % 50 + 30; // Random high health threshold in range from 30 to 50
	int low_health = high_health - (rand() % 25 + 5); // Random low health threshold in range 5 to 25
	p = new Player(identity, row, col, num_players_alive, maze[col][row].getRoomIndex(), low_health, high_health, ammu, can_attack);
	num_players_alive++;
	player_steps.push(p);

}

/* Build the path to the target cell */
Point2D* getPathToTarget(Point2D* current, Point2D* source)
{
	if (current == source)
		return new Point2D(-1, -1);

	Point2D* position = new Point2D(current);
	Point2D* LPosition;
	bool is_found = false;

	do
	{
		LPosition = new Point2D(position);
		position = new Point2D(maze[position->getRow()][position->getCol()].getParent()->getPoint());

		if (position->equals(source))
			is_found = true;
	} while (!is_found);

	return LPosition;
}

void DrawMaze()
{
	int i, j;
	double xsz = WIDTH / MSZ;
	double ysz = HEIGHT / MSZ;
	double x, y;

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			switch (maze[i][j].getIdentity()) // Set Cell color
			{
			case SPACE:
				// glColor3d(0.9, 0.9, 0.9);
				// assumption: the value of security_map[i][j] is in range (0,1)
				glColor3d(1 - security_map[i][j], 1 - security_map[i][j], 1 - security_map[i][j]);
				break;
			case WALL:
				glColor3d(0.3, 0, 0);	// Dark Red
				break;
			case START:
				glColor3d(0.5, 0.5, 1);		// Cyan
				break;
			case TARGET:
				glColor3d(1, 0, 0);		// Red
				break;
			case TEAM_1:
				glColor3d(0, 1, 1);		// Cyan
				break;
			case TEAM_2:
				glColor3d(1, 0.5, 0);	// Orange
				break;
			case HEALTH:
				glColor3d(0.0, 0.0, 0.9);	// Blue
				break;
			case AMMUNITION:
				glColor3d(0.0, 0.7, 0.2);	// Green
				break;
			}

			// draw square
			x = 2.0 * (j * xsz) / WIDTH - 1; // value in range [-1,1)
			y = 2.0 * (i * ysz) / HEIGHT - 1; // value in range [-1,1)
			glBegin(GL_POLYGON);
			glVertex2d(x, y);
			glVertex2d(x, y + (2.0 * ysz) / HEIGHT);
			glVertex2d(x + 2.0 * xsz / WIDTH, y + (2.0 * ysz) / HEIGHT);
			glVertex2d(x + 2.0 * xsz / WIDTH, y);
			glEnd();
		}
}

void CreateSecurityMap()
{
	int num_simulations = 5000;
	double one_bullet_hurt = 1.0 / num_simulations;
	double x, y;
	Grenade* pg;

	for (int i = 1; i <= num_simulations; i++)
	{
		x = -1 + 2 * (rand() % 100) / 100.0;
		y = -1 + 2 * (rand() % 100) / 100.0;
		pg = new Grenade(x, y);
		pg->SimulateExplosion(maze, security_map, one_bullet_hurt);
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	DrawMaze();

	//if (pb != nullptr)
	//	pb->showMe();

	if (pg)
		pg->ShowMe();


	glutSwapBuffers();	// show all
}

bool findSpaces(double x, double y)
{
	int row, col;
	row = MSZ * (x + 1) / 2;
	col = MSZ * (y + 1) / 2;
	return  maze[row][col].getIdentity() == SPACE;
}

double AttackCost(int x, int y)
{
	return (security_map[x][y] + 3);
}

double HealthCost(int x, int y)
{
	int temp;
	temp = (maze[x][y].getIdentity() == HEALTH) ? 1 : 0;
	return (security_map[x][y] + 1) + temp * (-1.5);
}

double AmmunitionCost(int x, int y)
{
	int temp;
	temp = (maze[x][y].getIdentity() == AMMUNITION) ? 1 : 0;
	return (security_map[x][y] + 1) + temp * (-1.5);
}

double AssistanceCost(int x, int y)
{
	int temp;
	temp = (maze[x][y].getIdentity() == ASSIST) ? 1 : 0;
	return (security_map[x][y] + 1) + temp * (-0.5);
}

double AmmunitionHealthCost(int x, int y)
{
	int tempA;
	int tempH;
	tempA = (maze[x][y].getIdentity() == AMMUNITION) ? 1 : 0;
	tempH = (maze[x][y].getIdentity() == HEALTH) ? 1 : 0;
	return (security_map[x][y] + 3) + tempH * (-1.5) + tempA * (-1.5);
}

/* This functions gets a string and a delimeter and returns a vector containing strings splitted by the delimeter */
std::vector<std::string> SplitString(std::string strToSplit, char delimeter)
{
	std::stringstream ss(strToSplit);
	std::string item;
	std::vector<std::string> str;
	while (std::getline(ss, item, delimeter))
	{
		str.push_back(item);
	}
	return str;
}


string GetPlayerHitCells(Point2D* p)
{
	string space = "";
	int i = p->getRow();
	int j = p->getCol();
	for (int row = -1; row <= 1; row++)
	{
		for (int col = -1; col <= 1; col++)
		{
			if (row == 0 && col == 0)
				continue;
			space += HitCellsDirection(i, j, row, col);
		}
	}
	return space;
}

string HitCellsDirection(int x1, int y1, int x2, int y2)
{
	string space = "";
	int x = x1 + x2;
	int y = y1 + y2;
	while (maze[x][y].getRoomIndex() == maze[x1][y1].getRoomIndex() && maze[x][y].getIdentity() == SPACE)
	{
		space += maze[x][y].getPoint().toString() + ",";
		x += x2;
		y += y2;
	}
	return space;
}

/* The function checks if it is on target map */
bool IsCellToShoot(Point2D* current, Point2D* source, map<string, string> args)
{
	string space = args[MAP_KEY_HIT_TRG];
	size_t found = space.find(current->toString());
	return found != string::npos;
}

/* The function checks if the cell is a ammunition or health cell, if not, then if it is on target map */
bool IsCellToShootOrAmmuHealth(Point2D* current, Point2D* source, map<string, string> args)
{
	int i = current->getRow();
	int j = current->getCol();
	if (maze[i][j].getIdentity() == AMMUNITION || maze[i][j].getIdentity() == HEALTH)
		return true;
	string spots = args[MAP_KEY_HIT_TRG];
	size_t found = spots.find(current->toString());
	return found != string::npos;
}

/* The function checks if the cell is a ammunition cell, if not, then if it is on target map */
bool IsCellToShootOrAmmu(Point2D* current, Point2D* source, map<string, string> args)
{
	int i = current->getRow();
	int j = current->getCol();
	if (maze[i][j].getIdentity() == AMMUNITION)
		return true;
	string space = args[MAP_KEY_HIT_TRG];
	size_t found = space.find(current->toString());
	return found != string::npos;
}

/* The function checks if the cell is a health cell, if not, then if it is on target map */
bool IsCellToShootOrHealth(Point2D* current, Point2D* source, map<string, string> args)
{
	int i = current->getRow();
	int j = current->getCol();
	if (maze[i][j].getIdentity() == HEALTH)
		return true;
	string spots = args[MAP_KEY_HIT_TRG];
	size_t found = spots.find(current->toString());
	return found != string::npos;
}

/* The function checks if the cell is an attack target */
bool IsAttackTargetCell(Point2D* current, Point2D* source, map<string, string> args)
{
	string temp;
	vector <string> vec;
	int i = current->getRow();
	int j = current->getCol();
	vec = SplitString(args[MAP_KEY_ROOM_TRG], ',');
	for each (string v in vec)
	{
		temp = std::to_string(maze[i][j].getRoomIndex());
		if (temp.compare(v) == 0) {
			return true;
		}
	}
	return false;
}


bool IsAttackTargetCellHealthAmmunition(Point2D* current, Point2D* source, map<string, string> args)
{
	string temp;
	vector <string> vec;
	int i = current->getRow();
	int j = current->getCol();

	if (maze[i][j].getIdentity() == HEALTH || maze[i][j].getIdentity() == AMMUNITION)
		return true;
	vec = SplitString(args[MAP_KEY_ROOM_TRG], ',');
	for each (string v in vec)
	{
		temp = std::to_string(maze[i][j].getRoomIndex());
		if (temp.compare(v) == 0) {
			return true;
		}
	}
	return false;
}

bool IsAttackTargetCellAmmunition(Point2D* current, Point2D* source, map<string, string> args)
{
	string temp;
	vector <string> vec;
	int i = current->getRow();
	int j = current->getCol();

	if (maze[i][j].getIdentity() == AMMUNITION)
		return true;
	vec = SplitString(args[MAP_KEY_ROOM_TRG], ',');
	for each (string v in vec)
	{
		temp = std::to_string(maze[i][j].getRoomIndex());
		if (temp.compare(v) == 0) {
			return true;
		}
	}
	return false;
}

bool IsAttackTargetCellHealth(Point2D* current, Point2D* source, map<string, string> args)
{
	string temp;
	vector <string> tokens;
	int i = current->getRow();
	int j = current->getCol();
	if (maze[i][j].getIdentity() == HEALTH)
		return true;
	tokens = SplitString(args[MAP_KEY_ROOM_TRG], ',');
	for each (string token in tokens)
	{
		temp = std::to_string(maze[i][j].getRoomIndex());
		if (temp.compare(token) == 0) {
			return true;
		}
	}
	return false;
}

bool IsSurvivorInSameCell(Point2D* current, Point2D* source, map<string, string> args)
{
	if (maze[current->getRow()][current->getCol()].getIdentity() == HEALTH)
		return true;
	if (maze[current->getRow()][current->getCol()].getRoomIndex() == maze[source->getRow()][source->getCol()].getRoomIndex())
		return false;
	return false;
}

bool IsSurvivorNotInSameCell(Point2D* current, Point2D* source, map<string, string> args)
{
	if (maze[current->getRow()][current->getCol()].getIdentity() == HEALTH)
		return true;
	return false;
}


/* Get a vector of the alive enemies */
vector<Player*> GetEnemies(Player* p)
{
	vector<Player*> enemies;
	for (int i = 0; i < player_steps.size(); i++)
	{
		Player* pTemp = player_steps.front();
		if (pTemp->isAlive() && pTemp->getTeam() != p->getTeam()) {
			enemies.push_back(pTemp);
		}
		player_steps.pop();
		player_steps.push(pTemp);
	}
	return enemies;
}

/* Get a vector of the alive team players */
vector<Player*> GetFriends(Player* p)
{
	vector<Player*> friends;
	for (int i = 0; i < player_steps.size(); i++)
	{
		Player* pTemp = player_steps.front();
		if (pTemp->isAlive() && pTemp->getTeam() == p->getTeam()) {
			friends.push_back(pTemp);
		}
		player_steps.pop();
		player_steps.push(pTemp);
	}
	return friends;
}

bool IsNeedAmmunition(Point2D* current, Point2D* source, map<string, string> args)
{
	if (maze[current->getRow()][current->getCol()].getIdentity() == AMMUNITION)
		return true;
	return false;
}

bool IsNeedAssistance(Point2D* current, Point2D* source, map<string, string> args)
{
	if (maze[current->getRow()][current->getCol()].getIdentity() == ASSIST)
		return true;
	return false;
}


bool IsEnemyInTheSameRoom(Player* p)
{
	for each (auto enemy in GetEnemies(p))
	{
		int x_enemy = enemy->getX();
		int y_enemy = enemy->getY();
		int x_player = p->getX();
		int y_player = p->getY();
		if (maze[y_enemy][x_enemy].getRoomIndex() == maze[y_player][x_player].getRoomIndex())
			return true;
	}
	return false;
}

bool IsFriendInTheSameRoom(Player* p)
{
	for each (auto f_player in GetFriends(p))
	{
		int x_friend = f_player->getX();
		int y_friend = f_player->getY();
		int x_player = p->getX();
		int y_player = p->getY();
		if (maze[y_friend][x_friend].getRoomIndex() == maze[y_player][x_player].getRoomIndex())
			return true;
	}
	return false;
}

queue<Point2D*> FriendsInSameRoom(Player* p)
{
	source->~Point2D();
	int x_player = p->getX();
	int y_player = p->getY();
	source = new Point2D(x_player, y_player);
	priority_queue <Point2D*, vector<Point2D*>, PointsThatCloseToSrc> pq;
	queue<Point2D*> result;

	for each (Player * f_player in GetFriends(p))
	{
		int x_friend = f_player->getX();
		int y_friend = f_player->getY();

		if (maze[y_friend][x_friend].getRoomIndex() == maze[y_player][x_player].getRoomIndex())
		{
			pq.push(new Point2D(x_friend, y_friend));
		}
	}
	while (!pq.empty())
	{
		result.push(pq.top());
		pq.pop();
	}
	return result;
}

queue<Point2D*> EnemiesInSameRoom(Player* p)
{
	source->~Point2D();
	int x_player = p->getX();
	int y_player = p->getY();
	source = new Point2D(x_player, y_player);
	priority_queue <Point2D*, vector<Point2D*>, PointsThatCloseToSrc> pq;
	queue<Point2D*> result;

	for each (Player * enemy in GetEnemies(p))
	{
		int x_enemy = enemy->getX();
		int y_enemy = enemy->getY();

		if (maze[y_enemy][x_enemy].getRoomIndex() == maze[y_player][x_player].getRoomIndex())
		{
			pq.push(new Point2D(x_enemy, y_enemy));
		}
	}
	while (!pq.empty())
	{
		result.push(pq.top());
		pq.pop();
	}
	return result;
}

bool HitEnemy(Player* p, int& team_players_alive, double thresholdLength)
{
	int row, col;
	priority_queue <Point2D*, vector<Point2D*>, PointsThatCloseToSrc> pq;
	source->~Point2D();
	source = new Point2D(p->getX(), p->getY());
	for (row = -1; row <= 1; row++)
	{
		for (col = -1; col <= 1; col++)
		{
			Point2D* el = EnemysHit(p->getTeam(), p->getX(), p->getY(), row, col);
			if (el->getRow() != -1) {
				pq.push(el);
			}
		}
	}
	if (pq.size() == 0) {
		return false;
	}
	double length = calculateDistance(pq.top()->getCol(), pq.top()->getRow(), p->getX(), p->getY());
	if (length <= thresholdLength)
	{
		Player* enemy = SetPlayerLocation(pq.top()->getCol(), pq.top()->getRow());
		enemy->gotHit(CalculateDamage(length));
		p->hit();
		cout << "[PLAYER " << p->getId() << "] Hit Enemy: " << enemy->getId() << endl;

		if (!enemy->isAlive()) {
			maze[enemy->getY()][enemy->getX()].setIdentity(SPACE);
			(team_players_alive)--;
			cout << "[PLAYER " << enemy->getId() << "] Player is DEAD!" << endl;
			cout << "[GAME STATUS] Team A | Players Alive: " << team_1_alive_count << endl;
			cout << "[GAME STATUS] Team B | Players Alive: " << team_2_alive_count << endl;
			if (team_players_alive == 0)
			{
				cout << "[GAME STATUS] Team " << ((p->getTeam() == 12) ? 'A' : 'B') << " Wins!!!" << endl;
				startPlay = false;
			}
		}
		return true;
	}
	return false;
}

Player* SetPlayerLocation(int x, int y)
{
	Player* p = new Player();
	for (int i = 0; i < player_steps.size(); i++)
	{
		Player* temp = player_steps.front();
		if (temp->getX() == x && temp->getY() == y)
			p = temp;
		player_steps.pop();
		player_steps.push(temp);
	}
	return p;
}

Point2D* EnemysHit(int team, int x_player, int y_player, int x, int y)
{
	Point2D* enemy_location = new Point2D(-1, -1);
	x_player += x;
	y_player += y;
	while (maze[y_player][x_player].getIdentity() == SPACE)
	{
		x_player += x;
		y_player += y;
	}
	if (maze[y_player][x_player].getIdentity() == TEAM_1 + TEAM_2 - team)
	{
		enemy_location->setCol(x_player);
		enemy_location->setRow(y_player);
	}
	return enemy_location;
}

void RunGame()
{
	Player* player = player_steps.front();
	bool(*trgOfEnemyNSameRoom)(Point2D * current, Point2D * source, map<string, string> args);
	bool(*trgOfEnemySameRoom)(Point2D * current, Point2D * src, map<string, string> args);

	double(*path_function)(int x, int y);
	if (player->getMode() == ATTACK && player->canAttack()) {
		trgOfEnemyNSameRoom = IsAttackTargetCell;
		trgOfEnemySameRoom = IsCellToShoot;
		path_function = AttackCost;		// Attack 
	}
	else if (player->getMode() == ATTACK_AND_HEALTH) {
		trgOfEnemySameRoom = IsCellToShootOrHealth;
		trgOfEnemyNSameRoom = IsAttackTargetCellHealth;
		path_function = AmmunitionHealthCost;	//Attack & Health 
	}
	else if (player->getMode() == ATTACK_HEALTH_AMMU) {
		trgOfEnemySameRoom = IsCellToShootOrAmmuHealth;
		trgOfEnemyNSameRoom = IsAttackTargetCellHealthAmmunition;
		path_function = AmmunitionHealthCost;	// Attack & Health & Ammou
	}
	else if (player->getMode() == NEED_AMMUNITION) {
		trgOfEnemySameRoom = IsNeedAmmunition;
		trgOfEnemyNSameRoom = IsNeedAmmunition;
		path_function = AmmunitionCost;	//Ammou
	}
	else if (player->getMode() == ATTACK_AND_AMMU) {
		trgOfEnemySameRoom = IsCellToShootOrAmmu;
		trgOfEnemyNSameRoom = IsAttackTargetCellAmmunition;
		path_function = AmmunitionHealthCost;	// Attack & Health & Ammou

	}
	else if (player->getMode() == ASSISTANCE) {
		trgOfEnemySameRoom = IsNeedAssistance;
		trgOfEnemyNSameRoom = IsNeedAssistance;;
		path_function = AmmunitionCost;	//Ammou
	}
	else {
		trgOfEnemySameRoom = IsSurvivorInSameCell;
		trgOfEnemyNSameRoom = IsSurvivorNotInSameCell;
		path_function = HealthCost;
	}
	bool is_enemy_in_room = false, is_friends_in_room = false;;
	player_steps.pop();
	if (!player->isAlive()) {
		return;
	}
	bool hit = false;
	is_enemy_in_room = IsEnemyInTheSameRoom(player);
	is_friends_in_room = IsFriendInTheSameRoom(player);
	if (player->hasAmmunition() && is_enemy_in_room && player->canAttack())
	{
		hit = HitEnemy(player, (player->getTeam() == TEAM_1) ? team_2_alive_count : team_1_alive_count, rand() % (30 - 5 + 1) + 5);
	}
	if (hit)
	{
		player_steps.push(player);
		return;
	}
	map<string, string> target_params;
	Point2D* target;
	target_params[MAP_KEY_ROOM_TRG] = "";
	if (is_friends_in_room && !player->canAttack())
	{
		queue<Point2D*> friends = FriendsInSameRoom(player);	// Get friends queue
		Point2D* friend_point;	// Point of friend
		target_params[MAP_KEY_HIT_TRG] = "";
		string space;
		while (!friends.empty())
		{
			friend_point = friends.front();
			friends.pop();
			space = GetPlayerHitCells(friend_point);
			target_params[MAP_KEY_HIT_TRG] += space;
		}

		target = Astar(new Point2D(player->getX(), player->getY()), target_params, trgOfEnemySameRoom);
	}
	else if (is_enemy_in_room) {
		queue<Point2D*> enemies = EnemiesInSameRoom(player);	// Get enemies queue
		Point2D* enemy_point;	// Point of enemy
		target_params[MAP_KEY_HIT_TRG] = "";
		string space;
		while (!enemies.empty())
		{
			enemy_point = enemies.front();
			enemies.pop();
			space = GetPlayerHitCells(enemy_point);
			target_params[MAP_KEY_HIT_TRG] += space;
		}
		target = Astar(new Point2D(player->getX(), player->getY()), target_params, trgOfEnemySameRoom);
	}
	else {
		vector<Player*> e = GetEnemies(player);
		for each (Player * enemy in e)
		{
			int trgRoomIndex = maze[enemy->getY()][enemy->getX()].getRoomIndex();
			if (trgRoomIndex == -1) {
				target_params[MAP_KEY_ROOM_TRG] += std::to_string(enemy->getLastRIndex()) + ",";
			}
			else {
				target_params[MAP_KEY_ROOM_TRG] += std::to_string(trgRoomIndex) + ",";
			}
		}
		target = Astar(new Point2D(player->getX(), player->getY()), target_params, trgOfEnemyNSameRoom);
	}
	Point2D* nextStep = MoveToTarget(new Point2D(player->getX(), player->getY()), new Point2D(target), path_function);
	if (nextStep->getRow() != -1)
	{
		if (maze[nextStep->getRow()][nextStep->getCol()].getIdentity() == TEAM_1 || maze[nextStep->getRow()][nextStep->getCol()].getIdentity() == TEAM_2)
		{
			vector<Point2D*> np = getWalkableNeighbors(new Point2D(player->getX(), player->getY()));
			if (np.size() > 0)
			{
				priority_queue <Cell*, vector<Cell*>, CompareCellsF> pq;
				for each (Point2D * step in np)
				{
					maze[step->getRow()][step->getCol()].setFVal(maze[step->getRow()][step->getCol()].getDanger());
					pq.push(new Cell(maze[step->getRow()][step->getCol()]));
				}
				nextStep = new Point2D(pq.top()->getPoint());
				while (!pq.empty())
				{
					maze[pq.top()->getPoint().getRow()][pq.top()->getPoint().getCol()].setFVal(0);
					pq.pop();
				}
			}
		}
		if (maze[nextStep->getRow()][nextStep->getCol()].getIdentity() == AMMUNITION)
		{
			DeployAmmunition();
			deletePoint(new Point2D(nextStep->getCol(), nextStep->getRow()));
			player->loadAmmu(rand() % 3 + 1);
		}
		if (maze[nextStep->getRow()][nextStep->getCol()].getIdentity() == HEALTH)
		{
			DeployHealthPoints();
			deleteHealthPoint(new Point2D(nextStep->getCol(), nextStep->getRow()));
			player->loadHealth(rand() % 100 + 1);
		}
		if (maze[nextStep->getRow()][nextStep->getCol()].getIdentity() == SPACE)
		{
			if (maze[player->getY()][player->getX()].getRoomIndex() != -1)
			{
				player->setLastRIndex(maze[player->getY()][player->getX()].getRoomIndex());
			}
			maze[player->getY()][player->getX()].setIdentity(SPACE);
			player->move(nextStep->getCol(), nextStep->getRow());
			maze[nextStep->getRow()][nextStep->getCol()].setIdentity(player->getTeam());
		}
	}
	player_steps.push(player);
}

int CalculateDamage(double length)
{
	return (30 / length) * (rand() % 10);
}

void Idle()
{
	if (resume && pg != nullptr)
	{
		pg->Exploding(maze);
	}
	if (startPlay) {
		RunGame();
	}

	glutPostRedisplay(); // go to disply function
}

void Menu(int choice)
{
	switch (choice)
	{
	case 1:
		resume = false;
		CreateSecurityMap();
		break;
	case 2:
		pg->Explode();
		resume = true;
		break;
	case 3:
		if (!defineTeams)
		{
			for (int i = 0; i < PLAYERS; i++)
			{
				if (i == PLAYERS - 1)
					AddPlayer(0, TEAM_1, false);
				else
					AddPlayer(0, TEAM_1, true);
				team_1_alive_count++;
			}
			for (int i = 0; i < PLAYERS; i++)
			{
				if (i == PLAYERS - 1)
					AddPlayer(1, TEAM_2, false);
				else
					AddPlayer(1, TEAM_2, true);
				team_2_alive_count++;
			}
			defineTeams = true;
		}
		break;
	case 4:
		if (defineTeams && !startPlay)
		{
			startPlay = true;
		}
		break;
	}
}

void mouse(int button, int state, int x, int y)
{

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		double xx, yy;
		xx = 2 * ((double)x) / WIDTH - 1;
		yy = 2 * ((HEIGHT - (double)y)) / HEIGHT - 1;
		//	pb = new Bullet(xx, yy); // we shall need a bullet to fire it from riffle (only wall or enemy body will stop it)
		pg = new Grenade(xx, yy); // in the game the granade's bullets are flying to the limited distance
	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("Dungeon Game");

	glutDisplayFunc(display);
	glutIdleFunc(Idle);

	glutMouseFunc(mouse);

	// menu
	glutCreateMenu(Menu);
	glutAddMenuEntry("Create Security Map", 1);
	glutAddMenuEntry("Throw grenade", 2);
	glutAddMenuEntry("Setup Teams", 3);
	glutAddMenuEntry("Play", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop();
}