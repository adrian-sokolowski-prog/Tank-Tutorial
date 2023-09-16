#include "Tank.h"

Tank::Tank(sf::Texture const & t_texture, std::vector<sf::Sprite>& t_wallSprites)
: m_texture(t_texture), m_wallSprites(t_wallSprites)
{
	initSprites(sf::Vector2f{0,0});
}
void Tank::update(double dt, std::function<void(int)>& t_funcApplyDamage, sf::Sprite t_tankBase)
{	
	if (m_hasFuel == true)
	{
		m_previousPosition = m_tankBase.getPosition();
		float x = m_tankBase.getPosition().x + cos(m_rotation * MathUtility::DEG_TO_RAD) * m_speed * (dt / 1000);
		float y = m_tankBase.getPosition().y + sin(m_rotation * MathUtility::DEG_TO_RAD) * m_speed * (dt / 1000);

		m_tankBase.setPosition(sf::Vector2f{ x,y });
		m_tankBase.setRotation(m_rotation);
		m_noise.setPosition(m_tankBase.getPosition());
		m_turret.setPosition(sf::Vector2f{ x,y });
		m_turret.setRotation(m_turretRotation);
		//m_speed = std::clamp(m_speed, MAX_REVERSE_SPEED, MAX_FORWARD_SPEED);
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
	m_pool.update(dt, m_wallSprites, t_funcApplyDamage, t_tankBase);
	if (checkWallCollision() == true)
	{
		deflect();
	}
}
void Tank::render(sf::RenderWindow & window) 
{
	window.draw(m_tankBase);
	window.draw(m_turret);
	m_pool.render(window);
}
void Tank::setPosition(sf::Vector2f t_pos)
{
	
	m_tankBase.setPosition(t_pos);
	m_turret.setPosition(t_pos);
}
void Tank::increaseSpeed()
{
	m_previousSpeed = m_speed;
	m_speed += 2;
	if (m_noise.getRadius() <= 100)
	{
		m_noise.setRadius(m_noise.getRadius() + m_noiseIncrement);
		m_noise.setOrigin(m_noise.getOrigin().x + m_noiseIncrement, m_noise.getOrigin().y + m_noiseIncrement);
	}
	
}
void Tank::decreaseSpeed()
{
	m_previousSpeed = m_speed;
	m_speed -= 2;
	if (m_noise.getRadius() <= 100)
	{
		m_noise.setRadius(m_noise.getRadius() + m_noiseIncrement);
		m_noise.setOrigin(m_noise.getOrigin().x + m_noiseIncrement, m_noise.getOrigin().y + m_noiseIncrement);
	}
}
void Tank::increaseRotation()
{
	m_previousTurretRotation = m_turretRotation;
	m_previousRotation = m_rotation;
	m_rotation += 1;
	if (m_centeringTurret == false)
	{

		m_turretRotation += 1;
	}
	if (m_rotation >= 360.0)
	{
		m_rotation = 0;
	}
	if (m_turretRotation >= 360.0)
	{
		m_turretRotation = 0;
	}
}
void Tank::decreaseRotation()
{
	m_previousTurretRotation = m_turretRotation;
	m_previousRotation = m_rotation;
	m_rotation -= 1;
	if (m_centeringTurret == false)
	{
		m_turretRotation -= 1;
	}
	if (m_rotation <= 0.0)
	{
		m_rotation = 359.0;
	}
	if (m_turretRotation <= 0.0)
	{
		m_turretRotation = 359.0;
	}
}
void Tank::friction()
{
	m_speed = m_speed * 0.99;
	if (m_speed < 0.5 && m_speed > -0.5)
	{
		m_speed = 0;
	}
	if (m_noise.getRadius() >= 50)
	{
		m_noise.setRadius(m_noise.getRadius() - m_noiseDecrement);
		m_noise.setOrigin(m_noise.getOrigin().x - m_noiseDecrement, m_noise.getOrigin().y - m_noiseDecrement);
	}
}
void Tank::handleKeyInput()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		increaseSpeed();
 	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		decreaseRotation();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		increaseRotation();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		decreaseSpeed();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
	{
		if (m_turretRotation != m_rotation)
		{
			m_centeringTurret = true;
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
	{

		m_centeringTurret = false;
		increaseTurretRotation();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
	{

		m_centeringTurret = false;
		decreaseTurretRotation();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		if (m_fireRequested == false)
		{

		requestFire();
		}
	}
}
void Tank::increaseTurretRotation()
{
	m_previousTurretRotation = m_turretRotation;
	m_turretRotation += 1;
	if (m_turretRotation >= 360)
	{
		m_turretRotation = 0;
	}
}
void Tank::decreaseTurretRotation()
{
	m_previousTurretRotation = m_turretRotation;
	m_turretRotation -= 1;
	if (m_turretRotation <= 0)
	{
		m_turretRotation = 359;
	}
}
void Tank::centreTurret()
{
	if (m_turretRotation == m_rotation)
	{
		m_centeringTurret = false;
		m_turretRotation = m_rotation;
	}
	else if (m_turretRotation != m_rotation)
	{
		if (m_turretRotation < m_rotation)
		{
			if (abs(m_turretRotation - m_rotation) < 180)
			{
				m_previousTurretRotation = m_turretRotation;
				m_turretRotation += 1;
				std::cout << "on + First \n";
			}
			else
			{
				m_previousTurretRotation = m_turretRotation;
				m_turretRotation -= 1;
				std::cout << "on - First \n";
				if (m_turretRotation <= 0)
				{
					m_previousTurretRotation = m_turretRotation;
					m_turretRotation = 359;
				}
			}
		}
		else
		{
			if (abs(m_turretRotation - m_rotation) < 180)
			{
				m_previousTurretRotation = m_turretRotation;
				m_turretRotation -= 1;
				std::cout << "on - second \n";
			}
			else
			{
				m_previousTurretRotation = m_turretRotation;
				m_turretRotation += 1;
				std::cout << "on + Second \n";
				if (m_turretRotation >= 360)
				{
					m_previousTurretRotation = m_turretRotation;
					m_turretRotation = 0;
				}
			}
		}
	}
}
bool Tank::isTankHit() const
{
	return m_tankHit;
}
sf::Vector2f Tank::getPosition() const
{
	return m_tankBase.getPosition();
}
sf::Sprite Tank::getTurret() const
{
	return  m_turret;
}
sf::Sprite Tank::getBase() const
{
	return m_tankBase;
}
sf::FloatRect Tank::getNoiseRect() const
{
	return m_noise.getGlobalBounds();
}
void Tank::applyDamage(int t_damageAmount)
{
	m_tankHealth -= t_damageAmount;
}
void Tank::initSprites(sf::Vector2f pos)
{
	// Initialise the tank base
	m_tankBase.setTexture(m_texture);
	sf::IntRect baseRect(2, 43, 79, 43);
	m_tankBase.setTextureRect(baseRect);
	m_tankBase.setOrigin(baseRect.width / 2.0, baseRect.height / 2.0);
	m_tankBase.setPosition(pos);
	m_tankBase.setScale(0.5, 0.5);
	// Initialise the turret
	m_turret.setTexture(m_texture);
	sf::IntRect turretRect(19, 1, 83, 31);
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(turretRect.width / 3.0, turretRect.height / 2.0);
	m_turret.setPosition(pos);
	m_turret.setScale(0.5, 0.5);
	// Init tank noise
	m_noise.setOrigin(50, 50);
	m_noise.setRadius(50);
	m_noise.setFillColor(sf::Color::Magenta);
}
bool Tank::checkWallCollision()
{
	{
		for (sf::Sprite const& sprite : m_wallSprites)
		{
			// Checks if either the tank base or turret has collided with the current wall sprite.
			if (CollisionDetector::collision(m_turret, sprite) ||
				CollisionDetector::collision(m_tankBase, sprite))
			{
				return true;
			}
		}
		if (m_tankBase.getPosition().x - 20 <= 0)
		{
			return true;
		}
		if (m_tankBase.getPosition().y - 15 <= 0)
		{
			return true;
		}
		if (m_tankBase.getPosition().x + 20 >= 1440)
		{
			return true;
		}
		if (m_tankBase.getPosition().y + 15 >= 900)
		{
			return true;
		}
		return false;
	}
}
/// <summary>
/// @brief Stops the tank if moving and applies a small increase in speed in the opposite direction of travel.
/// If the tank speed is currently 0, the rotation is set to a value that is less than the previous rotation value
///  (scenario: tank is stopped and rotates into a wall, so it gets rotated towards the opposite direction).
/// If the tank is moving, further rotations are disabled and the previous tank position is restored.
/// The tank speed is adjusted so that it will travel slowly in the opposite direction. The tank rotation 
///  is also adjusted as above if necessary (scenario: tank is both moving and rotating, upon wall collision it's 
///  speed is reversed but with a smaller magnitude, while it is rotated in the opposite direction of it's 
///  pre-collision rotation).
/// </summary>
void Tank::deflect()
{
	// In case tank was rotating.
	adjustRotation();
	// If tank was moving.
	if (m_speed != 0)
	{
		// Temporarily disable turret rotations on collision.
		m_centeringTurret = false;
		// Back up to position in previous frame.
		m_tankBase.setPosition(m_previousPosition);
		// Apply small force in opposite direction of travel.
		if (m_previousSpeed < 0)
		{
			m_speed = 8;
		}
		else
		{
			m_speed = -8;
		}
	}
}
void Tank::adjustRotation()
{
	// If tank was rotating...
	if (m_rotation != m_previousRotation)
	{
		// Work out which direction to rotate the tank base post-collision.
		if (m_rotation > m_previousRotation)
		{
			m_rotation = m_previousRotation - 1;
		}
		else
		{
			m_rotation = m_previousRotation + 1;
		}
	}
	// If turret was rotating while tank was moving
	if (m_turretRotation != m_previousTurretRotation)
	{
		// Set the turret rotation back to it's pre-collision value.
		m_turretRotation = m_previousTurretRotation;
	}
}
/// <summary>
/// fires the bullet from the tip of the turret
/// </summary>
void Tank::requestFire()
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
