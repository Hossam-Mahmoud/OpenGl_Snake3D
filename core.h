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
#include <vector>
#include <fstream>
#include <string.h>

using namespace std;
#define  EMPTY 0 // Empty cells
#define WALL 1	 // Wall cells
#define FOOD 2   // Normal Food Cells
#define SPECIAL 3 //Special Food Cells
#define Snake_Head 4 // Snake head cell
#define Snake_Tail 6 // Snake tail Cell
#define Snake_Body 5 // Snake Body Cells
#define max_value 6 // Implementation details
#define Length 3 // Length of the grid
#define Width 3 // Width of the grid
#define Max_Level 3
#define Left 1 // Left Direction
#define Right 2 //Right Direction
#define Periodic 3 // Move in the current direction
#define UP 4 //Up direction
#define DOWN 5 //Down direction
#define MIN_SIZE 3 //Implementation Details (initial Length of the Snake)
string separator_line =
		"********************************************************";
string files[] = { "level1.txt", "level2.txt", "level3.txt" };
/************************Struct Map ****************************/
struct Map {
	int level;//Current Level -->1,2,...,Max_Level
	int grid[Length][Width];
	int score;//Total Score
	bool dead;
	int current_score;//Current Score in the current level , or just use score !!
	vector<int> snake;//Used to store the snake cells , for movement
	bool error;
	int snake_head;// int value of the snake_head position
	int current_direction;
	bool eating;

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
		if (type == WALL || type == FOOD || type == EMPTY || type == SPECIAL)
			return true;
		return false;
	}

	void addFood();//Add normal food , normal food stays in the grid till the snake eats it , when to add the normal food!!
	void addSpecial();//Add special food , special food stays in the grid for certain time (??) , when to add the special food !!!
};
#endif /* CORE_H_ */
