#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>
#include <sstream>
#include "Tilemap.h"

struct Node
{
    TileCoord coordinate;
    int cost;

    Node() : cost(0) {}
    Node(TileCoord coord, int c) : coordinate(coord), cost(c) {}
};

class Pathfinder
{
private:
    TileCoord startNode;
    TileCoord endNode;

    std::unordered_map<TileCoord, int, std::hash<TileCoord>> costs; // Total cost to reach each node
    std::unordered_map<TileCoord, TileCoord, std::hash<TileCoord>> prevNodes; // Best previous node for each node
    std::unordered_map<TileCoord, bool, std::hash<TileCoord>> visited; // Visited status of each node

    std::priority_queue<Node, std::vector<Node>, std::function<bool(const Node&, const Node&)>> unvisitedNodes; // Nodes in the unvisited set

    Node lowestCostNode; // Variable to store the lowest cost node

public:
    Pathfinder()
    {
        // Initialize start and end nodes to invalid positions (-1, -1)
        startNode = { -1, -1 };
        endNode = { -1, -1 };
    }

    // Set the start and goal positions for the pathfinding algorithm
    void SetStartAndGoal(const TileCoord& start, const TileCoord& goal)
    {
        startNode = start;
        endNode = goal;
    }

    // Restart the pathfinding process from the beginning
    // Clear data structures and set the start node's cost to 0
    void Restart()
    {
        costs.clear();
        prevNodes.clear();
        visited.clear();

        while (!unvisitedNodes.empty())
        {
            unvisitedNodes.pop();
        }

        costs[startNode] = 0;
        unvisitedNodes.push(Node(startNode, 0));
    }

    // Find the lowest cost node from the unvisited set and set it as the current node
    void FindLowestCostNode()
    {
        if (!unvisitedNodes.empty())
        {
            lowestCostNode = unvisitedNodes.top();
            unvisitedNodes.pop();

            TileCoord currentNode = lowestCostNode.coordinate;

            // Set the current node as visited
            visited[currentNode] = true;
        }
    }

    // Perform one step of the pathfinding algorithm
    void Step()
    {
        if (!unvisitedNodes.empty())
        {
            // Find the lowest cost node from the unvisited set and set it as the current node
            FindLowestCostNode();
            TileCoord currentNode = lowestCostNode.coordinate;

            // Set the current node as visited
            visited[currentNode] = true;

            // Termination condition A: Check if the goal node is in the visited set
            if (currentNode == endNode)
            {
                // Algorithm ends, success!
                return;
            }

            // Process neighbors of the current node
            ProcessNeighbors(currentNode);
        }

        // Termination condition B: Check if the lowest cost node in the unvisited set has a cost of Infinity
        if (unvisitedNodes.empty() || lowestCostNode.cost == INFINITY)
        {
            // Algorithm ends, failure!
            return;
        }
    }

    // Draw debug information on the screen based on the state of the pathfinding algorithm
    void DrawDebugInformation(Tilemap& tilemap)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            for (int y = 0; y < MAP_HEIGHT; y++)
            {
                TileCoord currentCoord(x, y);
                Color textColor = WHITE;

                if (currentCoord == startNode)
                {
                    textColor = GREEN;
                }
                else if (currentCoord == endNode)
                {
                    textColor = RED;
                }

                std::stringstream ss;
                ss << costs[currentCoord];

                DrawText(ss.str().c_str(), x * tilemap.tileSizeX + 10, y * tilemap.tileSizeY + 10, 20, textColor);
            }
        }
    }

private:
    // Process the neighbors of a given node
    void ProcessNeighbors(const TileCoord& currentNode)
    {
        // Get the neighboring tile coordinates
        std::vector<TileCoord> neighbors = GetNeighbors(currentNode);

        // Iterate through the neighbors
        for (const TileCoord& neighbor : neighbors)
        {
            // Skip if the neighbor is already visited
            if (visited[neighbor])
            {
                continue;
            }

            // Calculate the cost to reach the neighbor from the current node
            int newCost = costs[currentNode] + 1;

            // If the neighbor is not in the unvisited set, or the new cost is lower than the existing cost
            if (!unvisitedNodesContains(neighbor) || newCost < costs[neighbor])
            {
                // Update the cost and best previous node for the neighbor
                costs[neighbor] = newCost;
                prevNodes[neighbor] = currentNode;

                // Add the neighbor to the unvisited set
                unvisitedNodes.push(Node(neighbor, newCost));
            }
        }
    }

    // Helper function to get neighboring tile coordinates
    std::vector<TileCoord> GetNeighbors(const TileCoord& coordinate)
    {
        std::vector<TileCoord> neighbors;
        // Add logic to retrieve the neighboring tile coordinates based on the coordinate
        int x = coordinate.x;
        int y = coordinate.y;

        // Add the adjacent tiles (up, down, left, right) to the neighbors vector
        if (x - 1 >= 0) {
            neighbors.push_back(TileCoord(x - 1, y));
        }
        if (x + 1 < MAP_WIDTH) {
            neighbors.push_back(TileCoord(x + 1, y));
        }
        if (y - 1 >= 0) {
            neighbors.push_back(TileCoord(x, y - 1));
        }
        if (y + 1 < MAP_HEIGHT) {
            neighbors.push_back(TileCoord(x, y + 1));
        }

        return neighbors;
    }

    // Helper function to calculate the cost between two tile coordinates
    int GetCost(const TileCoord& source, const TileCoord& destination)
    {
        // Add logic to calculate the cost between the source and destination coordinates
        return 1; // Assuming a constant cost of 1 for simplicity
    }

    // Helper function to check if the unvisited set contains a specific tile coordinate
    bool unvisitedNodesContains(const TileCoord& coordinate)
    {
        // Return true if the coordinate is found in the unvisitedNodes, false otherwise
        return costs.find(coordinate) != costs.end();
    }
};
