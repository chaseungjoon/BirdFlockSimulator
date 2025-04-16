#pragma once

#include <SFML/Graphics.hpp>
#include <random>

struct FlockConfig {
    float neighborDistance = 50.f;
    float alignmentStrength = 0.3f;
    float cohesionStrength = 0.05f;
    float separationStrength = 50.f;
    float desiredSeparation = 70.f;
    float maxSeparationForce = 70.f;
    float maxSpeed = 0.5f;
    bool addNoise = true;
    float randomness = 0.03f;
};

class BirdShape : public sf::ConvexShape {
public:
    static FlockConfig config;

    explicit BirdShape(float size);
    float getSize() const;

    void setVelocity(const sf::Vector2f& velocity);
    sf::Vector2f getVelocity() const;

    void applyNoise();
    void debugDraw(sf::RenderWindow& window) const;
    void checkCollision(BirdShape& other);
    void checkOutOfBounds(const sf::FloatRect& bounds);
    void checkMaxSpeed();
    void update(const sf::FloatRect & bounds,std::vector<BirdShape>& flock);

    void Alignment(const std::vector<BirdShape>& flock);
    void Cohesion(const std::vector<BirdShape>& flock);
    void Separation(const std::vector<BirdShape>& flock);

private:
    float m_size;
    void updateShape();
    sf::Vector2f m_velocity;
};
