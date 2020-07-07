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
const int CA/*commandAmount*/ = 8;//64;
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
    int fotosintesis_energy = 20;
    int max_energy = 512;
    int energy_perit = 10;

        
    class Bot{
    public:
        int genome[CA];
        int birthday;
        int pointer;
        int energy;
        int number;
        int x, y;
        
        Bot(int global_iter = -1){
            pointer = 0;
            energy  = 100;
            birthday = global_iter;
            for (int i = 0; i < CA; i++){
                genome[i] = rca;
            }
        }
        
        void copy(Bot parent, int global_iter = -1){
            pointer = 0;
            energy  = 100;
            birthday = global_iter;
            //genome = parent.genome;
            for (int i = 0; i < CA; i++){
                genome[i] = parent.genome[i];
            }
        }
        
        void mutation(){ genome[rca] = rca; }
        
        /*bool operator <  (const Bot &b) const { return birthday <  b.birthday; }
        bool operator >  (const Bot &b) const { return birthday >  b.birthday; }
        bool operator == (const Bot &b) const { return birthday == b.birthday; }
        bool operator <= (const Bot &b) const { return birthday <= b.birthday; }
        bool operator >= (const Bot &b) const { return birthday >= b.birthday; }*/
    } bots[MBA];
    
    bool bot__move(int i, int direction){
        int tx = bots[i].x;
        int ty = bots[i].y;
        //directoin:  6 7 0
        //            5   1
        //            4 3 2
        switch (direction){
            case 0:
                tx = (tx + 1) % xLen;
                ty = (yLen + ty - 1) % yLen;
                break;
            case 1:
                tx = (tx + 1) % xLen;
                break;
            case 2:
                tx = (tx + 1) % xLen;
                ty = (ty + 1) % yLen;
                break;
            case 3:
                ty = (ty + 1) % yLen;
                break;
            case 4:
                tx = (xLen + tx - 1) % xLen;
                ty = (ty + 1) % yLen;
                break;
            case 5:
                tx = (xLen + tx - 1) % xLen;
                break;
            case 6:
                tx = (xLen + tx - 1) % xLen;
                ty = (yLen + ty - 1) % yLen;
                break;
            case 7:
                ty = (yLen + ty - 1) % yLen;
                break;
            default:
                return false;
        }
        
        if (field[ty][tx] == 0){
            field[bots[i].y][bots[i].x] = 0;
            field[ty][tx] = i;
            bots[i].x = tx;
            bots[i].y = ty;
            return true;
        } else {
            return false;
        }
    }
    
    bool bot__fotosintesis(int i){
        bots[i].energy = (bots[i].energy + fotosintesis_energy) % max_energy;
        return true;
    }
    
    bool bot__die(int i){
        field[bots[i].y][bots[i].x] = -1;
        bots[i] = bots[ba - 1];
        ba--;
        free_indexes.insert(i);
        return true;
    }
    
    bool bots_act(){
        for (int i = 0; i < ba; i++){
            int p = bots[i].pointer;
            int act = bots[i].genome[p];
            switch (act) {
                case 0:
                    bot__move(i, bots[i].genome[p + 1]);
                    bots[i].pointer = (p + 2) % CA;
                    break;
                case 1:
                    bot__fotosintesis(i);
                    bots[i].pointer = (p + 1) % CA;
                    break;
                default:
                    bots[i].pointer = (p + act) % CA;
                    break;
            }
            bots[i].energy -= energy_perit;
            if (bots[i].energy <= 0){
                bot__die(i);
            }
        }
        return true;
    }
    
    World(int bot_amount = 50){
        ba = bot_amount;
        for (int i = ba; i < MBA; i++){
            free_indexes.insert(i);
        }
        for (int i = 0; i < yLen; i++){
            for (int j = 0; j < xLen; j++){
                field[i][j] = 0;
            }
        }
        for (int i = 0; i < ba; i++){
            int tx = rand() % xLen;
            int ty = rand() % yLen;
            if (field[ty][tx] == 0){
                bots[i].number = i;
                field[ty][tx] = i;
                bots[i].x = tx;
                bots[i].y = ty;
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
    std::string str = "World, iteration " + std::to_string(iteration) + "\n";
    str += "Bot Amount = " + std::to_string(world.ba) + "\n";
    char buf[10];
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
    
    for (int i = 0; i < world.yLen; i++){
        for (int j = 0; j < world.yLen; j++){
            int ind = world.field[i][j];
            switch (ind){
                case 0:
                    str += " -- ";
                    break;
                case -1:
                    str += " xx ";
                    break;
                default:
                    sprintf(buf, "%3.d ", world.bots[ind].energy);
                    str += buf;
                    break;
            }
        }
        str += "\n";
    }
    return str;
}


int main(){
    srand(static_cast<unsigned int>(time(nullptr))); 
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(width, height), "Gen Alg", sf::Style::Default, settings);
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
        
        world.bots_act();
        
        if (debug){
            text.setString(get_debug_info(world));
            window.draw(text);
        }
        window.display();
        sf::sleep(sf::milliseconds(200));
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
    free_indexes.erase(free_indexes.begin());
    std::cout << freeindex << "\n";
    
    return 0;
}
