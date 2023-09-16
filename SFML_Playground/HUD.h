#pragma once

#include <SFML/Graphics.hpp>
#include "GameState.h"
#include <iostream>
#include "CollisionDetector.h"
#include "Tank.h"
/// <summary>
/// @brief A basic HUD implementation.
/// 
/// Shows current game state only.
/// </summary>

class HUD
{
public:
    /// <summary>
    /// @brief Default constructor that stores a font for the HUD and initialises the general HUD appearance.
    /// </summary>
    HUD(sf::Font& hudFont);

    /// <summary>
    /// @brief Checks the current game state and sets the appropriate status text on the HUD.
    /// </summary>
    /// <param name="gameState">The current game state</param>
    void update(GameState const& gameState, Tank const& playerTank);
    /// <summary>
    /// When player and a jerry can or toolbox collide, player damage or fuel gets replenished
    /// </summary>
    /// <param name="t_player">player sprite</param>
    void HUD::hudCollidesWithPlayer(Tank& playerTank);
    /// <summary>
    /// @brief Draws the HUD outline and text.
    ///
    /// </summary>
    /// <param name="window">The SFML Render window</param>
    void render(sf::RenderWindow& window);
    void playerCanMove(Tank& playerTank);
    // Fuel / Damage Hud amounts
    sf::RectangleShape m_fuelAmount;
    sf::RectangleShape m_damageAmount;
private:

    void positionPlayerInfoHud(sf::Vector2f t_playerPosition);
    // The font for this HUD.
    sf::Font m_textFont;

    // A container for the current HUD text.
    sf::Text m_gameStateText;
    int m_currentTankHP = 2;
    // Collectable Sprites
    //Jerry Can Info
    sf::Sprite m_jerryCan;
    sf::Texture m_jerryCanTexture;
    const sf::Vector2f MAX_FUEL{ 60.0f,10.0f };
    bool drawJerryCan = true;
    // Tool Box Info
    const sf::Vector2f MAX_HEALTH{ 60.0f,10.0f };
    sf::Sprite m_toolBox;
    sf::Texture m_toolBoxTexture;
    bool drawToolBox = true;

    // Fuel / Damage Hud Containers
    sf::RectangleShape m_fuelContainer;
    sf::RectangleShape m_damageContainer;
    
    // if player is moving
    sf::Vector2f lastPlayerPos{0,0};
    sf::Vector2f currentPlayerPos = lastPlayerPos;
    void hasPlayerUsedFuel(bool t_isTankHit);
    bool playerFixed = false;
    int fuelBurnt = 0;
    // A simple background shape for the HUD.
    sf::RectangleShape m_hudOutline;
};