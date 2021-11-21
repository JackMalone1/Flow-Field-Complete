#include "FlowFieldGraph.h"

void FlowFieldGraph::checkTileMouseClick(bool t_isLeftMouseClick, sf::Vector2i t_mousePosition)
{
	for (auto& row : m_tiles)
	{
		for (auto& tile : row)
		{
			if (tile->intersectsPoint(static_cast<sf::Vector2f>(t_mousePosition)) && tile->isTraversable())
			{
				handleMouseClick(tile, t_isLeftMouseClick);
				return;//no need to continue check if you clicked on a point after finding a tile that you clicked on
			}
		}
	}
}

FlowFieldGraph::FlowFieldGraph(sf::Font& t_font, sf::RenderWindow& t_window) : m_renderWindow{ t_window }, m_font{t_font}
{
	int y = -5;
	int x = 15;
	int id = 0;
	sf::Color colour = sf::Color(0,0,255,125);
	for (int row = 0; row < m_tiles.size(); row++)
	{
		y += 20;
		x = 15;
		for (int col = 0; col < m_tiles.at(row).size(); col++)
		{
			auto& tile = m_tiles.at(row).at(col);
			x += 20;
			tile = new Tile(900000, new sf::Vector2f(0.0f, 0.0f), sf::Vector2f(x, y), 20.0f, 20.0f, m_font, colour, row, col);
			id++;
			tile->setId(id);
		}
	}
}

void FlowFieldGraph::generateCostsForTiles()
{
	for (auto& row : m_tiles)
	{
		for (auto& tile : row)
		{
			tile->setMarked(false);
			if (tile->isTraversable())
			{
				tile->setCost(std::numeric_limits<float>::max() - 1);
			}
			else
			{
				tile->setCost(std::numeric_limits<float>::max());
			}
		}
	}
	m_goalNode->setCost(0);
	m_goalNode->setIntegrationCost(0.0f);
	m_goalNode->setBestNeighbour(m_goalNode->getRowAndCol());
	m_goalNode->setMarked(true);
	sf::Vector2i goalPos = m_goalNode->getRowAndCol();
	std::queue<Tile*> costSetupQueue;
	costSetupQueue.push(m_goalNode);
	while (!costSetupQueue.empty())
	{
		Tile* tile = costSetupQueue.front();
		costSetupQueue.pop();
		generateTileCostWithNeighbour(tile->getRowAndCol().x, tile->getRowAndCol().y, costSetupQueue);
	}
	m_goalNode->setCost(0);
}

void FlowFieldGraph::generateTileCostWithNeighbour(int t_row, int t_col, std::queue<Tile*>& tileQueue)
{
	for (int direction = 0; direction < 9; direction++)
	{
		if (direction == 4) continue;
		int row = t_row + ((direction % 3) - 1);
		int col = t_col + ((direction / 3) - 1);
		if (row >= 0 && row < NUM_ROWS && col >= 0 && col < NUM_COLS)
		{

			if (m_tiles.at(row).at(col)->getMarked() || !m_tiles.at(row).at(col)->isTraversable()) continue;
			Tile* tile = m_tiles.at(row).at(col);
			m_tiles.at(row).at(col)->setCost(m_tiles.at(t_row).at(t_col)->getCost() + 1);
			m_tiles.at(row).at(col)->setMarked(true);
			float distX = static_cast<float>(m_goalNode->getPosition().x) - tile->getPosition().x;
			float distY = static_cast<float>(m_goalNode->getPosition().y) - tile->getPosition().y;
			float distance = sqrt(distX * distX + distY * distY);
			tile->setIntegrationCost((tile->getCost() * 100) +
				distance);
			tileQueue.push(m_tiles.at(row).at(col));
		}
	}
}

void FlowFieldGraph::generateVectorFieldWithNeighbour(int t_row, int t_col)
{
	Tile& tile = *m_tiles.at(t_row).at(t_col);
	float lowestIntegrationCost = std::numeric_limits<float>::max();
	sf::Vector2i rowAndCol = tile.getRowAndCol();
	for (int direction = 0; direction < 9; direction++)
	{
		if (direction == 4) continue;
		int row = t_row + ((direction / 3) - 1);
		int col = t_col + ((direction % 3) - 1);
		if (row >= 0 && row < NUM_ROWS && col >= 0 && col < NUM_COLS)
		{
			float integrationCost = m_tiles.at(row).at(col)->getIntegrationCost();
			if (integrationCost < lowestIntegrationCost && m_tiles.at(row).at(col)->isTraversable())
			{
				lowestIntegrationCost = integrationCost;
				rowAndCol = sf::Vector2i(row, col);
			}
		}
	}
	tile.setBestNeighbour(rowAndCol);
	tile.setVectorField(new sf::Vector2f(m_tiles.at(rowAndCol.x).at(rowAndCol.y)->getPosition()));
}

void FlowFieldGraph::generateVectorField()
{
	m_goalNode->setVectorField(new sf::Vector2f(0.0f, 0.0f));
	m_goalNode->setMarked(true);
	sf::Vector2i goalPos = m_goalNode->getRowAndCol();

	for (auto& row : m_tiles)
	{
		for (auto& tile : row)
		{
			if (tile->isTraversable())
			{
				generateVectorFieldWithNeighbour(tile->getRowAndCol().x, tile->getRowAndCol().y);
			}			
		}
	}
}

void FlowFieldGraph::generatePathTowardsGoal()
{
	std::queue<Tile*> empty;
	tiles.swap(empty);
	tiles.push(m_startNode);
	int tilesChecked = 0;
	bool goalFound = false;
	while (!tiles.back()->isGoalNode() && tilesChecked < 25000)
	{
		tiles.push(m_tiles.at(tiles.back()->getBestNeighbour().x).at(tiles.back()->getBestNeighbour().y));
		tiles.back()->setColour(sf::Color::Yellow);
		tilesChecked++;
	}
	if (tiles.back()->isGoalNode())
	{
		goalFound = true;
		m_npc.setPath(tiles);
	}
	if (!goalFound)
	{
		m_npc.setPath(empty);
		for (auto& row : m_tiles)
		{
			for (auto& tile : row)
			{
				tile->noPathColour();	
			}
		}
	}
}

void FlowFieldGraph::render()
{
	for (auto& row : m_tiles)
	{
		for (auto& tile : row)
		{
			m_renderWindow.draw(*tile);
		}
	}
	m_renderWindow.draw(m_npc);
}

void FlowFieldGraph::updateTilesCostDisplay()
{
	for (auto& row : m_tiles)
	{
		for (auto& tile : row)
		{
			tile->setShouldDisplayCost(!tile->shouldDisplayCost());
		}
	}
}

void FlowFieldGraph::updateVectorFieldDisplay()
{
	for (auto& row : m_tiles)
	{
		for (auto& tile : row)
		{
			tile->setDisplayVectorField(!tile->shouldDisplayVectorField());
		}
	}
}


void FlowFieldGraph::handleMouseClick(Tile*& tile, bool t_isLeftClick)
{
	std::cout << "Intersecting with tile: " << tile->getID() << std::endl;
	if (t_isLeftClick)
	{
		if (!tile->isGoalNode() && !tile->isStartNode())
		{
			tile->setIsGoalNode(true);
			tile->setIsStartNode(false);
			if (m_goalNode) m_goalNode->setIsGoalNode(false);
			m_goalNode = tile;
			generateCostsForTiles();
			generateVectorField();			
			if (m_goalNode && m_startNode) generatePathTowardsGoal();
		}
	}
	else
	{
		if (!tile->isGoalNode() && !tile->isStartNode())
		{
			tile->setIsStartNode(true);
			tile->setIsGoalNode(false);
			if (m_startNode) m_startNode->setIsStartNode(false);
			m_startNode = tile;
			if (m_goalNode)
			{
				generateCostsForTiles();
				generateVectorField();
				if (m_goalNode && m_startNode) generatePathTowardsGoal();
			}
		}
	}
	
}

void FlowFieldGraph::placeObstructedTIle(sf::Vector2f t_mousePosition)
{
	for (auto& row : m_tiles)
	{
		for (auto& tile : row)
		{
			if (tile->intersectsPoint(t_mousePosition) && !tile->isGoalNode() && !tile->isStartNode())
			{		
				tile->setTraversable(!tile->isTraversable());
				if (m_goalNode)
				{
					generateCostsForTiles();
					generateVectorField();
					if (m_startNode)
					{
						generatePathTowardsGoal();
					}				
				}
				return;
			}
		}
	}	
}

void FlowFieldGraph::update()
{
	m_npc.update();
}

void FlowFieldGraph::displayNPC()
{
	m_npc.setVisible(!m_npc.isVisible());
}

