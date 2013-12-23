/*
 * core.h
 *
 *  Created on: Dec 19, 2013
 *      Author: hp
 */

#ifndef CORE_H_
#define CORE_H_
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <list>

using namespace std;
#define  EMPTY 0 // Empty cells
#define WALL 1	 // Wall cells
#define FOOD 2   // Normal Food Cells
#define SPECIAL 3 //Special Food Cells
#define Snake_Head 4 // Snake head cell
#define Snake_Tail 6 // Snake tail Cell
#define Snake_Body 5 // Snake Body Cells
#define max_value 6 // Implementation details
#define Length 19 // Length of the grid
#define Width 20 // Width of the grid
#define Max_Level 3
#define Left 0 // Left Direction
#define Right 1 //Right Direction
#define UP 2 //Up direction
#define DOWN 3 //Down direction
#define Periodic 2 // Move in the current direction
#define turn_left 0
#define turn_right 1
#define MIN_SIZE 3 //Implementation Details (initial Length of the Snake)
#define FOOD_SCORE 10
#define SPECIAL_SCORE 100
#define SPECIAL_TIMER 10
#define SPECIAL_CONDITION 5

/************************Struct Map ****************************/
class Map {
public:
	int level;//Current Level -->1,2,...,Max_Level
	int grid[Length][Width];
	int score;//Total Score
	int foodEaten;
	bool dead;
	int current_score;//Current Score in the current level , or just use score !!
	list<int> snake, snake_old;//Used to store the snake cells , for movement
	bool error;
	int snake_head;// int value of the snake_head position
	int current_direction;
	bool eating;
	double special_timer;
	int special_x, special_y;
	bool special_exist;

	string error_msg;

	void initialize();//Initialize the game
	int* decrypt(int index);//Convert the 1 int description of a cell to x & y assuming that length > width
	int encrypt(int x, int y);//convert X,Y of a cell to 1 int value , assuming length>width
	void check(int head_x, int head_y, bool check); // Check if cell [head_x][head_y] is has empty slot beside it , if there is no empty slots then the snake dies

	void generate_map();//Read the map from the file
	bool isDead();
	void move(int direction);
	void readFile();
	void printArray();
	bool levelUp();
	bool free(int type) {
		if (type == WALL || type == Snake_Body || type == Snake_Tail)
			return false;
		return true;
	}

	void addFood();//Add normal food , normal food stays in the grid till the snake eats it , when to add the normal food!!
	void addSpecial();//Add special food , special food stays in the grid for certain time (??) , when to add the special food !!!
	void update();
	int* foodPosition();
	bool isFood(int type) {
		if (type == FOOD || type == SPECIAL)
			return true;
		return false;
	}
	bool isFood(int i, int j) {
		return isFood(grid[i][j]);
	}

	bool isSpecial(int i, int j)
	{
		return grid[i][j]==SPECIAL;
	}

	int foodScore(int type) {
		if (type == FOOD)
			return FOOD_SCORE;
		//Special Food Score depends on time
		//TODO
		if (type == SPECIAL)
			return SPECIAL_SCORE * (special_timer);
		return 0;
	}
	double getDistance(int* head, int food_x, int food_y) {
		//TODO
		double s_dist = ((head[0] - food_x) * (head[0] - food_x)) + ((head[1]
				- food_y) * (head[1] - food_y));
		return s_dist;
	}
	void update_Old_List();
	void print_List(list<int> current) {
		list<int>::iterator iter;
		for (iter = current.begin(); iter != current.end(); iter++) {
			int* value = decrypt(*iter);
			cout << "( " << value[0] << " , " << value[1] << " ) ";
		}
		cout << endl;
	}
};
#endif /* CORE_H_ */
