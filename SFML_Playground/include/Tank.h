#pragma once
#include <SFML/Graphics.hpp>
#include "MathUtility.h"
#include <iostream>
#include "CollisionDetector.h"
#include "ProjectilePool.h"
#include <functional>
/// <summary>
/// @brief A simple tank controller.
/// 
/// This class will manage all tank movement and rotations.
/// </summary>
class Tank
{
public:	
	//Tank() = default;
	Tank(sf::Texture const & t_texture, std::vector<sf::Sprite>& t_wallSprites);
	void update(double dt, std::function<void(int)>& t_funcApplyDamage, sf::Sprite t_tankBase);
	void render(sf::RenderWindow & window);
	void setPosition(sf::Vector2f t_pos);
	void increaseSpeed();
	void decreaseSpeed();
	void increaseRotation();
	void decreaseRotation();
	void friction();
	void handleKeyInput();
	void increaseTurretRotation();
	void decreaseTurretRotation();
	void centreTurret();
	bool isTankHit() const;
	sf::Vector2f getPosition() const;
	bool m_centeringTurret{ false };
	sf::Sprite  getTurret() const;
	sf::Sprite  getBase() const;
	sf::FloatRect getNoiseRect() const;
	void applyDamage(int t_damageAmount);
	bool m_hasFuel = true;
	bool m_tankHit = false;
	int m_tankHealth = 2;
	
	sf::CircleShape m_noise;
private:
	void initSprites(sf::Vector2f pos);
	bool checkWallCollision();
	void deflect();
	void adjustRotation();
	sf::Sprite m_tankBase;
	sf::Sprite m_turret;
	sf::Texture const & m_texture;
	double m_speed{ 0.0 };
	double m_rotation{ 0.0 };
	const double MAX_FORWARD_SPEED{ 100 };
	const double MAX_REVERSE_SPEED{ -100 };
	double m_turretRotation{ 0.0 };
	std::vector<sf::Sprite>& m_wallSprites;
	bool m_enableRotation = true;
	sf::Vector2f m_previousPosition{ 0,0 };
	double m_previousSpeed{ 0.0 };
	double m_previousRotation{ 0.0 };
	double m_previousTurretRotation{ 0.0 };
	void requestFire();
	ProjectilePool m_pool;
	static int const s_TIME_BETWEEN_SHOTS{ 2000 };
	bool m_fireRequested = false;
	double m_shootTimer = s_TIME_BETWEEN_SHOTS;
	double m_noiseIncrement = 0.5;
	double m_noiseDecrement = 0.25;
	
};
