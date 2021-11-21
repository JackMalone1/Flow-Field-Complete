#pragma once

#include <SFML/Graphics.hpp>
#include <queue>
#include "Tile.h"

class NPC : public sf::Drawable
{
private:
	sf::Sprite m_sprite;
	sf::Texture m_texture;
	bool m_isVisible;
	std::queue<Tile*> m_path;
	sf::Clock m_clock;
	sf::Time m_timeToWait;
private:
	virtual void draw(sf::RenderTarget& t_target, sf::RenderStates t_states) const;
public:
	void setPath(std::queue<Tile*> t_path);
	void setVisible(bool t_isVisible)
	{
		m_isVisible = t_isVisible;
		m_clock.restart();
	}
	bool isVisible()
	{
		return m_isVisible;
	}
	void update();
	NPC();
};

