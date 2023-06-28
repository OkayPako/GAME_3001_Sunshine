#pragma once
#include "TileMap.h"
#include <unordered_map>

class Pathfinder
{
private:
	Tilemap* map = nullptr;

public:
	std::unordered_map<TileCoord, float, std::hash<TileCoord>, std::equal_to<TileCoord>> unvisited;

	std::unordered_map<TileCoord, float, std::hash<TileCoord>, std::equal_to<TileCoord>> visited;

	std::unordered_map<TileCoord, TileCoord, std::hash<TileCoord>, std::equal_to<TileCoord>> cheapestEdgeTo;

	TileCoord startNode;
	TileCoord goalNode;
	TileCoord currentNode;

	Pathfinder()
	{

	};

	Pathfinder(Tilemap* levelToNagivate, TileCoord startTile, TileCoord endTile)
	{
		map = levelToNagivate;
		startNode = startTile;
		goalNode = endTile;
		currentNode = startNode;
		Restart();
	}

	void Restart()
	{
		visited.clear();
		cheapestEdgeTo.clear();
		unvisited.clear();

		for (TileCoord position : map->GetAllTraversableTiles())
		{
			unvisited[position] = INFINITY;
		}
		unvisited[startNode] = 0;
	}

	void SetEndNode(TileCoord endTile)
	{
		goalNode = endTile;
	}

	std::pair<TileCoord, float> GetLowestCostIn(std::unordered_map<TileCoord, float> set)
	{
		TileCoord cheapestPosition = { -1,-1 };
		float lowestCost = INFINITY;
		for (auto nodeValuePair : set)
		{
			TileCoord tile = nodeValuePair.first;
			float cost = nodeValuePair.second;;

			if (cost < lowestCost)
			{
				cheapestPosition = tile;
				lowestCost = cost;
			}
		}
		return { cheapestPosition, lowestCost };
	}

	bool IsVisited(TileCoord pos) const { return visited.count(pos); }
	bool IsSolved() const { return IsVisited(goalNode); }
	bool IsCompleted() { return IsVisited(goalNode) || GetLowestCostIn(unvisited).second == INFINITY; }

	float GetTotalCostToReach(TileCoord pos) { return unvisited[pos]; }
	void SetCostToReach(TileCoord pos, float newCost) { unvisited[pos] = newCost; }

	void ProcessNextIterationFunctional()
	{
		if (IsCompleted()) return;
		currentNode = GetLowestCostIn(unvisited).first;

		for (auto adjacent : map->GetConnectedTraversableTiles(currentNode))
		{
			if (IsVisited(adjacent)) continue;

			float costThisWay = GetTotalCostToReach(currentNode) + map->GetTileTraversalCost(adjacent);

			float oldCost = GetTotalCostToReach(adjacent);
			if (costThisWay < oldCost)
			{
				SetCostToReach(adjacent, costThisWay);
				cheapestEdgeTo[adjacent] = currentNode;
			}
		}

		MoveToVistedSet(currentNode);
	}

	void MoveToVistedSet(TileCoord node)
	{
		visited[currentNode] = unvisited[currentNode];
		unvisited.erase(currentNode);
	}

	bool SolvePath() // Runs the full algorithm until completion and returns whether or not it was succesful
	{
		Restart();
		while (!IsCompleted())
		{
			ProcessNextIterationFunctional();
		}
		return IsSolved();
	}
};