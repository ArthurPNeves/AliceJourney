#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define MAX_DIMENSION 10000

typedef struct Node
{
    int x, y;
    int costFromStart;
    int estimatedCostToGoal;
    int totalEstimatedCost;
    struct Node *parent;
} Node;

char **maze;
int width = 0;
int height = 0;

bool load_maze(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening the file");
        return false;
    }
    // Start capacity small
    int capacity = 1000;
    maze = malloc(capacity * sizeof(char *));
    if (!maze)
    {
        perror("Error allocating memory for maze");
        fclose(file);
        return false;
    }

    // Read File line by line and store in maze
    char buffer[MAX_DIMENSION + 1];
    int y = 0;
    while (fgets(buffer, sizeof(buffer), file))
    {
        buffer[strcspn(buffer, "\n")] = '\0';

        // verify width size is bigger than 10.000
        if (y != 0)
        {
            if (strlen(buffer) != width)
            {
                printf("Error: maze width is not consistent.\n");
                fclose(file);
                return false;
            }
        }

        width = strlen(buffer);
        if (width > MAX_DIMENSION)
        {
            printf("Error: maze width (%d) exceeds the maximum allowed (%d).\n", width, MAX_DIMENSION);
            fclose(file);
            return false;
        }

        // duplicate capacity if needed
        if (y * width >= capacity)
        {
            capacity *= 2;
            char **new_maze = realloc(maze, capacity * sizeof(char *));
            if (!new_maze)
            {
                printf("Error reallocating memory for maze");
                fclose(file);
                return false;
            }
            maze = new_maze;
        }

        maze[y] = malloc((width + 1) * sizeof(char));
        strcpy(maze[y], buffer);
        y++;

        if (y > MAX_DIMENSION)
        {
            // verify height size is bigger than 10.000

            printf("Error: maze height (%d) exceeds the maximum allowed (%d).\n", y, MAX_DIMENSION);
            fclose(file);
            return false;
        }
    }
    height = y;
    fclose(file);

    printf("Maze Width %d Height %d \n", width, height);
    for (int i = 0; i < height; i++)
    {
        printf("%s\n", maze[i]);
    }

    return true;
}

int heuristic(int x1, int y1, int x2, int y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

bool is_valid(int x, int y)
{
    return (x >= 0 && x < width && y >= 0 && y < height && maze[y][x] == '*');
}

void a_star(int startX, int startY, int endX, int endY)
{
    // Allocate memory for openList and closedList
    Node **openList = malloc(width * height * sizeof(Node *));
    if (!openList)
    {
        printf("Error allocating memory for openList");
        return;
    }

    bool **closedList = malloc(height * sizeof(bool *));
    for (int i = 0; i < height; i++)
    {
        closedList[i] = calloc(width, sizeof(bool));
    }

    int openCount = 0;
    bool found = false;
    // Start inicial Node
    Node *start = malloc(sizeof(Node));
    start->x = startX;
    start->y = startY;
    start->costFromStart = 0;
    start->estimatedCostToGoal = heuristic(startX, startY, endX, endY);
    start->totalEstimatedCost = start->costFromStart + start->estimatedCostToGoal;
    start->parent = NULL;
    openList[openCount++] = start;

    Node *current = NULL;
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    while (openCount > 0)
    {

        // Find the node with the smallest totalEstimatedCost in the openList
        int idx = 0;
        for (int i = 1; i < openCount; i++)
        {
            if (openList[i]->totalEstimatedCost < openList[idx]->totalEstimatedCost)
            {
                idx = i;
            }
        }
        current = openList[idx];

        // Check if the current node is the goal
        if (current->x == endX && current->y == endY)
        {
            found = true;
            break;
        }

        // Remove the current node from the openList
        for (int i = idx; i < openCount - 1; i++)
        {
            openList[i] = openList[i + 1];
        }
        openCount--;
        closedList[current->y][current->x] = true;

        // Check the neighbors of the current node
        for (int i = 0; i < 4; i++)
        {
            int nx = current->x + dx[i];
            int ny = current->y + dy[i];

            if (is_valid(nx, ny) && !closedList[ny][nx])
            {
                int newCostFromStart = current->costFromStart + 1;
                bool inOpenList = false;
                // Check if the neighbor is in the openList
                for (int j = 0; j < openCount; j++)
                {
                    if (openList[j]->x == nx && openList[j]->y == ny)
                    {
                        inOpenList = true;
                        if (newCostFromStart < openList[j]->costFromStart)
                        {
                            openList[j]->costFromStart = newCostFromStart;
                            openList[j]->totalEstimatedCost = openList[j]->costFromStart + openList[j]->estimatedCostToGoal;
                            openList[j]->parent = current;
                        }
                        break;
                    }
                }
                // Add the neighbor to the openList
                if (!inOpenList)
                {
                    Node *neighbor = malloc(sizeof(Node));
                    neighbor->x = nx;
                    neighbor->y = ny;
                    neighbor->costFromStart = newCostFromStart;
                    neighbor->estimatedCostToGoal = heuristic(nx, ny, endX, endY);
                    neighbor->totalEstimatedCost = neighbor->costFromStart + neighbor->estimatedCostToGoal;
                    neighbor->parent = current;
                    openList[openCount++] = neighbor;
                }
            }
        }
    }

    // Print the path
    if (found)
    {
        while (current)
        {
            maze[current->y][current->x] = 'o';
            current = current->parent;
        }
        printf("\n\n\n\n--------------SOLUTION FOUND--------------\n\n");
        for (int i = 0; i < height; i++)
        {
            printf("%s\n", maze[i]);
        }
    }
    else
    {
        printf("\n\n\n\n--------------no solution found--------------\n");
    }
    // Free memory
    for (int i = 0; i < openCount; i++)
    {
        free(openList[i]);
    }
    free(openList);

    for (int i = 0; i < height; i++)
    {
        free(closedList[i]);
    }
    free(closedList);
}

int main(int argc, char *argv[])
{

    // Load Maze
    if (!load_maze(argv[1]))
    {
        return 1;
    }

    printf("--------------Maze loaded successfully--------------\n");
    printf("--------------Finding Solution----------------------\n");


    clock_t start = clock();
    a_star(0, 0, width - 1, height - 1);

    for (int i = 0; i < height; i++)
    {
        free(maze[i]);
    }
    free(maze);

    clock_t end = clock();
    
    // Calculate the elapsed time in seconds
    double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\n\nSolution time: %f seconds\n", elapsed_time);


    return 0;
}
