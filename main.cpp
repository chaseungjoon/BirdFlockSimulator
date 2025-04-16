#include <vector>
#include <random>
#include <SFML/Graphics.hpp>
#include "BirdShape.hpp"

const int windowX = 1400.f;
const int windowY = 800.f;

const bool debugDrawNeighbors = false;

std::vector<BirdShape> flock;
const int flockSize = 80;

const float avgBirdVel = 0.8f;
const float avgBirdSize = 20.f;

void initializeFlock(int flock_size)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDistX(0.f, (float)windowX);
    std::uniform_real_distribution<float> posDistY(0.f, (float)windowY);
    std::uniform_real_distribution<float> angleDist(0.f, 2.f * M_PI);
    std::uniform_real_distribution<float> sizeDist(avgBirdSize-5.f, avgBirdSize+5.f);

    flock.clear();

    for (int i = 0; i < flock_size; ++i) {
        BirdShape bird(sizeDist(gen));
        float angle = angleDist(gen);
        bird.setVelocity({std::cos(angle) * avgBirdVel, std::sin(angle) * avgBirdVel});
        bird.setPosition({posDistX(gen), posDistY(gen)});
        bird.setFillColor(sf::Color::Yellow);
        flock.push_back(bird);
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({windowX, windowY}), "BirdFlockSimulator");

    initializeFlock(flockSize);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::Black);

        for (auto& bird : flock) {
            bird.update(sf::FloatRect({0.f, 0.f}, {windowX, windowY}), flock);
            window.draw(bird);

            if (debugDrawNeighbors) bird.debugDraw(window);
        }
        window.display();
    }
    return 0;
}
