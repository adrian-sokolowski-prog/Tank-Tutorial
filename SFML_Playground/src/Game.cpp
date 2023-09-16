#include "Game.h"
#include <iostream>

// Our target FPS
static double const FPS{ 60.0f };

////////////////////////////////////////////////////////////
Game::Game()
	: m_window(sf::VideoMode(ScreenSize::s_width, ScreenSize::s_height, 32), "SFML Playground", sf::Style::Default)
	, m_tank(m_texture,m_wallSprites) , m_aiTank(m_texture, m_wallSprites)
	, m_hud(m_font)
{
	init();
	m_aiTank.init(m_level.m_aiTank.m_position);
	if (!m_font.loadFromFile("./resources/fonts/BebasNeue.otf"))
	{
		std::string s("Error loading font");
		throw std::exception(s.c_str());
	}
	// Point at TankAI::applyDamage()...this function expects 1 argument (damage amount), but that argument
		//  will be supplied later when we call the function inside Projectile::udpate()
		// So we use what is called a placeholder argument and this will be substituted later with the damage amount
	using std::placeholders::_1;
	// The parameters of bind are the function to be called, followed by the address of the target instance, 
	//  followed by the placeholder argument.
	m_funcApplyDamage = std::bind(&TankAi::applyDamage, &m_aiTank, _1);
	m_funcApplyDamage = std::bind(&Tank::applyDamage, &m_tank, _1);
}

////////////////////////////////////////////////////////////
void Game::init()
{
	int currentLevel = 1;
	// generates exception on loading failure
	try
	{
		LevelLoader::load(currentLevel, m_level);
	}
	catch (std::exception& e)
	{
		std::cout << "Level loading failure" << "\n";
		std::cout << e.what() << std::endl;
		throw e;
	}
	if (!m_bgTexture.loadFromFile(m_level.m_background.m_fileName))
	{
		std::cout << "Error loading background file";
	}
	m_bgSprite.setTexture(m_bgTexture);
	m_bgSprite.setPosition(sf::Vector2f{ 0.0f,0.0f });
	// Really only necessary is our target FPS is greater than 60.
	m_window.setVerticalSyncEnabled(true);
	
	if (!m_arialFont.loadFromFile("./resources/images/BebasNeue.otf"))
	{
		std::cout << "Error loading font file";
	}
#ifdef TEST_FPS
	x_updateFPS.setFont(m_arialFont);
	x_updateFPS.setPosition(20, 300);
	x_updateFPS.setCharacterSize(24);
	x_updateFPS.setFillColor(sf::Color::White);
	x_drawFPS.setFont(m_arialFont);
	x_drawFPS.setPosition(20, 350);
	x_drawFPS.setCharacterSize(24);
	x_drawFPS.setFillColor(sf::Color::White);
#endif
	if (!m_texture.loadFromFile("./resources/images/SpriteSheet.png"))
	{
		std::string errorMsg("Error loading texture");
		throw std::exception(errorMsg.c_str());
	}

	m_tank.setPosition(m_level.m_tank.m_position);
	generateWalls();
}
////////////////////////////////////////////////////////////
void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	
	sf::Time timePerFrame = sf::seconds(1.0f / FPS); // 60 fps
	while (m_window.isOpen())
	{
		processEvents(); // as many as possible
		m_tank.update(timePerFrame.asMilliseconds(),m_funcApplyDamage, m_aiTank.getTankBase());
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); // at least 60 fps
			update(timePerFrame.asMilliseconds()); //60 fps
#ifdef TEST_FPS
			x_secondTime += timePerFrame;
			x_updateFrameCount++;
			if (x_secondTime.asSeconds() > 1)
			{
				std::string updatesPS = "UPS " + std::to_string(x_updateFrameCount - 1);
				x_updateFPS.setString(updatesPS);
				std::string drawsPS = "DPS " + std::to_string(x_drawFrameCount);
				x_drawFPS.setString(drawsPS);
				x_updateFrameCount = 0;
				x_drawFrameCount = 0;
				x_secondTime = sf::Time::Zero;
			}
#endif
		}
		render(); // as many as possible
#ifdef TEST_FPS
		x_drawFrameCount++;
#endif
	}
}
////////////////////////////////////////////////////////////
void Game::processEvents()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_window.close();
		}
		processGameEvents(event);
	}
}
////////////////////////////////////////////////////////////
void Game::processGameEvents(sf::Event& event)
{
	// check if the event is a a mouse button release
	if (sf::Event::KeyPressed == event.type)
	{
		switch (event.key.code)
		{
		case sf::Keyboard::Escape:
			m_window.close();
			break;
		
		default:
			break;
		}
	}
}
////////////////////////////////////////////////////////////
void Game::generateWalls()
{
	sf::IntRect wallRect(2, 129, 33, 23);
	// Create the walls
	for (auto const& obstacle : m_level.m_obstacles)
	{
		sf::Sprite sprite;
		sprite.setTexture(m_texture);
		sprite.setTextureRect(wallRect);
		sprite.setOrigin(wallRect.width / 2.0, wallRect.height / 2.0);
		sprite.setPosition(obstacle.m_position);
		sprite.setRotation(obstacle.m_rotation);
		m_wallSprites.push_back(sprite);
	}
}
////////////////////////////////////////////////////////////
void Game::update(double dt)
{
	
	switch (m_gameState)
	{
	case GameState::GAME_RUNNING:

		m_tank.friction();
		m_tank.handleKeyInput();
		m_hasCollided -= dt;
		std::cout << m_tank.m_tankHealth << std::endl;
		if (m_aiTank.collidesWithPlayer(m_tank)&& m_hasCollided <= 0)
		{
			m_tank.m_tankHealth -= 1;
			m_hasCollided = 2000;

		}
		m_hud.update(m_gameState, m_tank);
		m_hud.playerCanMove(m_tank);
		m_hud.hudCollidesWithPlayer(m_tank);
		if (m_tank.m_centeringTurret == true)
		{
			m_tank.centreTurret();
		}
		m_aiTank.update(m_tank, dt, m_funcApplyDamage);
		if (m_tank.m_tankHealth <= 0)
		{
			m_gameState = GameState::GAME_LOSE;
			
		}
		
		break;
	case GameState::GAME_WIN:
		
		break;
	case GameState::GAME_LOSE:
		timeState += dt;
		if (timeState >= 10000)
		{
			m_gameState = GameState::GAME_RUNNING;
			timeState = 0.0;
		}
		break;
	default:
		break;
	}
	
	//std::cout << m_tank.centeringTurret << "\n";
}
////////////////////////////////////////////////////////////
void Game::render()
{
	m_window.clear(sf::Color(0, 0, 0, 0));
#ifdef TEST_FPS
	m_window.draw(x_updateFPS);
	m_window.draw(x_drawFPS);
#endif
	// Render your sprites here....
	m_window.draw(m_bgSprite);
	//m_window.draw(m_tank.m_noise);
	m_window.draw(m_aiTank.visionCone);
	// Range based loop
	for (sf::Sprite sprite : m_wallSprites)
	{
		m_window.draw(sprite);
	}
	m_tank.render(m_window);
	m_aiTank.render(m_window);
	m_hud.render(m_window);
	m_window.display();
}





