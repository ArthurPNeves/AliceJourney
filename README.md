# A* Maze Solver
This project implements the A* algorithm to solve mazes. The algorithm finds the shortest path from the top-left corner to the bottom-right corner of a given ASCII maze.

## Overview

This implementation leverages the A* algorithm, which uses both actual and estimated costs to prioritize paths that are more likely to reach the goal efficiently. The solution is written in C and supports mazes with a maximum width and height of 10,000.

## A* Algorithm

1. **Initialization**: Start at the top-left corner and calculate the initial costs.
2. **Node Exploration**: Evaluate neighbors and select the node with the lowest `totalEstimatedCost`.
3. **Path Reconstruction**: Once the goal is reached, backtrack from the goal to reconstruct the path.
4. **Completion**: If the goal is unreachable, the algorithm concludes with no solution.

## CPU vs. RAM Usage
- **CPU Usage**: A* is a CPU-intensive algorithm due to the frequent calculations and comparisons required to evaluate each node's cost. This is particularly noticeable in large mazes with many nodes to explore.
- **RAM Usage**: RAM consumption grows with the maze size, as the algorithm stores information about explored and unexplored nodes. Large mazes can demand significant memory resources, especially since both `openList` and `closedList` scale with the maze dimensions.

## Data Structures

- **Node Structure**: 
  - Each node represents a position in the maze and contains:
    - `x` and `y` coordinates.
    - `costFromStart`: Cost from the start node.
    - `estimatedCostToGoal`: Heuristic estimate to the goal.
    - `totalEstimatedCost`: Sum of `costFromStart` and `estimatedCostToGoal`.
    - `parent`: A pointer to the previous node, allowing for path reconstruction.

- **Open List**: A priority queue (min-heap) is used to store nodes that are yet to be explored, with the node having the lowest `totalEstimatedCost` given the highest priority. This ensures that the node with the best estimated path cost is selected for exploration next.

- **Closed List**: A boolean array marking nodes as explored to prevent revisiting them, which helps to improve the efficiency by reducing redundant path explorations.

## Efficiency

- **Time Complexity**: A* has a worst-case time complexity of \(O(b^d)\), where \(b\) is the branching factor and \(d\) is the depth of the shortest path. While generally efficient, its performance can decrease with larger and more complex mazes due to the increased number of nodes in the open list.

- **Memory Complexity**: A* stores all nodes in the open list, which can lead to high memory usage. This limits its scalability in very large mazes, as it requires more RAM to store potential paths.


## Heuristic

- **Manhattan Distance**: This heuristic calculates the sum of the horizontal and vertical distances from the current node to the goal. It was chosen for this implementation because it provides the fastest estimate without considering diagonal movement, which is suitable for grid-based mazes where movement is restricted to vertical and horizontal directions.
- **Impact**: The heuristic guides the algorithm toward the goal, helping avoid unnecessary exploration and improving overall efficiency.


## Usage

To compile and run the program, follow these steps:

1. Compile the program using a C compiler:
   ```bash
   gcc -o solver solver.c

2. Run
   ```bash
    .\solver.exe maze.txt
