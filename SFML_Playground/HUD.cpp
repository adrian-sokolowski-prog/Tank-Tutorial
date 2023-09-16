#include "HUD.h"

////////////////////////////////////////////////////////////
HUD::HUD(sf::Font& hudFont)
    : m_textFont(hudFont)
{
    m_gameStateText.setFont(hudFont);
    m_gameStateText.setCharacterSize(30);
    m_gameStateText.setFillColor(sf::Color::Blue);
    m_gameStateText.setPosition(sf::Vector2f(480, 5));
    m_gameStateText.setString("Game Running");

    //Setting up our hud properties 
    m_hudOutline.setSize(sf::Vector2f(1440.0f, 40.0f));
    m_hudOutline.setFillColor(sf::Color(0, 0, 0, 38));
    m_hudOutline.setOutlineThickness(-.5f);
    m_hudOutline.setOutlineColor(sf::Color(0, 0, 0, 100));
    m_hudOutline.setPosition(0, 0);
    
    // Sprites for collectables
    if (!m_jerryCanTexture.loadFromFile("./resources/images/JerryCan.png"))
    {
        std::cout << "jerrycan texture error\n";
    }
    else
    {
        m_jerryCan.setTexture(m_jerryCanTexture);
        m_jerryCan.setPosition(700, 400);
        m_jerryCan.setScale(0.1f, 0.1f);
    }
    if (!m_toolBoxTexture.loadFromFile("./resources/images/ToolBox.png"))
    {
        std::cout << "toolbox texture error\n";
    }
    else
    {
        m_toolBox.setTexture(m_toolBoxTexture);
        m_toolBox.setPosition(400, 400);
        m_toolBox.setScale(0.1f, 0.1f);
    }
    // Setting up Player Damage/Fuel info
    // fuel info
    m_fuelContainer.setSize(sf::Vector2f(60.0f, 10.0f));
    m_fuelContainer.setFillColor(sf::Color(0, 0, 0, 100));
    m_fuelContainer.setPosition(200, 200);
    m_fuelAmount.setSize(sf::Vector2f(30.0f, 10.0f));
    m_fuelAmount.setFillColor(sf::Color(0, 255, 0));
    m_fuelAmount.setPosition(200, 200);
    // damage info
    m_damageContainer.setSize(sf::Vector2f(60.0f, 10.0f));
    m_damageContainer.setFillColor(sf::Color(0, 0, 0, 100));
    m_damageContainer.setPosition(200, 200);
    m_damageAmount.setSize(sf::Vector2f(30.0f, 10.0f));
    m_damageAmount.setFillColor(sf::Color(150, 0, 0));
    m_damageAmount.setPosition(200, 200);
}

////////////////////////////////////////////////////////////
void HUD::update(GameState const& gameState, Tank const& playerTank)
{
    switch (gameState)
    {
    case GameState::GAME_RUNNING:
        m_gameStateText.setString("Game Running");
        positionPlayerInfoHud(playerTank.getPosition());
        currentPlayerPos = playerTank.getPosition();
        hasPlayerUsedFuel(playerTank.isTankHit());
        if (m_currentTankHP > playerTank.m_tankHealth)
        {
            sf::Vector2f newDamage{ m_damageAmount.getSize().x - MAX_HEALTH.x / 4, m_damageAmount.getSize().y };
            m_damageAmount.setSize(newDamage);
        }
            m_currentTankHP = playerTank.m_tankHealth;
        break;
    case GameState::GAME_WIN:
        m_gameStateText.setString("You Won");
        break;
    case GameState::GAME_LOSE:
        m_gameStateText.setString("You Lost");
        break;
    default:
        break;
    }
}

void HUD::hudCollidesWithPlayer(Tank& playerTank)
{
    // Checks if the JerryCan has collided with the player tank.
    if (CollisionDetector::collision(m_jerryCan, playerTank.getTurret()) ||
        CollisionDetector::collision(m_jerryCan, playerTank.getBase()))
    {
        m_fuelAmount.setSize(MAX_FUEL);
        m_jerryCan.setPosition(-1000, -1000);
        drawJerryCan = false;
    }
    // Checks if the ToolBox has collided with the player tank.
    if (CollisionDetector::collision(m_toolBox, playerTank.getTurret()) ||
        CollisionDetector::collision(m_toolBox, playerTank.getBase()))
    {
        m_damageAmount.setSize(MAX_HEALTH);
        playerTank.m_tankHealth = 4;
        m_toolBox.setPosition(-1000, -1000);
        drawToolBox = false;
        playerFixed = true;
    }

}


void HUD::render(sf::RenderWindow& window)
{
    window.draw(m_hudOutline);
    window.draw(m_gameStateText);
    if (drawJerryCan)
    {
    window.draw(m_jerryCan);
    }
    if (drawToolBox)
    {
    window.draw(m_toolBox);
    }
    window.draw(m_fuelContainer);
    window.draw(m_fuelAmount);
    window.draw(m_damageContainer);
    window.draw(m_damageAmount);
}

void HUD::playerCanMove(Tank& playerTank)
{
    if (m_fuelAmount.getSize().x > 0)
    {
        playerTank.m_hasFuel = true;
    }
    else
    {
        playerTank.m_hasFuel = false;
    }
    if (playerFixed == true)
    {
        playerTank.m_tankHit = false;
        playerFixed = false;
    }
}

void HUD::positionPlayerInfoHud(sf::Vector2f t_playerPosition)
{
    m_fuelAmount.setPosition(t_playerPosition.x - 30.0f, t_playerPosition.y - 50.0f);
    m_fuelContainer.setPosition(t_playerPosition.x - 30.0f, t_playerPosition.y - 50.0f);

    m_damageAmount.setPosition(t_playerPosition.x - 30.0f, t_playerPosition.y - 65.0f);
    m_damageContainer.setPosition(t_playerPosition.x - 30.0f, t_playerPosition.y - 65.0f);
}


void HUD::hasPlayerUsedFuel(bool t_isTankHit)
{
    if (lastPlayerPos != currentPlayerPos)
    {
        fuelBurnt -= 1;
        if (t_isTankHit == true)
        {
            fuelBurnt -= 1;
           
        }
       
    }
    if (fuelBurnt == -100)
    {
        sf::Vector2f newFuel{ m_fuelAmount.getSize().x - 1, m_fuelAmount.getSize().y };
        m_fuelAmount.setSize(newFuel);
        fuelBurnt = 0;
        std::cout << "fuel deplete\n";
    }
    lastPlayerPos = currentPlayerPos;
}
