/*
 * Core.cpp
 *
 *  Created on: Dec 19, 2013
 *      Author: Zonkoly
 */
#include "core.h"
/*
 *
 */
void Map::initialize() {
	//Initialize the Game
	// set level to 1
	level = 1;

	score = 0;
	dead = false;
	current_score = 0;
	error = false;
	foodEaten = 0;

	snake = queue<int> ();
	special_timer = -1;
	special_exist = false;
	special_x = 0;
	special_y = 0;
	generate_map();
	if (!error)
		addFood();
}
int* Map::decrypt(int index) {

	int* result = new int[2];
	result[0] = index / Length;
	result[1] = index % Length;

	return result;
}

int Map::encrypt(int x, int y) {
	return ((x * Length) + y);
}

void Map::generate_map() {
	//TODO
	for (int i = 0; i < Length; i++)
		for (int j = 0; j < Width; j++)
			grid[i][j] = 0;
	readFile();
	if (error)
		return;
	// After Reading the file , check if the Snake is in correct describtion
	int head_count = 0, tail_count = 0, body_count = 0;
	int head_x = -1, head_y = -1, body_x = -1, body_y = -1;
	int tail_x = -1, tail_y = -1;
	for (int i = 0; i < Length; i++) {
		for (int j = 0; j < Width; j++) {
			switch (grid[i][j]) {
			case Snake_Head:
				head_count++;
				head_x = i;
				head_y = j;
				break;
			case Snake_Tail:
				tail_count++;
				tail_x = i;
				tail_y = j;
				break;
			case Snake_Body:
				body_count++;
				body_x = i;
				body_y = j;
			default:
				break;
			}
		}
	}

	if (head_count != 1 || tail_count != 1 || (head_count + tail_count
			+ body_count != MIN_SIZE)) {
		error = true;
		error_msg
				= "Snake is not drawn correctly , number of points is not equal to the initial size of the snak or has 0 or more than 1 head or tail";
		return;
	}

	//Assuming that the initial size is 3
	//TODO
	snake.push(encrypt(tail_x, tail_y));
	snake.push(encrypt(body_x, body_y));
	snake.push(encrypt(head_x, head_y));

	//check up
	check(head_x, head_y, true);
}
void Map::check(int head_x, int head_y, bool change) {
	current_direction = UP;
	if (head_x - 1 >= 0 && free(grid[head_x - 1][head_y])) {
		if (change)
			current_direction = UP;
	} else {
		if (head_x + 1 < Length && free(grid[head_x + 1][head_y])) {

			if (change)
				current_direction = DOWN;
		} else {
			if (head_y - 1 >= 0 && free(grid[head_x][head_y - 1])) {
				if (change)
					current_direction = Left;
			} else {
				if (head_y + 1 < Width && free(grid[head_x][head_y + 1])) {
					if (change)
						current_direction = Right;
				} else {
					error = true;
					error_msg = "The Snake can't Move";
				}
			}
		}

	}

}

bool Map::isDead() {
	if (dead)
		return dead;

	//check for Death conditions
	int snake_pos = snake.back();
	int* data = decrypt(snake_pos);

	check(data[0], data[1], false);
	if (error)
		dead = true;
	return dead;
}
void Map::move(int direction) {
	//TODO
	switch (direction) {
	case Left:
		current_direction = Left;
		break;
	case Right:
		current_direction = Right;
		break;
	case Periodic:
		break;
	case UP:
		current_direction = UP;
		break;
	case DOWN:
		current_direction = DOWN;
		break;
	default:
		break;
	}
}
void Map::update() {
	//TODO
	// first check if the next step is valid
	int* head = decrypt(snake.back());
	int head_x = head[0];
	int head_y = head[1];
	bool grow = false;

	if (free(
			grid[head_x + delta_x[current_direction]][head_y
					+ delta_y[current_direction]])) {
		int type = grid[head_x + delta_x[current_direction]][head_y
				+ delta_y[current_direction]];
		if (isFood(type)) {
			grow = true;
			score += foodScore(type);
			current_score += foodScore(type);
			if (type == SPECIAL) {
				special_exist = false;
				special_timer = -1;
			} else {
				foodEaten++;
				if (foodEaten == SPECIAL_CONDITION && !special_exist) {
					addSpecial();
					foodEaten = 0;
				}
			}
			grid[head_x + delta_x[current_direction]][head_y
					+ delta_y[current_direction]] = EMPTY;

			addFood();
			//Special Condition to add Special Food
		}
		if (special_exist) {
			special_timer--;
			if (special_timer == 0) {
				special_exist = false;
				special_timer = -1;
				grid[special_x][special_y] = EMPTY;
				special_x = -1;
				special_y = -1;
			}
		}
		//TODO
		/*Actions taken when the snake eats.
		 * */
	} else {
		dead = true;
		return;
	}
	// First check if we need to add another part to the snake or not
	int temp = snake.front();//Remove the tail
	if (!grow)
		snake.pop();
	head = decrypt(temp);
	grid[head[0]][head[1]] = EMPTY;
	temp = snake.back();//old head
	head = decrypt(temp);
	grid[head[0]][head[1]] = Snake_Body;
	// now head contains the new position of head
	head[0] = head_x + delta_x[current_direction];
	head[1] = head_y + delta_y[current_direction];
	head_x = head[0];
	head_y = head[1];
	grid[head[0]][head[1]] = Snake_Head;
	temp = encrypt(head[0], head[1]);
	snake.push(temp);
	temp = snake.front();//Remove the tail
	head = decrypt(temp);
	grid[head[0]][head[1]] = Snake_Tail;
	// now check if the next move in any direction/Current Direction is food/special
	for (temp = 0; temp < 4; temp++) {
		if (isFood(grid[head_x + delta_x[temp]][head_y + delta_y[temp]])) {
			temp = 5;
			eating = true;
		}
	}
	if (temp == 4)
		eating = false;
}

void Map::readFile() {
	ifstream file;
	char* fileName = new char[files[level - 1].length() + 1];
	strcpy(fileName, files[level - 1].c_str());

	file.open(fileName);
	string line;
	int row = 0;
	if (file.is_open()) {
		while (getline(file, line)) {
			if (line.length() != Width * 2 - 1) {
				error_msg = "Error in File Format , line (row) ";
				error_msg += row;
				error_msg += " doesn't fit the grid width ";
				error_msg += line;
				cout << error_msg << endl;
				error = true;
				file.close();
				return;
			}

			for (int i = 0; i < Width; i++) {
				if (((2 * i) + 1) < (2 * Width - 1) && line[(2 * i) + 1] != ' ') {
					error_msg = "Error in File Format , line (row) ";
					error_msg += row;
					error_msg += " slot ";
					error_msg += ((2 * i) + 1);
					error_msg += " =|";
					error_msg += line[(2 * i) + 1];
					error_msg += "| is incorrect";
					cout << error_msg << endl;
					error = true;
					file.close();
					return;
				}
				int value = (int) line[2 * i] - (int) '0';
				if (value < 0 || value > max_value) {
					error_msg += "Error in File Format , Line (row) ";
					error_msg += row + (int) '0';
					error_msg += " slot ";
					error_msg += (2 * i) + (int) '0';
					error_msg += "|";
					error_msg += line[2 * i];
					error_msg += "|";
					error_msg += " is incompatible.";
					cout << error_msg << endl;
					error = true;
					file.close();
					return;
				} else {
					grid[row][i] = (int) line[2 * i] - (int) '0';
				}
			}
			row++;
		}
		if (row != Length) {
			error_msg
					+= "Error in File Format , Number of rows is incorrect = ";
			error_msg += row;
			error_msg += " and should be ";
			error_msg += Length;
			cout << error_msg << endl;
			error = true;
			file.close();
			return;
		}
	} else {
		error_msg += "Error File ";
		error_msg += files[level - 1];
		error_msg += " doesn't exist";
		cout << error_msg << endl;
		error = true;
		file.close();
		return;
	}
	//TODO
	file.close();
}
void Map::printArray() {
	cout << separator_line << endl;
	cout << "GRID" << endl;
	if (dead)
		cout << "Snake is Dead" << endl;
	else
		cout << "Snake is Alive" << endl;

	if (eating)
		cout << "Snake is about to eat" << endl;
	else
		cout << "No near food " << endl;
	cout << endl << endl;
	for (int i = 0; i < Length; i++) {
		cout << grid[i][0];
		for (int j = 1; j < Width; j++) {
			cout << " , " << grid[i][j];
		}
		cout << endl;
	}
	cout << separator_line << endl;
}
bool Map::levelUp() {
	level++;
	if (level > Max_Level)
		return false;
	generate_map();

	return true;
}

void Map::addFood() {
	//TODO

	int* food = foodPosition();

	if (food[0] != -1) {
		grid[food[0]][food[1]] = FOOD;
	}

}
void Map::addSpecial() {
	if (!special_exist) {
		int* food = foodPosition();
		if (food[0] != -1) {
			grid[food[0]][food[1]] = SPECIAL;
			special_exist = true;
			special_x = food[0];
			special_y = food[1];
			special_timer = SPECIAL_TIMER;
		}
	}
	//TODO
}

int* Map::foodPosition() {
	int* head = decrypt(snake.back());
	int* food = new int[2];
	food[0] = -1;
	food[1] = -1;

	double max_distance = -1;

	for (int i = 0; i < Length; i++) {
		for (int j = 0; j < Width; j++) {
			if (grid[i][j] == EMPTY) {
				double distance = getDistance(head, i, j);
				if (distance > max_distance) {
					max_distance = distance;
					food[0] = i;
					food[1] = j;
				}
			}
		}
	}
	return food;
}

/*
 Methods to use
 Map myMap;
 myMap.initialize(); // Initialize the game
 int direction = Periodic;//Possible directions {Left,Right,Up,Down,Periodic==CurrentDirection}
 myMap.move(direction); // After t seconds force the snake to move in a certain direction
 myMap.isDead();//True if the Snake is dead :D
 myMap.score;//Shows the current Score
 myMap.levelUp();//Starts a new level , don't know yet when are we going to do that!!
 myMap.error;
 myMap.error_msg;
 myMap.grid;//The map ,grid is int[Length][Width], possible values {EMPTY,WALL,FOOD,SPECIAL,Snake_Head,Snake_Tail,Snake_Body}
 Length;
 Width;
 myMap.eating;//If true this means that the head's next position in the current direction contains food , if we want to make a special shape in condition of eating food
 myMap.addFood();//Drop Normal Food in a random place
 myMap.addSpecial();//Drop Special Food in a random place
 return 0;
 }
 */
//int main(int argc, char **argv) {
//	Map e;
//	e.initialize();
//
//	cout << tab << "Core Test " << endl;
//	cout << separator_line << endl;
//	cout << "Instructions : " << endl;
//	cout << tab << "Update : 1" << endl;
//	cout << tab << "Move   : 2" << endl;
//	cout << tab << "Print  : 3" << endl;
//	cout << tab << "Exit   : 4" << endl;
//
//	int input;
//	cin >> input;
//
//	while (input != 4) {
//		switch (input) {
//		case 1:
//			e.update();
//			break;
//		case 2:
//			cout << tab << tab << "Enter Direction" << endl;
//			cin >> input;
//			e.move(input);
//			break;
//		case 3:
//			e.printArray();
//			break;
//		case 4:
//			e.printArray();
//			return 0;
//			break;
//		default:
//			break;
//		}
//
//		cout << separator_line << endl;
//		cout << tab << "Core Test " << endl;
//		cout << separator_line << endl;
//		cout << "Instructions : " << endl;
//		cout << tab << "Update : 1" << endl;
//		cout << tab << "Move   : 2" << endl;
//		cout << tab << "Print  : 3" << endl;
//		cout << tab << "Exit   : 4" << endl;
//		cin >> input;
//	}
//
//	return 0;
//}
