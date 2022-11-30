#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX 4 // define maximum amount of neighbours each node can have [up down left right]

#define MOVE_DISTANCE 13.5 //Define robot to move a distance of 13.5cm then start to record data again

#define WALL_DISTANCE 3 // define max wall distance to mark it as wall

// int compass = 0; // 0 == Front
                           // 1 == Right
                           // 2 == Back
                           // 3 == Left

int frontSensor(){
    int distance = 0; //Insert Get ultrasonic distance funciton
    return distance;
}

int leftSensor(){
    int distance = 0; //Insert Get ultrasonic distance funciton
    return distance;
}

int rightSensor(){
    int distance = 0; //Insert Get ultrasonic distance funciton
    return distance;
}

void turnLeft(){

}

void turnRight(){

}

void uTurn(){
    turnLeft();
    turnLeft();
}

void moveForward(){
    //Car moves forward by MOVE_DISTANCE [13.5cm]
}

bool checkVisited(int num_Of_Visited, int **visted_Coordinates, int car_Coord_x, int car_Coord_y){
    int i,j;
    for(i = 0; i < num_Of_Visited; ++i){
        if(visted_Coordinates[i][0] == car_Coord_x && visted_Coordinates[i][0] == car_Coord_y){
            return false; //if visited, send false to break the loop and fall back on junction node.
        }else{
            return true; //if never visited before, return true to continue algo
        }
        
    }
}

int direction_Calculator(int compass, char direction){
    if (direction == 'R'){
        compass = (compass + 1) % 4;
    }
    if (direction == 'L'){
        compass += (compass + 3) % 4;
    }
}

int back_To_Last_Junction(int **visited_Coordinates, int num_Of_Visited, int **junction_Coordinate, int num_Of_Junction_Nodes, int current_x, int current_y, int compass){
    int goal_Position_x = 0;
    int goal_Position_y = 0;
    int tempX, tempY;
    //check if current position already exist in junction_Coordinate array
    if(checkVisited(num_Of_Junction_Nodes, junction_Coordinate, current_x, current_y)){
        delete_element(junction_Coordinate, num_Of_Junction_Nodes, current_x, current_y);
        num_Of_Junction_Nodes--;
        return num_Of_Junction_Nodes;
    }

    if(num_Of_Junction_Nodes > 0){
        goal_Position_x = junction_Coordinate[0][0];
        goal_Position_y = junction_Coordinate[0][1];
    }


    //while current postion is not equal to goal position
    while(current_x != goal_Position_x && current_y != goal_Position_y){
        if(current_x > goal_Position_x && checkVisited(num_Of_Visited, visited_Coordinates, current_x - 1, current_y)){ // if goal is southwards, check if there is a space there, face south. Then move forward
            while(compass != 2){
                turnLeft();
                direction_Calculator(compass, 'L');
                
            } 
            moveForward();
            current_x--;
        }else if(current_x < goal_Position_x && checkVisited(num_Of_Visited, visited_Coordinates, current_x + 1, current_y)){ // if goal is northwards, check if there is a space there, face north. Then move forward
            while(compass != 0){
                turnLeft();
                direction_Calculator(compass, 'L');
                
            }   
            moveForward();
            current_x++;  
        }else if(current_y < goal_Position_y && checkVisited(num_Of_Visited, visited_Coordinates, current_x, current_y + 1)){ // if goal is Eastwards, check if there is a space there, face east.  Then move forward
            while(compass != 1){
                turnLeft();
                direction_Calculator(compass, 'L');
            }   
            moveForward();
            current_y++;  
        }else if(current_y > goal_Position_y && checkVisited(num_Of_Visited, visited_Coordinates, current_x, current_y - 1)){ // if goal is Westwards, check if there is a space there, face west.  Then move forward
            while(compass != 3){
                turnLeft();
                direction_Calculator(compass, 'L');
            }   
            moveForward();
            current_y--;  
        }
    }
    delete_element(junction_Coordinate, num_Of_Junction_Nodes, current_x, current_y);
    num_Of_Junction_Nodes--;

    return num_Of_Junction_Nodes;
}

void delete_element(int **array, int num_of_element, int x, int y){
    int i, pos;
    for(i = 0; i < num_of_element; i++){
        if(array[i][0] == x && array[i][1] == y){
            pos = i;
            break;
        }
    }
    for(i = pos - 1; i < num_of_element - 1; i++){
        array[i] = array[i+1];
    }
    return 0;
}

int main(){
    int i, j;
    int x = 0;
    int y = 0;
    int tempX;
    int tempY;
    int num_Of_Nodes = 1;
    int **car_Coordinates;
    int num_Of_Visited = 0;
    int **visited_Coordinates;
    int num_Of_Junction_Nodes = 1;
    int **junction_Coordinates;
    int compass = 0; // 0 == front (Original CAR North), 1 == Right(Original CAR East) 2 == Back (Original CAR South) 3 == Left (Original CAR West)
    bool initialization_flag = true;
    int round = 0;
    int flag = 0;
    //For building 2D Map array:
    int **map;

    //Arrays: car_Coordinates tracks the coordinates of the path the car taken in each step
    //Arrays: visit_Coordinates keeps track of the coordinates that the car has taken

    while(initialization_flag){
        //Memory Allocation for 2D car_Coordinates array, visited stack array
        car_Coordinates = malloc(sizeof(int*) * num_Of_Nodes);
        visited_Coordinates = malloc(sizeof(int*) * num_Of_Nodes);
        junction_Coordinates = malloc(sizeof(int*) * num_Of_Junction_Nodes);

        if(car_Coordinates == NULL || visited_Coordinates == NULL || junction_Coordinates == NULL){
            exit(1);
        }

        for(i = 0; i < num_Of_Nodes; ++i){
            car_Coordinates[i] = malloc(sizeof(int) * 2);
            visited_Coordinates[i] = malloc(sizeof(int) * 2);
            if(car_Coordinates[i] == NULL || visited_Coordinates[i] == NULL){
                exit(1);
            }
        }

        for(i = 0; i < num_Of_Junction_Nodes; ++i){
            junction_Coordinates[i] = malloc(sizeof(int) * 2);
            if(junction_Coordinates[i] == NULL){
                exit(1);
            }
        }


        // Making sure car is facing the front at the start
        if(frontSensor > WALL_DISTANCE && round == 0){
            car_Coordinates[round][0] = x;
            car_Coordinates[round][1] = y; 
            visited_Coordinates[round][0] = x;
            visited_Coordinates[round][1] = y;
            moveForward();
            num_Of_Nodes++;
            num_Of_Visited++;
            round++;
        }else{
            turnLeft(); // turn the robot until the front is clear
            continue;
        }



        tempX = x;
        tempY = y; //Purpose is to check if current coordinate is visted.

         //Check current direction and adjust x/y accordingly.
        if(compass == 0 && checkVisited(num_Of_Visited, visited_Coordinates, ++tempX, tempY)){ // North facing
            car_Coordinates[round][0] = ++x;
            car_Coordinates[round][1] = y; 
            visited_Coordinates[round][0] = ++x;
            visited_Coordinates[round][1] = y;
        }else if(compass == 1 && checkVisited(num_Of_Visited, visited_Coordinates, tempX, ++tempY)){ // East facing
            car_Coordinates[round][0] = x;
            car_Coordinates[round][1] = ++y; 
            visited_Coordinates[round][0] = x;
            visited_Coordinates[round][1] = ++y;
        }else if(compass == 2 && checkVisited(num_Of_Visited, visited_Coordinates, --tempX, tempY)){ // South Facing
            car_Coordinates[round][0] = --x;
            car_Coordinates[round][1] = y; 
            visited_Coordinates[round][0] = --x;
            visited_Coordinates[round][1] = y;
        }else if (compass == 3 && checkVisited(num_Of_Visited, visited_Coordinates, tempX, --tempY)){ // West facing
            car_Coordinates[round][0] = x;
            car_Coordinates[round][1] = --y; 
            visited_Coordinates[round][0] = x;
            visited_Coordinates[round][1] = --y;
        }else{//when car hits a visited node
            if (num_Of_Junction_Nodes == 0){
                printf("Maze Explored!");
                initialization_flag == false;
                build_Map(map, car_Coordinates, num_Of_Nodes);
                break;
            }
            num_Of_Junction_Nodes = back_To_Last_Junction(visited_Coordinates, num_Of_Visited, junction_Coordinates, num_Of_Junction_Nodes, x, y, compass); // Moves car to a visted junction
            x = junction_Coordinates[0][0];
            y = junction_Coordinates[0][1];
          
            while(frontSensor < WALL_DISTANCE && flag == 0){ //Orientates car to the direction where there is a path.
                turnLeft();
                direction_Calculator(compass, 'L');
                if(compass == 0 && checkVisited(num_Of_Visited, visited_Coordinates, ++tempX, tempY)){ // North facing
                    flag = 1;
                }else if(compass == 1 && checkVisited(num_Of_Visited, visited_Coordinates, tempX, ++tempY)){ // East facing
                    flag = 1;
                }else if(compass == 2 && checkVisited(num_Of_Visited, visited_Coordinates, --tempX, tempY)){ // South Facing
                    flag = 1;
                }else if (compass == 3 && checkVisited(num_Of_Visited, visited_Coordinates, tempX, --tempY)){ // West facing
                    flag = 1;
                }
            }
            flag = 0;
        }

        //LEFT PATH PRIORITY, FOLLOWED BY FRONT PATH, THEN RIGHT PATH
        if(frontSensor < WALL_DISTANCE && rightSensor > WALL_DISTANCE && leftSensor < WALL_DISTANCE){//only Right path is available, turn car right        
            turnRight();
            direction_Calculator(compass, 'R');
        }else if(frontSensor < WALL_DISTANCE && rightSensor < WALL_DISTANCE && leftSensor > WALL_DISTANCE){//only Left path is available, turn car left      
            turnLeft();
            direction_Calculator(compass, 'L');
        }else if(frontSensor > WALL_DISTANCE && rightSensor > WALL_DISTANCE && leftSensor < WALL_DISTANCE){//Front and Right path is available, move forward, but add coordinates to the junction_Coordinate array        
            junction_Coordinates[round][0] = x;
            junction_Coordinates[round][1] = y;
        }else if((frontSensor > WALL_DISTANCE && rightSensor < WALL_DISTANCE && leftSensor > WALL_DISTANCE) || //Front and Left path is available,
                 (frontSensor < WALL_DISTANCE && rightSensor > WALL_DISTANCE && leftSensor > WALL_DISTANCE) || //or Left and Right path is available,
                 (frontSensor > WALL_DISTANCE && rightSensor > WALL_DISTANCE && leftSensor > WALL_DISTANCE)){  //all paths available,  turn car left and add coordinates to the junction_Coordinate array        

            turnLeft();
            direction_Calculator(compass, 'L');       
            junction_Coordinates[round][0] = x;
            junction_Coordinates[round][1] = y;
        }else if(frontSensor < WALL_DISTANCE && rightSensor < WALL_DISTANCE && leftSensor < WALL_DISTANCE){//deadend
            uTurn();
            direction_Calculator(compass, 'L');      
            direction_Calculator(compass, 'L');      
        }

        moveForward();
        num_Of_Nodes++;
        num_Of_Visited++;
        round++; 
    }





}

void build_Map(int **map, int **car_Coordinate, int num_of_Nodes){
    int i, j;
    int min_X = 0;
    int min_Y = 0;
    int max_X = 0;
    int max_Y = 0;
    
    // looking for min value of x and y as there will be negatives
    for(i = 0; i < num_of_Nodes; i++){
        if(car_Coordinate[i][0] < min_X){
            min_X = car_Coordinate[i][0];
        }

         if(car_Coordinate[i][1] < min_Y){
            min_Y = car_Coordinate[i][1];
        }
    }

    // Adding all x and y values to the abs value of the minimum current values to remove all negatives
    for(i = 0; i < num_of_Nodes; i++){
        car_Coordinate[i][0] += abs(min_X);
        car_Coordinate[i][1] += abs(min_Y); 
    }

    for(i = 0; i < num_of_Nodes; i++){
        if(car_Coordinate[i][0] > max_X){
            max_X = car_Coordinate[i][0];
        }

         if(car_Coordinate[i][1] > max_Y){
            max_Y = car_Coordinate[i][1];
        }
    }

    //Memmory allocation for map
    map = malloc(sizeof(int*) * max_X);

    for(i = 0; i < max_X; ++i){
        map[i] = malloc(sizeof(int) * max_Y);
        if(map[i] == NULL){
            exit(1);
        }
    }
    //      Complete Build Map function
    for(i=0; i < max_Y; i++){
        for(j=0; j < max_X; j++){
            memcpy(map[i][j] + 2, car_Coordinate[i][j], sizeof(car_Coordinate[i][j]));
        }
    }



}