# Mapping & Navigation
The mapping and navigation was done using Raspberry PICO. The demonstration video will illustrate how PICO was used and the ouputs on the displayed on console. 

## Usage of Raspberry Pico
The Shortest Path algorithm as well as navigation can be executed with the raspberry PICO module with the "map_nav.uf2" file in the mapping and navigation repository.

## Demonstration Video

### Part 1:

https://user-images.githubusercontent.com/97492848/204567292-2a334069-ecc6-40e3-aa0e-a504c60b00fd.mp4

### Part 2:
https://youtu.be/SDc_WiPf1T8

## Flow Charts
<img width="438" alt="image" src="https://user-images.githubusercontent.com/87920891/204574060-d37e902c-44d5-47f9-a8f4-6e265a38feed.png">

## Mapping
The Robot will start out by orientating itself to face forward. One done, it will start moving forward about 13cm by 13cm, adding nodes into a 2D array called Coordinate_Nodes along the way. Whenever the robot come accross a junction, it will add that coordinate into another 2D array called Junction_Nodes. The robot will be able to see the Coordinate of the node one step ahead through adding the y / x coordinate depending on where its facing. If the robot comes to a point where it is stepping on a visited node, it will stop its usual operations and look at the Junction_Node. It will travel back to the first coordinate of the Junction_Node and explore the unexplored paths and continue normal operations. Once the robot steps onto visted nodes again and the Junction_Node array is empty, the mapping is considered completed.


## Shortest Path Algorithm
The A* Search Algorithm selects the node at each stage based on a value, "f," which is a parameter equal to the sum of two additional factors, "g" and "h." It chooses the node or cell with the lowest "f" at each step and processes that node or cell. We define 'g' and 'h' below as simply as possible: g = the movement cost to move along the created path from the starting point to a particular square on the grid. h is the calculated cost of moving from a specific grid square to the desired location. This is frequently referred to as the heuristic, which is just another word for an educated guess. Because a variety of obstructions (such as walls) may be in the way, we actually can't know the distance until we find the path.

<img width="574" alt="image" src="https://user-images.githubusercontent.com/97492848/204579232-d1b6e0df-cc54-445d-831b-1b27ef9e0a35.png">

## Movement
Based on previous car movement, compare x and y values to find the direction the car is facing ???
From there, compare the coordinates with the next coordinate and find the direction to turn towards. ???
Example:???
When moving from (0,1) to (0,2) must mean that the car is moving from bottom up and if moving to (1,2) next, would mean that the car should turn right.???
Respectively for other directions

## Memory Usage on PICO
dynamic depending on the distance and number of nodes created
memory usage of maze = max col(11) * max row(9) * size of integer(4BYTES) 
=396 bytes

## Optimization
to use characters instead of integers as it takes up 1 byte instead of 4 

## Testing
Our group had tested that the mapping and navigation algorithm vigorously to ensure that the algorithm is working.

For black box testing, the algo was tested on its maze generation, the maze generation is done by making the 
The result of blackbox for 5 different mazes can be seen from the demo video as well as the results of the test case are shown as follows:

Maze 1                     |  Maze 2                   | Maze 3                    |  Maze 4                   |  Maze 5
:-------------------------:|:-------------------------:|:-------------------------:|:-------------------------:|:-------------------------:
![test1 result](./assets/maze1.jpeg)   |  ![test2 result](./assets/maze2.jpeg) |  ![test3 result](./assets/maze3.jpeg)  |  ![test4 result](./assets/maze4.jpeg) |  ![test5 result](./assets/maze5.jpeg) 


For whitebox testing, the algo was tested on its shortest path generation using the A* algorithm.

The image below shows an example console output during our whitebox testing:
The maze shows the coordiantes that it takes into account when moving. In addition, it shows the movement of the car as moving forward and turning left 90 degrees.

Maze 1                     |  
:-------------------------:
![test6 result](./assets/short1.jpeg) |
