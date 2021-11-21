#include "NPC.h"

void NPC::draw(sf::RenderTarget& t_target, sf::RenderStates t_states) const
{
	if (m_isVisible) t_target.draw(m_sprite, t_states);
}

void NPC::setPath(std::queue<Tile*> t_path)
{
	m_path = t_path;
}

void NPC::update()
{
	if (m_isVisible && !m_path.empty() && m_clock.getElapsedTime().asSeconds() > m_timeToWait.asSeconds())
	{
		m_clock.restart();
		sf::Vector2f newPosition = m_path.front()->getPosition();
		m_path.pop();
		m_sprite.setPosition(newPosition - sf::Vector2f(m_sprite.getGlobalBounds().width / 2.0f, m_sprite.getGlobalBounds().height / 2.0f));
	}
}

NPC::NPC() : m_isVisible{ false }, m_path{std::queue<Tile*>()}
{
	m_timeToWait = sf::seconds(0.5f);
	if (!m_texture.loadFromFile("ASSETS//IMAGES//NPC.png"))
	{
		std::cerr << "Error loading texture for npc" << std::endl;
	}
	m_sprite.setTexture(m_texture);
}
