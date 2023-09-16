#pragma once
#include "MathUtility.h"
#include "Tank.h"
#include <SFML/Graphics.hpp>
#include <Thor/Vectors.hpp>
#include <Thor/Math.hpp>
#include <iostream>
#include <queue>
#include "CollisionDetector.h"
#include <functional>
#include "ProjectilePool.h"
class TankAi
{
public:
	/// <summary>
	/// @brief Constructor that stores a reference to the obstacle container.
	/// Initialises steering behaviour to seek (player) mode, sets the AI tank position and
	///  initialises the steering vector to (0,0) meaning zero force magnitude.
	/// </summary>
	/// <param name="texture">A reference to the sprite sheet texture</param>
	///< param name="wallSprites">A reference to the container of wall sprites</param>
	TankAi(sf::Texture const & texture, std::vector<sf::Sprite> & wallSprites);

	/// <summary>
	/// @brief Steers the AI tank towards the player tank avoiding obstacles along the way.
	/// Gets a vector to the player tank and sets steering and velocity vectors towards
	/// the player if current behaviour is seek. If behaviour is stop, the velocity vector
	/// is set to 0. Then compute the correct rotation angle to point towards the player tank. 
	/// If the distance to the player tank is < MAX_SEE_AHEAD, then the behaviour is changed from seek to stop.
	/// Finally, recalculate the new position of the tank base and turret sprites.
	/// </summary>
	/// <param name="playerTank">A reference to the player tank</param>
	/// <param name="dt">update delta time</param>
	void update(Tank const & playerTank, double dt, std::function<void(int)>& t_funcApplyDamage);

	/// <summary>
	/// @brief Draws the tank base and turret.
	///
	/// </summary>
	/// <param name="window">The SFML Render window</param>
	void render(sf::RenderWindow & window);

	/// <summary>
	/// @brief Initialises the obstacle container and sets the tank base/turret sprites to the specified position.
	/// <param name="position">An x,y position</param>
	/// </summary>
	void init(sf::Vector2f position);

	/// <summary>
	/// @brief Checks for collision between the AI and player tanks.
	///
	/// </summary>
	/// <param name="player">The player tank instance</param>
	/// <returns>True if collision detected between AI and player tanks.</returns>
	bool collidesWithPlayer(Tank const& playerTank) const;
	// base tank health
	int m_tankHealth = 1;
	// applies damage when tank is shot
	void applyDamage(int t_damageAmount);
	// provides the base of the tank
	sf::Sprite getTankBase();
	// the convex shape making up the vision cone
	sf::ConvexShape visionCone;
private:
	void initSprites();

	void updateMovement(double dt);

	sf::Vector2f seek(sf::Vector2f playerPosition) const;

	sf::Vector2f collisionAvoidance();

	bool inDeadZone(sf::CircleShape m_obstacle);
	const sf::CircleShape findMostThreateningObstacle();

	// A reference to the sprite sheet texture.
	sf::Texture const & m_texture;

	// A sprite for the tank base.
	sf::Sprite m_tankBase;

	// A sprite for the turret
	sf::Sprite m_turret;

	// A reference to the container of wall sprites.
	std::vector<sf::Sprite> & m_wallSprites;

	// The current rotation as applied to tank base and turret.
	double m_rotation{ 0.0 };

	// Current velocity.
	sf::Vector2f m_velocity;

	// Steering vector.
	sf::Vector2f m_steering;

	// The ahead vector.
	sf::Vector2f m_ahead;

	// The half-ahead vector.
	sf::Vector2f m_halfAhead;

	// The maximum see ahead range.
	static float constexpr MAX_SEE_AHEAD{ 50.0f };

	// The maximum avoidance turn rate.
	static float constexpr MAX_AVOID_FORCE{ 50.0f };

	// max steering force
	static float constexpr MAX_FORCE{ 10.0f };

	// The maximum speed for this tank.
	float MAX_SPEED = 100.0f;
	static int const MAX_POINTS = 10;
	// Area of patrol
	sf::Vector2f patrolPoints[MAX_POINTS]
	{
		{800,200},
		{200,200},
		{800,800},
		{800,200},
		{450,450},
		{200,600},
		{700,200},
		{150,200},
		{600,700},
		{150,700}
	};
	// where the ai is currently patroling towards
	int currentPoint = 0;
	// A container of circles that represent the obstacles to avoid.
	std::vector<sf::CircleShape> m_obstacles;
	/// <summary>
	/// The behaviour states in which the ai tank is currently in
	/// </summary>
	enum class AIState
	{
		PATROL_MAP,
		PLAYER_DETECTED,
		ATTACK_PLAYER
	} m_aiBehaviour;
	// checks if the behaviour of the ai tank has changed since the last update
	AIState m_lastBehaviour;
	// max length of time between states reverting back to patrol mode if player didnt move within the vision cone
	const int MAX_TIME{ 2000 };
	// length of time since the last time the player moved within the vision cone
	double m_timeBetweenStates = MAX_TIME;

	/// <summary>
	/// determines if the ai tank should fire
	/// </summary>
	void requestFire();
	// the bullet
	ProjectilePool m_pool;
	// length of time between the ai tank firing 
	static int const s_TIME_BETWEEN_SHOTS{ 2000 };
	bool m_fireRequested = false;
	// how long since the last time the ai tank fired
	double m_shootTimer = s_TIME_BETWEEN_SHOTS;
	/// <summary>
	/// Starting size of the Patrol Vision cone
	/// </summary>
	sf::Vector2f m_modeOnePointOne{ 400,-100 };
	sf::Vector2f m_modeOnePointTwo{ 400,100 };
	/// <summary>
	/// Starting size of the Player detected vision cone
	/// </summary>
	sf::Vector2f m_modeTwoPointOne{ 600,-50 };
	sf::Vector2f m_modeTwoPointTwo{ 600,50};
	/// <summary>
	/// starting size of the attack vision cone
	/// </summary>
	sf::Vector2f m_modeThreePointOne{ 200,-200 };
	sf::Vector2f m_modeThreePointTwo{ 200,200 };

	/// <summary>
	/// the points which make up the vision cone convex shape
	/// </summary>
	sf::Vector2f m_pointOne = m_turret.getPosition();
	sf::Vector2f m_pointTwo = m_turret.getPosition() + m_modeOnePointOne;
	sf::Vector2f m_pointThree = m_turret.getPosition() + m_modeOnePointTwo;
	/// <summary>
	/// Updates the vision cone position and checks if the player is within the cone
	/// </summary>
	/// <param name="playerTank">player tank</param>
	void updateCone(Tank const& playerTank);
	/// <summary>
	/// Checks if the player is on the left or on the right of the vision cone, if not, then they are inside
	/// </summary>
	/// <param name="t_linePoint1"> left side vision cone vector</param>
	/// <param name="t_linePoint2">right side vision cone vector</param>
	/// <param name="t_point"> player tanks position</param>
	/// <returns></returns>
	bool isLeft(sf::Vector2f t_linePoint1, sf::Vector2f t_linePoint2, sf::Vector2f t_point) const;
	bool isRight(sf::Vector2f t_linePoint1, sf::Vector2f t_linePoint2, sf::Vector2f t_point) const;
	
	/// <summary>
	/// time between the ai moving against after getting shot
	/// </summary>
	int const REPAIR_TIME{ 10000 };
	double m_repairingTime = REPAIR_TIME;
};

