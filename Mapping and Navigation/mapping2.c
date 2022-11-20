#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX 4 // define maximum amount of neighbours each node can have [up down left right]

#define MOVE_DISTANCE 13.5 //Define robot to move a distance of 13.5cm then start to record data again

#define WALL_DISTANCE 3 // define max wall distance to mark it as wall



struct Node{
    int data[3]; // left ultra, front ultra right ultra (1 if wall 0 if empty)
    int dir[4]; //direction [0 = UP, 1 = LEFT, 2 = RIGHT, 3 = DOWN]
    int distance; // distance traveled from previous node
    int num_edges; // number of edges in this node 
    struct Node *nodes[MAX]; // the neighbours in this node
};

int join( struct Node *a, struct Node *b )
{
    if( a->num_edges >= MAX || b->num_edges >= MAX ) return 0;
    a->nodes[a->num_edges++] = b;
    b->nodes[b->num_edges++] = a;
    b->distance = MOVE_DISTANCE;
    return 1;
}

int has_edge( struct Node *a, struct Node *b )
{
    int i;
    for( i = 0; i < a->num_edges; i++ ) {
        if( a->nodes[i] == b ) return 1;
    }
    return 0;
}

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


//function to create node
// int createNode(int *node_Counter, double frontSensor, double leftSensor, double rightSensor){ 
//     struct Node *(*node_Counter);

//     *node_Counter = (struct (Node)*) malloc(sizeof(struct Node));

// }

int main(){

int num_of_LeftTurns = 0;
int num_of_RightTurns = 0;
int front_Position_Detector = 0; // + 13.5 if moving forward, -13.5 after turning left or turning right is even
int Right_Position_Detector = 0; // + 13.5 after turning right , -13.5 after turning left
int Left_Position_Detector = 0; // + 13.5 after turning left. -13.5 after turning right 
int direction_Array_var = 1;
int flag = 0; // Set to 1 when mapping is complete
int node_Counter = 1; //Define node counter Variable
int node_subPointer = 0;
bool initialization = true;
struct Node *ptr;
int stack[10];

    while(flag == 0){
        //Initial Memory Allocation to Node Struct
        ptr = (struct Node*)malloc(node_Counter * sizeof(struct Node));
        

        //Only execute when it is at starting point
        if(initialization == true){
            if(frontSensor > WALL_DISTANCE){
                if(leftSensor < WALL_DISTANCE && rightSensor < WALL_DISTANCE){
                    (ptr + node_subPointer)->data[0] = 1; // At initial, if left wall and right wall is detected.
                    (ptr + node_subPointer)->data[1] = 0;
                    (ptr + node_subPointer)->data[2] = 1;
                    (ptr + node_subPointer)->distance = 0;
                    (ptr + node_subPointer)->num_edges = 0;
                    node_subPointer++;
                    node_Counter++;
                    initialization = false;
                }else if(leftSensor < WALL_DISTANCE && rightSensor > WALL_DISTANCE){
                    (ptr + node_subPointer)->data[0] = 1; // At initial, if left wall detected only.
                    (ptr + node_subPointer)->data[1] = 0;
                    (ptr + node_subPointer)->data[2] = 0;
                    (ptr + node_subPointer)->distance = 0;
                    (ptr + node_subPointer)->num_edges = 0;
                    node_subPointer++;
                    node_Counter++;
                    initialization = false;
                }else if(rightSensor < WALL_DISTANCE && leftSensor > WALL_DISTANCE){
                    (ptr + node_subPointer)->data[0] = 0; // At initial, if right wall detected only.
                    (ptr + node_subPointer)->data[1] = 0;
                    (ptr + node_subPointer)->data[2] = 1;
                    (ptr + node_subPointer)->distance = 0;
                    (ptr + node_subPointer)->num_edges = 0;
                    node_subPointer++;
                    node_Counter++;
                    initialization = false;
                }
            }else{
                turnLeft(); // turn the robot until the front is clear
                continue;
            }
        }

        moveForward();
        front_Position_Detector = front_Position_Detector + MOVE_DISTANCE;

        if(frontSensor > WALL_DISTANCE){
            if(leftSensor < WALL_DISTANCE && rightSensor < WALL_DISTANCE){
                    (ptr + node_subPointer)->data[0] = 1; // At initial, if left wall and right wall is detected.
                    (ptr + node_subPointer)->data[1] = 0;
                    (ptr + node_subPointer)->data[2] = 1;
                    join(&ptr[node_subPointer], &ptr[node_subPointer-1]); //add edge to current node and previous node.
                    node_subPointer++;
                    node_Counter++;
                }else if(leftSensor < WALL_DISTANCE && rightSensor > WALL_DISTANCE){
                    (ptr + node_subPointer)->data[0] = 1; // At initial, if left wall detected only.
                    (ptr + node_subPointer)->data[1] = 0;
                    (ptr + node_subPointer)->data[2] = 0;
                    join(&ptr[node_subPointer], &ptr[node_subPointer-1]); //add edge to current node and previous node.
                    node_subPointer++;
                    node_Counter++;
                }else if(rightSensor < WALL_DISTANCE && leftSensor > WALL_DISTANCE){
                    (ptr + node_subPointer)->data[0] = 0; // At initial, if right wall detected only.
                    (ptr + node_subPointer)->data[1] = 0;
                    (ptr + node_subPointer)->data[2] = 1;
                    join(&ptr[node_subPointer], &ptr[node_subPointer-1]); //add edge to current node and previous node.
                    node_subPointer++;
                    node_Counter++;
                }
        }else{
            if(leftSensor < WALL_DISTANCE && rightSensor < WALL_DISTANCE){
                    (ptr + node_subPointer)->data[0] = 1; // At initial, if left wall and right wall is detected.
                    (ptr + node_subPointer)->data[1] = 1;
                    (ptr + node_subPointer)->data[2] = 1;
                    join(&ptr[node_subPointer], &ptr[node_subPointer-1]); //add edge to current node and previous node.
                    node_subPointer++;
                    node_Counter++;
                }else if(leftSensor < WALL_DISTANCE && rightSensor > WALL_DISTANCE){
                    (ptr + node_subPointer)->data[0] = 1; // At initial, if left wall detected only.
                    (ptr + node_subPointer)->data[1] = 1;
                    (ptr + node_subPointer)->data[2] = 0;
                    join(&ptr[node_subPointer], &ptr[node_subPointer-1]); //add edge to current node and previous node.
                    node_subPointer++;
                    node_Counter++;
                }else if(rightSensor < WALL_DISTANCE && leftSensor > WALL_DISTANCE){
                    (ptr + node_subPointer)->data[0] = 0; // At initial, if right wall detected only.
                    (ptr + node_subPointer)->data[1] = 1;
                    (ptr + node_subPointer)->data[2] = 1;
                    join(&ptr[node_subPointer], &ptr[node_subPointer-1]); //add edge to current node and previous node.
                    node_subPointer++;
                    node_Counter++;
                }



        }

    }   

}