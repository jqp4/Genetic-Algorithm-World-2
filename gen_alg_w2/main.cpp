//
//  main.cpp
//  gen_alg_w2
//
//  Created by  Gleb on 07.07.2020.
//  Copyright © 2020  Gleb. All rights reserved.
//

//https://en.cppreference.com/w/cpp/language/operator_comparison

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <cmath>
#include <set>

const int width   = 2100;
const int height  = 1400;
const int xcenter = width/2;
const int ycenter = height/2;
const int CA/*commandAmount*/ = 3;//64;
#define rca (rand() % CA)
#define freeindex (*free_indexes.begin())
const bool debug = true;
int iteration = 0;





class World{
public:
    static const int xLen = 30;
    static const int yLen = 20;
    static const int MBA  = xLen * yLen; //maxBotAmount
    int field[yLen][xLen],  ba; //BotAmount
    std::set<int> free_indexes;
    
    class Bot{
    public:
        int genome[CA];
        int birthday;
        int pointer;
        
        Bot(int global_iter = -1){
            pointer = 0;
            birthday = global_iter;
            for (int i = 0; i < CA; i++){
                genome[i] = rca;
            }
        }
        
        void copy(Bot parent, int global_iter = -1){
            pointer = 0;
            birthday = global_iter;
            //genome = parent.genome;
            for (int i = 0; i < CA; i++){
                genome[i] = parent.genome[i];
            }
        }
        
        void mutation(){ genome[rca] = rca; }
        
        /*bool operator <  (const Bot &b) const { return birthday <  b.birthday; } //true; }
        bool operator >  (const Bot &b) const { return birthday >  b.birthday; } //true; }
        bool operator == (const Bot &b) const { return birthday == b.birthday; } //true; }
        bool operator <= (const Bot &b) const { return birthday <= b.birthday; } //true; }
        bool operator >= (const Bot &b) const { return birthday >= b.birthday; } //true; }*/
    } bots[MBA];
    
    class ActsOfBot{
        
    } aof;
    
    World(int bot_amount = 10){
        ba = bot_amount;
        for (int i = ba - 1; i < MBA; i++){
            free_indexes.insert(i);
        }
        for (int i = 0; i < yLen; i++){
            for (int j = 0; j < xLen; j++){
                field[i][j] = 0;
            }
        }
    }
};


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


std::string get_debug_info(World world){
    std::string str = "iteration " + std::to_string(iteration) + "\n";
    char buf[100];
    /*for (int i = 0; i < line.settings.n; i++){
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
    int res = snprintf(buf, sizeof(buf), "Bot Amount = %3.d", world.ba);
    if (res >= 0 && res < sizeof(buf)){
        str += buf;
    } else {
        str = "FORMAT ERROR";
    }
    return str;
}


int main(){
    srand(static_cast<unsigned int>(time(nullptr))); 
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(width, height), "Gen Alg", sf::Style::Default, settings);
    sf::Vertex axes[] = {
        sf::Vertex(sf::Vector2f(0, ycenter)), sf::Vertex(sf::Vector2f(width, ycenter)),
        sf::Vertex(sf::Vector2f(xcenter, 0)), sf::Vertex(sf::Vector2f(xcenter, height))
    };
    sf::Text text;
    sf::Font font;
    set_text_settings(&text, &font, 30, sf::Color::Green);
    
    World world;
    
    
    
    
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
        
        
        window.draw(axes, 4, sf::Lines);
        if (debug){
            text.setString(get_debug_info(world));
            window.draw(text);
        }
        window.display();
        sf::sleep(sf::milliseconds(15));
    }
    return 0;
}


int main_set_test(){
    /*std::set<Bot> a;
    Bot bot1(12), bot2;
    a.insert(bot1);
    a.insert(bot2);
    std::cout << "Set Size = " << a.size() << std::endl;
    for (std::set<Bot>::iterator it = a.begin(); it != a.end(); it++){
        std::cout << " " << it->birthday;
    }
    std::cout << "\n";*/
    
    std::set<int> free_indexes;
    free_indexes.insert(4);
    free_indexes.insert(5);
    free_indexes.insert(6);
    
    std::cout << freeindex << "\n";
    
    return 0;
}
