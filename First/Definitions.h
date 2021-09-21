#pragma once

const int MSZ = 100;
const double SPEED = 0.001;

const int SPACE = 0;
const int WALL = 1;
const int START = 2;
const int TARGET = 3;
const int PATH = 4; // belongs to the path to target
const int GRAY = 5; // Fringe
const int BLACK = 6; // VISITED
const int HEALTH = 10;
const int AMMUNITION = 11;
const char TEAM_1 = 12;
const char TEAM_2 = 13;
const int ASSIST = 14;

const int MIN_ROOM_WIDTH = 8;
const int MIN_ROOM_HEIGHT = 8;
const int MAX_ASSIST_DIST = 2;

const int WIDTH = 800;	// Windows Width
const int HEIGHT = 800;	// Window Height

const int NUM_ROOMS = 12;	// The number of rooms in the dungeon
const int PLAYERS = 3;	// The number of players in a team