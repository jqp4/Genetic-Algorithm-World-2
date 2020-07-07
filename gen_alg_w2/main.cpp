//
//  main.cpp
//  gen_alg_w2
//
//  Created by  Gleb on 07.07.2020.
//  Copyright © 2020  Gleb. All rights reserved.
//

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <cmath>
#include <set>


const int width  = 2100;
const int height = 1400;
const int xLen = 30;
const int yLen = 20;
const int MBA  = xLen * yLen; //maxBotAmount
const int CA = 8;//64; //commandAmount
#define rca (rand() % CA)
#define freeindex (*free_indexes.begin())
const sf::Color bgColor = sf::Color::Black;
const bool debug = true;
int iteration = 0;



class World{
public:
    int field[xLen][yLen], ba, lbn; //botAmount, lastBotNumber
    std::set<int> free_indexes;
    int fotosintesis_energy = 20;
    int max_energy = 500;
    int energy_perit = 10;

    class Bot{
    public:
        int genome[CA];
        //int birthday;
        int pointer;
        int energy;
        int number;
        bool alive;
        int x, y;
        
        Bot(){
            pointer = 0;
            energy  = 100;
            alive   = true;
            for (int i = 0; i < CA; i++){
                genome[i] = rca;
            }
        }
        
        void mutation(){
            genome[rca] = rca;
        }
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
        
        if (field[tx][ty] == -1){
            field[bots[i].x][bots[i].y] = -1;
            field[tx][ty] = i;
            bots[i].x = tx;
            bots[i].y = ty;
            return true;
        }
        return false;
    }
    
    void bot__fotosintesis(int i){
        bots[i].energy = (bots[i].energy + fotosintesis_energy);
        if (bots[i].energy > max_energy){
            bots[i].energy = max_energy;
        }
    }
    
    void bot__die(int i){
        field[bots[i].x][bots[i].y] = -2;
        bots[i].alive = false;
        ba--;
        free_indexes.insert(i);
    }
    
    void bots_act(){
        for (int i = 0; i < lbn; i++){
            if (bots[i].alive){
                int p = bots[i].pointer;
                int act = bots[i].genome[p];
                bool r;
                switch (act) {
                    case 0:
                        r = bot__move(i, bots[i].genome[p + 1]);
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
        }
    }
    
    World(int bot_amount = xLen + yLen){
        ba  = bot_amount;
        lbn = bot_amount;
        for (int i = lbn; i < MBA; i++){
            free_indexes.insert(i);
        }
        for (int x = 0; x < xLen; x++){
            for (int y = 0; y < yLen; y++){
                field[x][y] = -1;
            }
        }
        for (int i = 0; i < lbn; i++){
            bool done = false;
            while (!done){
                int tx = rand() % xLen;
                int ty = rand() % yLen;
                if (field[tx][ty] == -1){
                    bots[i].number = i;
                    field[tx][ty] = i;
                    bots[i].x = tx;
                    bots[i].y = ty;
                    done = true;
                }
            }
        }
    }
};


class WindowField{
public:
    sf::RectangleShape ghost;//s[MBA];
    int pixel_size;
    int indent;
    int olt; //OutlineThickness
    
    WindowField(int pixel_size, int outline_thickness){
        this->pixel_size = pixel_size;
        olt = outline_thickness;
        indent = pixel_size/2;
        ghost = sf::RectangleShape(sf::Vector2f(pixel_size - olt*2, pixel_size - olt*2));
    }
    
    void draw(sf::RenderWindow *window, World world){
        for (int i = 0; i < world.lbn; i++) {
            int wx = indent + pixel_size * world.bots[i].x;
            int wy = indent + pixel_size * world.bots[i].y;
            ghost.setPosition(wx, wy);
            ghost.setOutlineThickness(olt);
            ghost.setFillColor(sf::Color::Yellow);
            ghost.setOutlineColor(sf::Color(82, 82, 82));
            window->draw(ghost);
        }
        
        for (int y = 0; y < yLen; y++){
            for (int x = 0; x < yLen; x++){
                if (world.field[y][x] == -2){
                    int wx = indent + pixel_size * x;
                    int wy = indent + pixel_size * y;
                    ghost.setPosition(wx, wy);
                    ghost.setFillColor(sf::Color(202, 202, 202));
                    ghost.setOutlineColor(bgColor);
                    window->draw(ghost);
                }
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
    std::string str = "World iteration " + std::to_string(iteration) + "\n";
    str += "Bot Amount = " + std::to_string(world.ba) + "\n";
    str += "Last Bot Number = " + std::to_string(world.lbn) + "\n";
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
    for (int x = 0; x < xLen; x++){
        for (int y = 0; y < yLen; y++){
            int ind = world.field[x][y];
            switch (ind){
                case -1:
                    str += " -- ";
                    break;
                case -2:
                    str += " xx ";
                    break;
                default:
                    std::string tmps = std::to_string(world.bots[ind].energy);
                    int sc = 4 - (int)tmps.length();
                    str += (sc > 1 ? " " : "") + tmps + (sc > 0 ? " " : "");
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
    World world(20);
    WindowField winfld(40, 2);
    
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
        //winfld.draw(&window, world);    //   BUG !!!
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
    std::set<int> free_indexes;
    free_indexes.insert(4);
    free_indexes.insert(5);
    free_indexes.insert(6);
    free_indexes.erase(free_indexes.begin());
    std::cout << freeindex << "\n";
    return 0;
}
