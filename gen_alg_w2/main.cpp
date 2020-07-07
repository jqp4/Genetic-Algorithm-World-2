//
//  main.cpp
//  gen_alg_w2
//
//  Created by  Gleb on 07.07.2020.
//  Copyright © 2020  Gleb. All rights reserved.
//

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>


const int height = 1400, width = 2100;
const int xcenter = width/2, ycenter = height/2;
const bool debug = true;
int iteration = 0;





void set_text_settings(sf::Text *text, sf::Font *font, int size, sf::Color color){
    std::string font_path = "/Users/gleb/Projects/C++/SFML/Inconsolata/inconsolata.ttf";
    if (!font->loadFromFile(font_path)){
        std::cout << "\nLOAD FONT ERROR\n\n";
        //return EXIT_FAILURE;
    }
    text->setFont(*font);
    text->setCharacterSize(size);
    text->setFillColor(color);
}


std::string get_debug_info(){
    std::string str = "iteration " + std::to_string(iteration) + "\n";
    /*char buf[100];
    for (int i = 0; i < line.settings.n; i++){
        int res = snprintf(buf, sizeof(buf), \
                           "zln[%02.d] x = %4.d y = %4.d xn = %6.3f yn = %6.3f a = %6.3f\n", \
                           i, int(line.ghost[i].x), int(line.ghost[i].y), line.ghost[i].xn, \
                           line.ghost[i].yn, line.ghost[i].a);
        if (res >= 0 && res < sizeof(buf)){
            str += buf;
        } else {
            str = "FORMAT ERROR";
            break;
        }
    }*/
    return str;
}




int main(){
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(width, height), "Thick Line", sf::Style::Default, settings);
    sf::Vertex axes[] = {
        sf::Vertex(sf::Vector2f(0, ycenter)), sf::Vertex(sf::Vector2f(width, ycenter)),
        sf::Vertex(sf::Vector2f(xcenter, 0)), sf::Vertex(sf::Vector2f(xcenter, height)),
        sf::Vertex(sf::Vector2f(0, height)), sf::Vertex(sf::Vector2f(height, 0))
    };
    sf::Text text;
    sf::Font font;
    set_text_settings(&text, &font, 30, sf::Color::Green);
    
    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                default:
                    break;
            }
        }
        
        iteration ++;
        window.clear();
        
        window.draw(axes, 6, sf::Lines);
        if (debug){
            text.setString(get_debug_info());
            window.draw(text);
        }
        window.display();
        sf::sleep(sf::milliseconds(15));
    }
    return 0;
}
