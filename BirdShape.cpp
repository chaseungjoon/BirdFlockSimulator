#include "BirdShape.hpp"

BirdShape::BirdShape(float size)
        : m_size(size)
{
    setPointCount(3);
    updateShape();
    setOrigin({m_size * 0.5f, m_size * 0.75f});
}

FlockConfig BirdShape::config;

float BirdShape::getSize() const
{
    return m_size;
}

void BirdShape::updateShape()
{
    setPoint(0, { 0.f,  0.f });   // Left wing
    setPoint(1, { m_size * 0.5f, m_size * 1.5f }); // Bird tip
    setPoint(2, { m_size,  0.f });   // Right wing
}

void BirdShape::debugDraw(sf::RenderWindow& window) const
{
    sf::CircleShape neighborCircle(config.neighborDistance);
    neighborCircle.setOrigin({config.neighborDistance, config.neighborDistance});
    neighborCircle.setPosition(getPosition());
    neighborCircle.setFillColor(sf::Color::Transparent);
    neighborCircle.setOutlineColor(sf::Color(0, 255, 0, 50));
    neighborCircle.setOutlineThickness(1.f);
    window.draw(neighborCircle);
}

void BirdShape::setVelocity(const sf::Vector2f& velocity) {
    m_velocity = velocity;
}

sf::Vector2f BirdShape::getVelocity() const {
    return m_velocity;
}

void BirdShape::checkCollision(BirdShape& other)
{
    sf::Vector2f posA = getPosition();
    sf::Vector2f posB = other.getPosition();
    sf::Vector2f diff = posB - posA;
    float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    float minDistance = (m_size * .5f) + (other.getSize() * .5f);

    if (distance < minDistance && distance > 0.f) {
        sf::Vector2f velA = m_velocity;
        sf::Vector2f velB = other.getVelocity();

        sf::Vector2f normal = diff / distance;
        sf::Vector2f relativeVelocity = velA - velB;

        float velAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;

        if (velAlongNormal > 0.f) return;

        sf::Vector2f impulse = normal * velAlongNormal;
        m_velocity -= impulse;
        other.setVelocity(velB + impulse);

        float overlap = minDistance - distance;
        sf::Vector2f correction = normal * (overlap / 2.f);
        setPosition(posA - correction);
        other.setPosition(posB + correction);

    }
}

void BirdShape::checkOutOfBounds(const sf::FloatRect& bounds)
{
    if (m_velocity != sf::Vector2f{0.f, 0.f}) {
        float angle = std::atan2(m_velocity.y, m_velocity.x) * 180.f / M_PI;
        setRotation(sf::degrees(angle + 270.f)); // Adjust if bird nose points up
    }

    sf::Vector2f pos = getPosition();
    bool bouncedX = false;
    bool bouncedY = false;

    float minX = bounds.position.x;
    float maxX = bounds.position.x + bounds.size.x;
    float minY = bounds.position.y;
    float maxY = bounds.position.y + bounds.size.y;

    if (pos.x < minX) {
        pos.x = minX;
        m_velocity.x *= -1.f;
        bouncedX = true;
    }
    else if (pos.x > maxX) {
        pos.x = maxX;
        m_velocity.x *= -1.f;
        bouncedX = true;
    }

    if (pos.y < minY) {
        pos.y = minY;
        m_velocity.y *= -1.f;
        bouncedY = true;
    }
    else if (pos.y > maxY) {
        pos.y = maxY;
        m_velocity.y *= -1.f;
        bouncedY = true;
    }

    if (bouncedX || bouncedY) {
        pos.x += bouncedX ? m_velocity.x * 0.01f : 0.f;
        pos.y += bouncedY ? m_velocity.y * 0.01f : 0.f;
        setPosition(pos);
    }
}

void BirdShape::checkMaxSpeed()
{
    float speed = std::sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);
    if (speed > config.maxSpeed) {
        m_velocity = (m_velocity / speed) * config.maxSpeed;
    }
}

void BirdShape::applyNoise()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> noiseDist(config.randomness * -1.f, config.randomness);

    // Apply random noise to the velocity
    m_velocity.x += noiseDist(gen);
    m_velocity.y += noiseDist(gen);

    checkMaxSpeed();
}

void BirdShape::Alignment(const std::vector<BirdShape>& flock)
{
    /*
    A bird's "angle" is defined as atan(radians)
    1. Calculate the average angle of the flock
    2. Calculate the angle difference (average angle of flock - bird's angle)
    3. Add -> angle difference * alignment Strength (0~1)  to the bird's current angle (without changing bird's speed)
    */

    sf::Vector2f velocity = this->getVelocity();
    float birdAngle = std::atan2(velocity.y, velocity.x);
    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);

    int count = 0;
    float angleSum = 0.f;

    for (const auto& other : flock) {
        if (&other == this) continue;
        sf::Vector2f otherVel = other.getVelocity();
        angleSum += std::atan2(otherVel.y, otherVel.x);
        count++;
    }

    if (count > 0) {
        float avgAngle = angleSum / static_cast<float>(count);

        float newAngle = birdAngle + (avgAngle - birdAngle) * config.alignmentStrength;

        m_velocity.x = std::cos(newAngle) * speed;
        m_velocity.y = std::sin(newAngle) * speed;
    }
}

void BirdShape::Cohesion(const std::vector<BirdShape>& flock)
{
    /*
    1. Find the flock's center mass
    2. Steer vector -> center mass - current position
    3. Add steer vector * cohesion Strength (0~1) to current vector
    */

    sf::Vector2f centerMass{0.f, 0.f};
    int count = 0;

    for (const auto& other : flock){
        if (&other == this) continue;
        centerMass += other.getPosition();
        count++;
    }
    if (count > 0) {
        centerMass /= static_cast<float>(count);
    }

    sf::Vector2f toCenter = centerMass - getPosition();
    float distance = std::sqrt(toCenter.x * toCenter.x + toCenter.y * toCenter.y);

    if (distance > 0.f) {
        toCenter /= distance;
        m_velocity += toCenter * config.cohesionStrength;
    }
}

void BirdShape::Separation(const std::vector<BirdShape>& flock)
{
    /*
    1. If other come inside desired separation radius, repulse.
    2. Repulse vector -> bird's position - other's position
    3. Normalize repulse vector and add it accordingly
    */
    float desiredSeparation = config.desiredSeparation;
    float separationStrength = config.separationStrength;
    float maxSeparationForce = config.maxSeparationForce;

    sf::Vector2f steer{0.f, 0.f};
    int count = 0;

    for (const auto& other : flock) {
        if (&other == this) continue;

        sf::Vector2f diff = getPosition() - other.getPosition();
        float distSquared = diff.x * diff.x + diff.y * diff.y;

        if (distSquared > 0.f && distSquared < desiredSeparation * desiredSeparation) {
            sf::Vector2f away = diff / distSquared;
            steer += away;
            count++;
        }
    }

    if (count > 0) {
        steer /= static_cast<float>(count);

        float len = std::sqrt(steer.x * steer.x + steer.y * steer.y);
        if (len > 0.f) {
            steer = (steer / len) * std::min(separationStrength * len, maxSeparationForce);
            m_velocity += steer;
        }
    }
}

void BirdShape::update(const sf::FloatRect& bounds,std::vector<BirdShape>& flock) {

    // Apply 3 rules of Boid
    Separation(flock);
    Cohesion(flock);
    Alignment(flock);

    if (config.addNoise) applyNoise();
    else checkMaxSpeed();

    move(m_velocity);

    checkMaxSpeed();
    checkOutOfBounds(bounds);

    // Check collision with birds
    for (auto& other : flock) {
        if (&other != this) checkCollision(other);
    }
}
