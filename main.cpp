#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <entityx/entityx.h>
#include <sstream>

namespace ex = entityx;

inline float r(int a, float b = 0) { return static_cast<float>(std::rand() % (a * 1000) + b * 1000) / 1000.0; }

////////////////////////////////////////////////////////////////

struct Body {
  Body(const sf::Vector2f& position, const sf::Vector2f& direction) : position(position), direction(direction) {}

  sf::Vector2f position;
  sf::Vector2f direction;
};

struct BodySystem : public ex::System<BodySystem> {
  void update(ex::EntityManager& es, ex::EventManager& events, ex::TimeDelta dt) override
  {
    const auto fdt = static_cast<float>(dt);
    es.each<Body>([fdt](ex::Entity entity, Body& body) { body.position += body.direction * fdt; });
  };
};

class BounceSystem : public ex::System<BounceSystem>
{
public:
  explicit BounceSystem(sf::Vector2u size) : size(size) {}

  void update(ex::EntityManager& es, ex::EventManager& events, ex::TimeDelta dt) override
  {
    es.each<Body>([this](ex::Entity entity, Body& body) {
      if (body.position.x + body.direction.x < 0 || body.position.x + body.direction.x >= size.x) {
        body.direction.x = -body.direction.x;
      }
      if (body.position.y + body.direction.y < 0 || body.position.y + body.direction.y >= size.y) {
        body.direction.y = -body.direction.y;
      }
    });
  }

private:
  sf::Vector2u size;
};

////////////////////////////////////////////////////////////////

using Renderable = std::shared_ptr<sf::Shape>;

class RenderSystem : public ex::System<RenderSystem>
{
public:
  explicit RenderSystem(sf::RenderTarget& target, sf::Font& font) : target(target)
  {
    text.setFont(font);
    text.setPosition(sf::Vector2f(32, 2));
  }

  void update(ex::EntityManager& es, ex::EventManager& events, ex::TimeDelta dt) override
  {
    es.each<Body, Renderable>([this](ex::Entity entity, Body& body, Renderable& renderable) {
      renderable->setPosition(body.position);
      target.draw(*renderable);
    });

    print_fps(dt, es.size());
  }

private:
  void print_fps(ex::TimeDelta dt, size_t nb_entities)
  {
    last_update += dt;
    ++frame_count;
    if (last_update >= 0.5) {
      const double fps = frame_count / last_update;
      oss.seekp(0);
      oss << nb_entities << " entities (" << static_cast<int>(fps) << " fps) ";
      text.setString(oss.str());
      last_update = 0.0;
      frame_count = 0.0;
    }
    target.draw(text);
  }

private:
  sf::RenderTarget& target;
  sf::Text text;
  double last_update = 0;
  double frame_count = 0;
  std::ostringstream oss;
};

////////////////////////////////////////////////////////////////

class GameEngine : public ex::EntityX
{
public:
  GameEngine(sf::RenderTarget& target, sf::Font& font)
  {
    systems.add<BodySystem>();
    systems.add<BounceSystem>(target.getSize());
    systems.add<RenderSystem>(target, font);
    systems.configure();
  }

  void update(ex::TimeDelta dt) { systems.update_all(dt); }
};

void create_bodies(ex::EntityManager& es, sf::Vector2u size, int count)
{
  for (int i = 0; i < count; i++) {
    ex::Entity entity = es.create();

    entity.assign<Body>(sf::Vector2f(r(size.x), r(size.y)), sf::Vector2f(r(200, -200), r(200, -200)));

    Renderable shape(new sf::CircleShape(20));
    shape->setOrigin(10, 10);
    shape->setFillColor(sf::Color(r(128, 127), r(128, 127), r(128, 127)));
    entity.assign<Renderable>(shape);
  }
}

////////////////////////////////////////////////////////////////

int main()
{
  std::srand(std::time(nullptr));

  sf::Font font;
  if (!font.loadFromFile("Roboto.ttf")) {
    std::cerr << "error: failed to load Roboto.ttf" << std::endl;
    return 1;
  }

  sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "ECS Test", sf::Style::Fullscreen);
  window.setVerticalSyncEnabled(true);

  GameEngine engine(window, font);

  create_bodies(engine.entities, window.getSize(), 100);

  sf::Clock clock;
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
        case sf::Event::KeyPressed:
          window.close();
          break;
        default:
          break;
      }
    }

    window.clear();
    auto elapsed = clock.restart();
    engine.update(elapsed.asSeconds());
    window.display();
  }

  return 0;
}
