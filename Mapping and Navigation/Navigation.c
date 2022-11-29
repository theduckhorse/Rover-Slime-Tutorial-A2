#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <iso646.h>

#define MOVE_DISTANCE 13.5 //Define robot to move a distance of 13.5cm then start to record data again
#define COORDINATES 2

// MAZE TEST 1 
// ████████████
// █xxxxx.....█
// █x█.█x████.█
// █x█.█xxxx█.█
// █x█.██████.█
// █x█........█
// █x████████.█
// █x.........█
// ████████████
// path cost is 16:

// //Initialise test path
// int path[][COORDINATES] = {
//     {8, 3}, //move forward
//     {7, 3}, //move forward
//     {6, 3}, //move forward
//     {5, 3}, // turn right, move forward
//     {5, 2}, //move forward
//     {5, 1}, // turn left, move forward
//     {4, 1}, //move forward
//     {3, 1}, //move forward
//     {2, 1}, //move forward
//     {1, 1}, //  turn left, move forward
//     {1, 2}, //move forward
//     {1, 3}, //move forward
//     {1, 4}, //move forward
//     {1, 5}, //move forward
//     {1, 6}, //move forward
//     {1, 7}
// };

// MAZE TEST 2
// ███████████
// █xxxxx....█
// █x█.█x███.█
// █x█.█xxx█.█
// █x█.█████.█
// █x█.......█
// █x███████.█
// █x........█
// ███████████
// path cost is 15:


int path[][COORDINATES] = {
    {1, 7},
    {1, 6},
    {1, 5},
    {1, 4},
    {1, 3},
    {1, 2},
    {1, 1},
    {2, 1},
    {3, 1},
    {4, 1},
    {5, 1},
    {5, 2},
    {5, 3},
    {6, 3},
    {7, 3}
};

int total_Cost = 16; //p_len

void turnLeft(){
    printf("Turning Left 90 Degrees!\n");
}

void turnRight(){  
    printf("Turning right 90 Degrees!\n");
}

void uTurn(){
    turnLeft();
    turnLeft();
}

void moveForward(){
    //Car moves forward by MOVE_DISTANCE [13.5cm]
    printf("Moving Forward!\n");
}

    // Horizontal Left <-
    // {2, 1}, //move forward
    // {1, 1}, //  turn left, move forward
    // {1, 2}, //move forward
// ---------------------------------------ASSUMING YOU ARE IN A 2D PLANE-------------------------------------
    // Firsly, Check whether previous y and current y coordinates are the same, means you are moving along the x - axis(Horizontally)
    // Secondly, Check if the current y is LESS than next y, which means you have gone DOWNWARDS from moving along the x axis
    // Thirdly, check the direction you are heading when u moving along the x axis, if current x is less then previous x, it means you are going from RIGHT TO LEFT
    // Therefore, when you go from RIGHT TO LEFT, then head DOWNWARDS, it results in a LEFT TURN.


    // Horizontal Left ->
    // {1, 2}, //move forward
    // {2, 2}, //  turn left, move forward
    // {2, 1}, //move forward
// ---------------------------------------ASSUMING YOU ARE IN A 2D PLANE-------------------------------------
    // Firsly, Check whether previous y and current y coordinates are the same, means you are moving along the x - axis(Horizontally)
    // Secondly, Check if the current y is MORE than next y, which means you have gone UPWARDS from moving along the x axis
    // Thirdly, check the direction you are heading when u moving along the x axis, if current x is MORE then previous x, it means you are going from LEFT to RIGHT
    // Therefore, when you go from LEFT to RIGHT, then head UPWARDS, it results in a LEFT TURN.


    // Vertical Left ^ 
    // {5, 2}, //move forward
    // {5, 1}, // turn left, move forward
    // {4, 1}, //move forward
// ---------------------------------------ASSUMING YOU ARE IN A 2D PLANE-------------------------------------
    // Firsly, Check whether previous x and current x coordinates are the same, means you are moving along the y - axis(Vertically)
    // Secondly, Check if the current x is MORE than next x, which means you have TURNED LEFT from moving along the y axis
    // Thirdly, check the direction you are heading when u moving along the Y axis, if current y is LESS then previous y, it means you are going from BOTTOM TO UP
    // Therefore, when you go from BOTTOM to UP, then head LEFTWARDS, it results in a LEFT TURN. 


    // Vertical Left _ 
    // {5, 1}, //move forward
    // {5, 2}, // turn left, move forward
    // {4, 1}, //move forward
// ---------------------------------------ASSUMING YOU ARE IN A 2D PLANE-------------------------------------
    // Firsly, Check whether previous x and current x coordinates are the same, means you are moving along the y - axis(Vertically)
    // Secondly, Check if the current x is LESS than next x, which means you have TURNED RIGHT from moving along the y axis
    // Thirdly, check the direction you are heading when u moving along the Y axis, if current y is MORE then previous y, it means you are going from UP TO BOTTOM
    // Therefore, when you go from UP to BOTTOM, then head RIGHT-WARDS, it results in a LEFT TURN.     


    // Horizontal Right <-
    // {6, 3}, //move forward
    // {5, 3}, // turn right, move forward
    // {5, 2}, //move forward
// --------------------------------------ASSUMING YOU ARE IN A 2D PLANE-------------------------------------
    // Firsly, Check whether previous y and current y coordinates are the same, means you are moving along the x - axis(Horizontally)
    // Secondly, Check if the current y is MORE than next y, which means you have gone UPWARDS from moving along the x axis
    // Thirdly, check the direction you are heading when u moving along the x axis, if current x is less then previous x, it means you are going from RIGHT TO LEFT
    // Therefore, when you go from RIGHT TO LEFT, then head UPWARDS, it results in a RIGHT TURN.


    // Horizontal Right ->
    // {4, 3}, //move forward
    // {5, 3}, // turn right, move forward
    // {5, 4}, //move forward
// --------------------------------------ASSUMING YOU ARE IN A 2D PLANE-------------------------------------
    // Firsly, Check whether previous y and current y coordinates are the same, means you are moving along the x - axis(Horizontally)
    // Secondly, Check if the current y is MORE than next y, which means you have gone UPWARDS from moving along the x axis
    // Thirdly, check the direction you are heading when u moving along the x axis, if current x is less then previous x, it means you are going from RIGHT TO LEFT
    // Therefore, when you go from RIGHT TO LEFT, then head UPWARDS, it results in a RIGHT TURN.


    // Vertial Right ^
    // {5, 2}, //move forward
    // {5, 1}, // turn left, move forward
    // {6, 1}, //move forward
// ---------------------------------------ASSUMING YOU ARE IN A 2D PLANE-------------------------------------
    // Firsly, Check whether previous x and current x coordinates are the same, means you are moving along the y - axis(Vertically)
    // Secondly, Check if the current x is LESS than next x, which means you have TURNED RIGHT from moving along the y axis
    // Thirdly, check the direction you are heading when u moving along the Y axis, if current y is LESS then previous y, it means you are going from BOTTOM TO UP
    // Therefore, when you go from BOTTOM to UP, then head RIGHT-WARDS, it results in a RIGHT TURN. 

    // Vertial Right _
    // {5, 1}, //move forward
    // {5, 2}, // turn left, move forward
    // {4, 2}, //move forward
// ---------------------------------------ASSUMING YOU ARE IN A 2D PLANE-------------------------------------
    // Firsly, Check whether previous x and current x coordinates are the same, means you are moving along the y - axis(Vertically)
    // Secondly, Check if the current x is MORE than next x, which means you have TURNED LEFT from moving along the y axis
    // Thirdly, check the direction you are heading when u moving along the Y axis, if current y is MORE then previous y, it means you are going from UP TO BOTTOM
    // Therefore, when you go from UP to BOTTOM, then head LEFT-WARDS, it results in a RIGHT TURN. 


int follow_The_Path(int path[][2], int total_Cost){
    int i, j;
    for(i = 0; i < total_Cost -1 ; i++){ // total_cost - 1 because last node don't have to move anymore.
        if((path[i][1] == path[i-1][1] and path[i][1]<path[i+1][1] and path[i][0] < path[i-1][0]) or // Horizontal Left <-
        (path[i][1] == path[i-1][1] and path[i][1]>path[i+1][1] and path[i][0] > path[i-1][0]) or  // Horizontal Left ->
        (path[i][0] == path[i-1][0] and path[i][0] > path[i+1][0] and path[i][1] < path[i-1][1]) or // Vertical Left ^ 
        (path[i][0] == path[i-1][0] and path[i][0] < path[i+1][0] and path[i][1] > path[i-1][1])) { // Vertical Left _ 

            turnLeft();
            moveForward();

        }else if((path[i][1] == path[i-1][1] and path[i][1]>path[i+1][1] and path[i][0] < path[i-1][0]) or  // Horizontal Right <-
        (path[i][1] == path[i-1][1] and path[i][1] < path[i+1][1] and path[i][0] > path[i-1][0]) or // Horizontal Right ->
        (path[i][0] == path[i-1][0] and path[i][0] < path[i+1][0] and path[i][1] < path[i-1][1]) or // Vertial Right ^
        (path[i][0] == path[i-1][0] and path[i][0] > path[i+1][0] and path[i][1] > path[i-1][1])) { // Vertial Right _

            turnRight();
            moveForward();

        }else{
            moveForward();
        }
    }
}

int main(){
    follow_The_Path(path, total_Cost);
    return 0;
}