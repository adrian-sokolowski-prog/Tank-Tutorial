#include "TankAi.h"

////////////////////////////////////////////////////////////
TankAi::TankAi(sf::Texture const & texture, std::vector<sf::Sprite> & wallSprites)
	: m_aiBehaviour(AIState::PATROL_MAP)
	, m_texture(texture)
	, m_wallSprites(wallSprites)
	, m_steering(0, 0)
{
	// Initialises the tank base and turret sprites.
	initSprites();
}

////////////////////////////////////////////////////////////
void TankAi::update(Tank const & playerTank, double dt, std::function<void(int)>& t_funcApplyDamage)
{
	m_pool.update(dt, m_wallSprites, t_funcApplyDamage, playerTank.getBase());
	if (m_tankHealth >= 0)
	{
		// updates ai movement
		updateMovement(dt);
		updateCone(playerTank);
		sf::Vector2f vectorToPlayer = seek(playerTank.getPosition());
		sf::Vector2f vectorToPoint = seek(patrolPoints[currentPoint]);
		// if behaviour changed, puts the time between behaviours at max
		if (m_lastBehaviour != m_aiBehaviour)
		{
			m_timeBetweenStates = MAX_TIME;
		}
		m_timeBetweenStates -= dt;
		m_lastBehaviour = m_aiBehaviour;
		switch (m_aiBehaviour)
		{
		case AIState::PATROL_MAP:
			// checks if ai tank sees the player tank
			if (!isLeft(m_pointTwo, m_pointOne, playerTank.getPosition()) && !isRight(m_pointThree, m_pointOne, playerTank.getPosition()) /* || playerTank.m_noise.getGlobalBounds().intersects(m_tankBase.getGlobalBounds()*/)
			{
				std::cout << "patrol change" << std::endl;
				m_aiBehaviour = AIState::PLAYER_DETECTED;
				break;
			}
			// automatic ai tank movement
			m_steering += thor::unitVector(vectorToPoint);
			m_steering += collisionAvoidance();
			m_steering = MathUtility::truncate(m_steering, MAX_FORCE);
			m_velocity = MathUtility::truncate(m_velocity + m_steering, MAX_SPEED);
			if ((patrolPoints[currentPoint].x + MAX_SEE_AHEAD >= m_tankBase.getPosition().x && patrolPoints[currentPoint].x - MAX_SEE_AHEAD <= m_tankBase.getPosition().x)
				&& patrolPoints[currentPoint].y + MAX_SEE_AHEAD >= m_tankBase.getPosition().y && patrolPoints[currentPoint].y - MAX_SEE_AHEAD <= m_tankBase.getPosition().y)
			{
				if (currentPoint < MAX_POINTS)
				{
					currentPoint += 1;
				}
				else
				{
					currentPoint = 0;
				}
			}
			break;
		case AIState::PLAYER_DETECTED:
			// checks the distance between the player and ai tank, before changin modes
			double distance;
			// checks if ai tank sees the player tank
			if (!isLeft(m_pointTwo, m_pointOne, playerTank.getPosition()) && !isRight(m_pointThree, m_pointOne, playerTank.getPosition()))
			{
				distance = MathUtility::distance(m_pointOne, playerTank.getPosition());
				std::cout << distance << std::endl;
				if (200 >= distance && -200 <= distance)
				{
					m_aiBehaviour = AIState::ATTACK_PLAYER;
					m_timeBetweenStates = MAX_TIME;
					break;
				}
			}
			// automatic ai tank movement
			m_steering += thor::unitVector(vectorToPlayer);
			m_steering += collisionAvoidance();
			m_steering = MathUtility::truncate(m_steering, MAX_FORCE);
			m_velocity = MathUtility::truncate(m_velocity + m_steering, MAX_SPEED);
			if (m_timeBetweenStates <= 0)
			{
				m_aiBehaviour = AIState::PATROL_MAP;
				m_timeBetweenStates = MAX_TIME;
			}
			break;
		case AIState::ATTACK_PLAYER:
			// checks if ai tank sees the player tank
			if (!isLeft(m_pointTwo, m_pointOne, playerTank.getPosition()) && !isRight(m_pointThree, m_pointOne, playerTank.getPosition()))
			{
				m_timeBetweenStates = MAX_TIME;
				// determines if the ai tank should fire their bullet
				if (m_fireRequested == false)
				{
					requestFire();
				}
				if (m_fireRequested == true)
				{
					m_shootTimer -= dt;
					if (m_shootTimer <= 0)
					{
						m_fireRequested = false;
						m_shootTimer = s_TIME_BETWEEN_SHOTS;
					}
				}
			}
			// automatic ai tank movement
			m_steering += thor::unitVector(vectorToPlayer);
			m_steering += collisionAvoidance();
			m_steering = MathUtility::truncate(m_steering, MAX_FORCE);
			m_velocity = MathUtility::truncate(m_velocity + m_steering, MAX_SPEED);
			if (m_timeBetweenStates <= 0)
			{
				m_aiBehaviour = AIState::PLAYER_DETECTED;
				m_timeBetweenStates = MAX_TIME;
			}

			break;
		default:
			break;

		}
	}
	else
	{
	m_repairingTime -= dt;
		if (m_repairingTime <= 0)
		{
			m_tankHealth = 1;
			m_repairingTime = REPAIR_TIME;
		}
	}

	// Now we need to convert our velocity vector into a rotation angle between 0 and 359 degrees.
	// The m_velocity vector works like this: vector(1,0) is 0 degrees, while vector(0, 1) is 90 degrees.
	// So for example, 223 degrees would be a clockwise offset from 0 degrees (i.e. along x axis).
	// Note: we add 180 degrees below to convert the final angle into a range 0 to 359 instead of -PI to +PI
	auto dest = atan2(-1 * m_velocity.y, -1 * m_velocity.x) / thor::Pi * 180 + 180;

	auto currentRotation = m_rotation;

	// Find the shortest way to rotate towards the player (clockwise or anti-clockwise)
	if (std::round(currentRotation - dest) == 0.0)
	{
		m_steering.x = 0;
		m_steering.y = 0;
	}
	else if ((static_cast<int>(std::round(dest - currentRotation + 360))) % 360 < 180)
	{
		// rotate clockwise
		m_rotation = static_cast<int>((m_rotation) + 1) % 360;
		
	}
	else
	{
		// rotate anti-clockwise
		m_rotation = static_cast<int>((m_rotation) - 1) % 360;
		
	}
	
}

////////////////////////////////////////////////////////////
void TankAi::render(sf::RenderWindow & window)
{
	// TODO: Don't draw if off-screen...
	
	window.draw(m_tankBase);
	window.draw(m_turret);
	m_pool.render(window);
}

////////////////////////////////////////////////////////////
void TankAi::init(sf::Vector2f position)
{
	m_tankBase.setPosition(position);
	m_turret.setPosition(position);

	for (sf::Sprite const wallSprite : m_wallSprites)
	{
		sf::CircleShape circle(wallSprite.getTextureRect().width * 1.5f);
		circle.setOrigin(circle.getRadius(), circle.getRadius());
		circle.setPosition(wallSprite.getPosition());
		m_obstacles.push_back(circle);
	}
}

bool TankAi::collidesWithPlayer(Tank const& playerTank) const
{
	// Checks if the AI tank has collided with the player tank.
	if (CollisionDetector::collision(m_turret, playerTank.getTurret()) ||
		CollisionDetector::collision(m_tankBase, playerTank.getBase()))
	{
		return true;
	}
	return false;
	
}

////////////////////////////////////////////////////////////
sf::Vector2f TankAi::seek(sf::Vector2f playerPosition) const
{
	return playerPosition - m_tankBase.getPosition();
}

////////////////////////////////////////////////////////////
sf::Vector2f TankAi::collisionAvoidance()
{
	auto headingRadians = thor::toRadian(m_rotation);
	sf::Vector2f headingVector(std::cos(headingRadians) * MAX_SEE_AHEAD, std::sin(headingRadians) * MAX_SEE_AHEAD);
	m_ahead = m_tankBase.getPosition() + headingVector;
	m_halfAhead = m_tankBase.getPosition() + (headingVector * 0.5f);
	const sf::CircleShape mostThreatening = findMostThreateningObstacle();
	sf::Vector2f avoidance(0, 0);
	// if there is an obstacle AND we are not in the deadzone
	if (mostThreatening.getRadius() != 0.0 && !inDeadZone(mostThreatening))
	{		
		avoidance.x = m_ahead.x - mostThreatening.getPosition().x;
		avoidance.y = m_ahead.y - mostThreatening.getPosition().y;
		avoidance = thor::unitVector(avoidance);
		avoidance *= MAX_AVOID_FORCE;
		
	}
	else
	{
		avoidance *= 0.0f;
		
	}
	
	return avoidance;
}

bool TankAi::inDeadZone(sf::CircleShape m_obstacle)
{
	double dist = MathUtility::distance(m_ahead, m_obstacle.getPosition());
	double inner = m_obstacle.getRadius() * 0.90;
	double outer = m_obstacle.getRadius() * 1.10;
	if (dist >= inner && dist <= outer)
	{
		return true;
	}
	else
	{
		return false;
	}
}

////////////////////////////////////////////////////////////
const sf::CircleShape TankAi::findMostThreateningObstacle()
{
	sf::CircleShape mostThreatening;
	/// <summary>
	/// checks if each obstacle is within range of the ai tank vision, if yes return which obsticle
	/// </summary>
	/// <returns></returns>
	for (size_t i = 0; i < m_obstacles.size(); i++)
	{
		if (MathUtility::lineIntersectsCircle(m_ahead, m_halfAhead, m_obstacles[i]))
		{
			mostThreatening = m_obstacles[i];
			return mostThreatening;
		}
	};
	return mostThreatening;
}
/// <summary>
/// fires the bullet from the tip of the turret
/// </summary>
void TankAi::requestFire()
{
	m_fireRequested = true;
	if (m_shootTimer == s_TIME_BETWEEN_SHOTS)
	{
		sf::Vector2f tipOfTurret(m_turret.getPosition().x + 2.0f, m_turret.getPosition().y);
		tipOfTurret.x += std::cos(MathUtility::DEG_TO_RAD * m_turret.getRotation()) * ((m_turret.getLocalBounds().top + m_turret.getLocalBounds().height) * 1.7f);
		tipOfTurret.y += std::sin(MathUtility::DEG_TO_RAD * m_turret.getRotation()) * ((m_turret.getLocalBounds().top + m_turret.getLocalBounds().height) * 1.7f);
		m_pool.create(m_texture, tipOfTurret.x, tipOfTurret.y, m_turret.getRotation());

	}
}

void TankAi::updateCone(Tank const& playerTank)
{
	sf::Vector2f secondPoint; // position to which the second point in the convex shape will move to
	sf::Vector2f thirdPoint; // position to which the third point in the convex shape will move to
	m_pointOne = m_turret.getPosition();
	// rotating the points of the convex shape to align with the turret for each ai behaviour
	switch (m_aiBehaviour)
	{	
	case TankAi::AIState::PATROL_MAP:

		 
		secondPoint.x = (m_modeOnePointOne.x * std::cos(MathUtility::DEG_TO_RAD * m_rotation)) - (m_modeOnePointOne.y * std::sin(MathUtility::DEG_TO_RAD * m_rotation));
		secondPoint.y = (m_modeOnePointOne.x * std::sin(MathUtility::DEG_TO_RAD * m_rotation)) + (m_modeOnePointOne.y * std::cos(MathUtility::DEG_TO_RAD * m_rotation));

		thirdPoint.x = (m_modeOnePointTwo.x * std::cos(MathUtility::DEG_TO_RAD * m_rotation)) - (m_modeOnePointTwo.y * std::sin(MathUtility::DEG_TO_RAD * m_rotation));
		thirdPoint.y = (m_modeOnePointTwo.x * std::sin(MathUtility::DEG_TO_RAD * m_rotation)) + (m_modeOnePointTwo.y * std::cos(MathUtility::DEG_TO_RAD * m_rotation));
					  
		
		m_pointTwo = m_turret.getPosition() + secondPoint;
		m_pointThree = m_turret.getPosition() + thirdPoint;

		visionCone.setPoint(0, m_pointOne);
		visionCone.setPoint(1, m_pointTwo);
		visionCone.setPoint(2, m_pointThree);
		
		break;
	case TankAi::AIState::PLAYER_DETECTED:
		secondPoint.x = (m_modeTwoPointOne.x * std::cos(MathUtility::DEG_TO_RAD * m_rotation)) - (m_modeTwoPointOne.y * std::sin(MathUtility::DEG_TO_RAD * m_rotation));
		secondPoint.y = (m_modeTwoPointOne.x * std::sin(MathUtility::DEG_TO_RAD * m_rotation)) + (m_modeTwoPointOne.y * std::cos(MathUtility::DEG_TO_RAD * m_rotation));
					 
		thirdPoint.x = (m_modeTwoPointTwo.x * std::cos(MathUtility::DEG_TO_RAD * m_rotation)) - (m_modeTwoPointTwo.y * std::sin(MathUtility::DEG_TO_RAD * m_rotation));
		thirdPoint.y = (m_modeTwoPointTwo.x * std::sin(MathUtility::DEG_TO_RAD * m_rotation)) + (m_modeTwoPointTwo.y * std::cos(MathUtility::DEG_TO_RAD * m_rotation));

		m_pointTwo = m_turret.getPosition() + secondPoint;
		m_pointThree = m_turret.getPosition() + thirdPoint;

		visionCone.setPoint(0, m_pointOne);
		visionCone.setPoint(1, m_pointTwo);
		visionCone.setPoint(2, m_pointThree);
		break;
	case TankAi::AIState::ATTACK_PLAYER:
		secondPoint.x = (m_modeThreePointOne.x * std::cos(MathUtility::DEG_TO_RAD * m_rotation)) - (m_modeThreePointOne.y * std::sin(MathUtility::DEG_TO_RAD * m_rotation));
		secondPoint.y = (m_modeThreePointOne.x * std::sin(MathUtility::DEG_TO_RAD * m_rotation)) + (m_modeThreePointOne.y * std::cos(MathUtility::DEG_TO_RAD * m_rotation));

		thirdPoint.x = (m_modeThreePointTwo.x * std::cos(MathUtility::DEG_TO_RAD * m_rotation)) - (m_modeThreePointTwo.y * std::sin(MathUtility::DEG_TO_RAD * m_rotation));
		thirdPoint.y = (m_modeThreePointTwo.x * std::sin(MathUtility::DEG_TO_RAD * m_rotation)) + (m_modeThreePointTwo.y * std::cos(MathUtility::DEG_TO_RAD * m_rotation));

		m_pointTwo = m_turret.getPosition() + secondPoint;
		m_pointThree = m_turret.getPosition() + thirdPoint;

		visionCone.setPoint(0, m_pointOne);
		visionCone.setPoint(1, m_pointTwo);
		visionCone.setPoint(2, m_pointThree);
		break;
	default:
		break;
	}
	
}
// is the player to the left
bool TankAi::isLeft(sf::Vector2f t_linePoint1, sf::Vector2f t_linePoint2, sf::Vector2f t_point) const
{
	// return ( (x2-x1) * (y3-y1) ) - ( (y2 - y1) * (x3-x1) )
	return ((t_linePoint2.x - t_linePoint1.x) *
		(t_point.y - t_linePoint1.y) -
		(t_linePoint2.y - t_linePoint1.y) *
		(t_point.x - t_linePoint1.x)) > 0;
}
// is the player to the right
bool TankAi::isRight(sf::Vector2f t_linePoint1, sf::Vector2f t_linePoint2, sf::Vector2f t_point) const
{
	// return ( (x2-x1) * (y3-y1) ) - ( (y2 - y1) * (x3-x1) )
	return ((t_linePoint2.x - t_linePoint1.x) *
		(t_point.y - t_linePoint1.y) -
		(t_linePoint2.y - t_linePoint1.y) *
		(t_point.x - t_linePoint1.x)) < 0;
}

void TankAi::applyDamage(int t_damageAmount)
{
	m_tankHealth -= t_damageAmount;
}

sf::Sprite TankAi::getTankBase()
{
	return m_tankBase;
}

////////////////////////////////////////////////////////////
void TankAi::initSprites()
{
	// Initialise the tank base
	m_tankBase.setTexture(m_texture);
	sf::IntRect baseRect(103, 43, 79, 43);
	m_tankBase.setTextureRect(baseRect);
	m_tankBase.setOrigin(baseRect.width / 2.0, baseRect.height / 2.0);
	m_tankBase.setScale(0.5, 0.5);
	// Initialise the turret
	m_turret.setTexture(m_texture);
	sf::IntRect turretRect(122, 1, 83, 31);
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(turretRect.width / 3.0, turretRect.height / 2.0);
	m_turret.setScale(0.5, 0.5);

	visionCone.setPointCount(3);
	visionCone.setFillColor(sf::Color::Blue);
	visionCone.setPoint(0, m_pointOne);
	visionCone.setPoint(1, m_pointTwo);
	visionCone.setPoint(2, m_pointThree);
}


////////////////////////////////////////////////////////////
void TankAi::updateMovement(double dt)
{
	double speed = thor::length(m_velocity);
	sf::Vector2f newPos(m_tankBase.getPosition().x + std::cos(MathUtility::DEG_TO_RAD  * m_rotation) * speed * (dt / 1000),
		m_tankBase.getPosition().y + std::sin(MathUtility::DEG_TO_RAD  * m_rotation) * speed * (dt / 1000));
	m_tankBase.setPosition(newPos.x, newPos.y);
	m_tankBase.setRotation(m_rotation);
	m_turret.setPosition(m_tankBase.getPosition());
	m_turret.setRotation(m_rotation);
	
}