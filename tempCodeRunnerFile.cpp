 pWindow->drawLine((float)rayCount, 0, (float)rayCount, halfHeight, sf::Color::Green);
            pWindow->drawLine((float)rayCount, halfHeight, (float)rayCount, (float)HEIGHT, sf::Color::Blue);
            pWindow->drawLine((float)rayCount, halfHeight - wallHeight, (float)rayCount, halfHeight + wallHeight, sf::Color(100, 62, 10, 100));
            pWindow->drawVericalLine(0, (int)halfHeight, rayCount, sf::Color::Green);