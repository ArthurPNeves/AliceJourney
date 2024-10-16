


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
    if (!file)
    {
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

// Insert a node into the priority queue (min-heap)
void insert_into_queue(Node **queue, int *queue_size, Node *new_node)
{
    queue[*queue_size] = new_node;
    int i = (*queue_size)++;
    while (i > 0)
    {
        int parent = (i - 1) / 2;
        if (queue[parent]->totalEstimatedCost <= queue[i]->totalEstimatedCost)
        {
            break;
        }
        Node *temp = queue[i];
        queue[i] = queue[parent];
        queue[parent] = temp;
        i = parent;
    }
}

// Remove the node with the smallest totalEstimatedCost from the priority queue
Node *remove_from_queue(Node **queue, int *queue_size)
{
    Node *min_node = queue[0];
    queue[0] = queue[--(*queue_size)];
    int i = 0;
    while (i * 2 + 1 < *queue_size)
    {
        int left = i * 2 + 1;
        int right = left + 1;
        int smallest = (right < *queue_size && queue[right]->totalEstimatedCost < queue[left]->totalEstimatedCost) ? right : left;
        if (queue[i]->totalEstimatedCost <= queue[smallest]->totalEstimatedCost)
        {
            break;
        }
        Node *temp = queue[i];
        queue[i] = queue[smallest];
        queue[smallest] = temp;
        i = smallest;
    }
    return min_node;
}

void a_star(int startX, int startY, int endX, int endY)
{
    // Initialize openList and closedList
    Node **openList = malloc(width * height * sizeof(Node *));
    if (!openList)
    {
        printf("Error allocating memory for openList");
        return;
    }
    int openCount = 0;

    bool **closedList = malloc(height * sizeof(bool *));
    for (int i = 0; i < height; i++)
    {
        closedList[i] = calloc(width, sizeof(bool));
    }

// Initialize the start node and insert it into the openList
    Node *start = malloc(sizeof(Node));
    start->x = startX;
    start->y = startY;
    start->costFromStart = 0;
    start->estimatedCostToGoal = heuristic(startX, startY, endX, endY);
    start->totalEstimatedCost = start->costFromStart + start->estimatedCostToGoal;
    start->parent = NULL;
    insert_into_queue(openList, &openCount, start);

    Node *current = NULL;
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};
    bool found = false;

    while (openCount > 0)
    {

        // Remove the node with the smallest totalEstimatedCost from the openList
        current = remove_from_queue(openList, &openCount);

// Check if the current node is the goal node
        if (current->x == endX && current->y == endY)
        {
            found = true;
            break;
        }

        closedList[current->y][current->x] = true;

    // Expand the current node
        for (int i = 0; i < 4; i++)
        {
            int nx = current->x + dx[i];
            int ny = current->y + dy[i];

            if (is_valid(nx, ny) && !closedList[ny][nx])
            {
                int newCostFromStart = current->costFromStart + 1;

                // Check if the neighbor node is in the openList
                bool inOpenList = false;
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

                // If the neighbor node is not in the openList, add it to the openList
                if (!inOpenList)
                {
                    Node *neighbor = malloc(sizeof(Node));
                    neighbor->x = nx;
                    neighbor->y = ny;
                    neighbor->costFromStart = newCostFromStart;
                    neighbor->estimatedCostToGoal = heuristic(nx, ny, endX, endY);
                    neighbor->totalEstimatedCost = neighbor->costFromStart + neighbor->estimatedCostToGoal;
                    neighbor->parent = current;
                    insert_into_queue(openList, &openCount, neighbor);
                }
            }
        }
    }

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
        printf("No solution found\n");
    }

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

    printf("--------------Maze %dx%d loaded successfully--------------\n", width, height);
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
