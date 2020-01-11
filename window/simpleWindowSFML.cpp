// STL
#include <iostream>
#include <optional>
// SFML
#include <SFML/Window.hpp>

static auto parseProgramOptions(int argc, char** argv)
  -> std::optional<std::pair<uint32_t, uint32_t>> {
  if(argc != 3) {
    return std::nullopt;
  }

  return std::make_pair(
    std::stoi(argv[1]),
    std::stoi(argv[2])
  );
}

auto main(int argc, char* argv[]) -> int{
  auto width  = 640U;
  auto height = 480U;
  if(const auto args = parseProgramOptions(argc, argv);
     args) {
    width  = std::min(args.value().first, sf::VideoMode::getDesktopMode().width);
    height = std::min(args.value().second, sf::VideoMode::getDesktopMode().height);
  }

  sf::Window window(sf::VideoMode(width, height), "My window");

  while(window.isOpen()) {
    sf::Event event;
    while(window.pollEvent(event)) {
      if(event.type == sf::Event::Closed) {
        window.close();
      }
    }
    window.display();
  }

  return EXIT_SUCCESS;
}
