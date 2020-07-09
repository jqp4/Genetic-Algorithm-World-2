//
//  main.cpp
//  gen_alg_w2
//
//  Created by  Gleb on 07.07.2020.
//  Copyright © 2020  Gleb. All rights reserved.
//

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <set>


const int width  = 2100;
const int height = 1400;
const int xLen = 97;
const int yLen = 97;
const int MBA  = xLen * yLen; //maxBotAmount
const int CA   = 64; //commandAmount
#define rca (rand() % CA)
#define freeindex (*free_indexes.begin())
#define check_availability(x, y, tx, ty) (field[tx][y] == -1 || field[x][ty] == -1)
const sf::Color bgColor = sf::Color::White;
const sf::Color frameColor = sf::Color(100, 100, 100);
std::string savePath = "/Users/gleb/Projects/C++/SFML/Genetic-Algorithm-World-2/gen_alg_w2/saves/";
int iteration, debug = 0; // 0 - none, 1 - text, 2 - field
char showMode = 'd';


class World{
public:
    int field[xLen][yLen], ba, lbn; //botAmount, lastBotNumber
    //field:  -3      error
    //        -2      organics
    //        -1      free
    //    0..MBA - 1  bot (id)
    std::set<int> free_indexes;
    static const int mutation_probability = 50;
    static const int birth_energy = 70;
    int cell_division_energy = 20;
    int photosynthesis_energy = 20;
    int organics_energy = 50;
    int energy_perit = 10;
    int max_energy = 950;
    
    
    class Bot{
    public:
        int genome[CA];
        int pointer;
        int direction_look; // 0..8
        int energy;
        bool alive;
        int x, y;
        sf::Color color;
        int energy_source; //0 - photo, 20 - kills
        
        Bot(){
            pointer = 0;
            energy  = birth_energy * 2;
            alive   = true;
            color   = sf::Color::Red;
            energy_source = 10;
            direction_look = rand() % 8;
            for (int i = 0; i < CA; i++){
                genome[i] = rca;
            }
        }
        
        void mutation(){
            genome[rca] = rca;
        }
        
        void birth(Bot parent){
            pointer = 0;
            energy  = birth_energy;
            alive   = true;
            color   = sf::Color::Red;
            energy_source = 10;
            direction_look = rand() % 8;
            for (int i = 0; i < CA; i++){
                genome[i] = parent.genome[i];
            }
            if (rand() % 100 < mutation_probability){
                //mutation();
                genome[rca] = rca;
            }
        }
        
        void copy_genome(Bot a){
            for (int i = 0; i < CA; i++){
                genome[i] = a.genome[i];
            }
        }
        
        void filesave(std::string filename){
            std::ofstream file;
            file.open(savePath + filename);
            if (file.is_open()){
                int j = 0;
                for (int i = 0; i < CA; i++){
                    std::string sgi = std::to_string(genome[i]);
                    file << ((int)sgi.length() < 2 ? " " : "") + sgi + " ";
                    if (++j == 8){
                        file << "\n";
                        j = 0;
                    }
                }
                file.close();
            } else {
                std::cout << "ERROR in World::Bot::filesave - Unable to open file";
            }
        }
        
        void fileread(std::string filename){
            std::string line;
            std::ifstream file;
            file.open(savePath + filename);
            if (file.is_open()){
                int x, i = 0;
                int command = -1;
                while (getline (file, line)){
                    while (line.length() > 0){
                        x = (int)line[0] - 48;
                        if (x == -12){
                            // line[0] = '$'
                            command = (int)line[1] - 48;
                            x = (int)line[2] - 48;
                            if (0 <= x && x <= 9){
                                command = command * 10 + x;
                            }
                            genome[i++] = CA - command;
                            command = -1;
                            line.erase(0, 3);
                        } else {
                            while (0 <= x && x <= 9){
                                // line[0] = 0, 1, ..., 9
                                if (command == -1) command = x;
                                else command = command * 10 + x;
                                line.erase(0, 1);
                                x = (int)line[0] - 48;
                            }
                            if (command != -1){
                                genome[i++] = command;
                                command = -1;
                            } else {
                                line.erase(0, 1);
                            }
                        }
                    }
                }
                file.close();
            } else {
                std::cout << "ERROR in World::Bot::fileread - Unable to open file";
            }
        }
    } bots[MBA];
    
    sf::Color get_bot_color(int i){
        int xc;
        switch (showMode){
            case 'e':
                xc = 250 - (int)((float)bots[i].energy / max_energy * 150);
                return sf::Color(255, xc, 0);
                break;
            case 'd':
                xc = (int)((float)bots[i].energy_source / 20 * 150);
                return sf::Color(100 + xc, 150 - xc, 250 - xc);
                break;
            default:
                return sf::Color(0, 0, 0);
                break;
        }
    }
    
    void get_direction_coords(int *x, int *y, int direction){
        //directoin:  6 7 0
        //            5   1
        //            4 3 2
        switch (direction){
            case 0:
                *x = (*x + 1) % xLen;
                *y = (yLen + *y - 1) % yLen;
                break;
            case 1:
                *x = (*x + 1) % xLen;
                break;
            case 2:
                *x = (*x + 1) % xLen;
                *y = (*y + 1) % yLen;
                break;
            case 3:
                *y = (*y + 1) % yLen;
                break;
            case 4:
                *x = (xLen + *x - 1) % xLen;
                *y = (*y + 1) % yLen;
                break;
            case 5:
                *x = (xLen + *x - 1) % xLen;
                break;
            case 6:
                *x = (xLen + *x - 1) % xLen;
                *y = (yLen + *y - 1) % yLen;
                break;
            case 7:
                *y = (yLen + *y - 1) % yLen;
                break;
            default:
                *x = *y = -1; // return error
        }
    }
    
    int  bot__look(int i, int rd){
        int tx = bots[i].x;
        int ty = bots[i].y;
        //direction = (bots[i].direction_look + relative_direction) % 8;
        get_direction_coords(&tx, &ty, (bots[i].direction_look + rd) % 8);
        return field[tx][ty];
    }
    
    int  bot__move(int i, int rd){
        int tx = bots[i].x;
        int ty = bots[i].y;
        //direction = (bots[i].direction_look + relative_direction) % 8;
        get_direction_coords(&tx, &ty, (bots[i].direction_look + rd) % 8);
        int obj = field[tx][ty];
        if (obj == -1){
            field[bots[i].x][bots[i].y] = -1;
            field[tx][ty] = i;
            bots[i].x = tx;
            bots[i].y = ty;
        }
        return obj;
    }
    
    void bot__photosynthesis(int i){
        bots[i].energy = bots[i].energy + photosynthesis_energy;
        bots[i].energy_source -= bots[i].energy_source > 0 ? 1 : 0;
        if (bots[i].energy > max_energy){
            bots[i].energy = max_energy + energy_perit;
        }
    }
    
    void bot__rotate(int i, int angel8){
        bots[i].direction_look = (bots[i].direction_look + angel8) % 8;
    }
    
    bool bot__energy_check(int i, int factor){
        int cmp_energy = 15 * factor;
        if (bots[i].energy >= cmp_energy){
            return true;
        }
        return false;
    }
    
    void bot__die(int i, bool kill = false){
        //field[bots[i].x][bots[i].y] = -2;
        field[bots[i].x][bots[i].y] = kill ? -1 : -2;
        bots[i].alive = false;
        ba--;
        free_indexes.insert(i);
    }
    
    void bot__eat(int i, int rd){
        int tx = bots[i].x;
        int ty = bots[i].y;
        //direction = (bots[i].direction_look + relative_direction) % 8;
        get_direction_coords(&tx, &ty, (bots[i].direction_look + rd) % 8);
        int obj = field[tx][ty];
        if (obj == -2){
            bots[i].energy = bots[i].energy + organics_energy;
            bots[i].energy_source += bots[i].energy_source < 20 ? 1 : 0;
            field[tx][ty] = -1;
        } else if (obj == -1){
            //pass
        } else if (obj >= 0){
            bots[i].energy = bots[i].energy + organics_energy + bots[obj].energy/10;
            bots[i].energy_source += bots[i].energy_source < 20 ? 1 : 0;
            bot__die(obj, true);
        }
        if (bots[i].energy > max_energy){
            bots[i].energy = max_energy + energy_perit;
        }
    }
    
    /*bool check_availability(int x, int y, int tx, int ty){
        return field[tx][y] == -1 || field[x][ty] == -1;
    }*/
    
    bool bot__division(int i, int rd){
        if (bots[i].energy >= cell_division_energy + birth_energy * 2){
            if (free_indexes.size() > 0){
                int tx = bots[i].x;
                int ty = bots[i].y;
                get_direction_coords(&tx, &ty, (bots[i].direction_look + rd) % 8);
                if (check_availability(bots[i].x, bots[i].y, tx, ty)){
                    int obj = field[tx][ty];
                    if (obj == -1){
                        int j = freeindex;
                        bots[j].birth(bots[i]);
                        bots[i].energy -= cell_division_energy + birth_energy;
                        field[tx][ty] = j;
                        bots[j].x = tx;
                        bots[j].y = ty;
                        free_indexes.erase(free_indexes.begin());
                        ba++;
                        if (lbn < ba){
                            lbn++;
                        }
                        return true;
                    }
                }
            }
        }
        return false;
    }
    
    void bots_act(){
        for (int i = 0; i < lbn; i++){
            if (bots[i].alive){
                int steps2think = 1;
                bool again = false;
                while (steps2think-- + (int)again){
                    int p = bots[i].pointer;
                    int act = bots[i].genome[p];
                    int ires;
                    bool bres;
                    switch (act) {
                        case 23:
                            bot__rotate(i, bots[i].genome[p + 1]);
                            bots[i].pointer = (p + 2) % CA;
                            break;
                        case 25:
                            bot__photosynthesis(i);
                            bots[i].pointer = (p + 1) % CA;
                            break;
                        case 26:
                            ires = bot__move(i, bots[i].genome[p + 1]);
                            if (ires == -3){
                                printf("\nbot %d act %d MOVE ERROR\n", i, act);
                                bots[i].pointer = (p + 1) % CA;
                            } else if (ires == -2){
                                bots[i].pointer = (p + bots[i].genome[p + 3]) % CA;
                            } else if (ires == -1){
                                bots[i].pointer = (p + bots[i].genome[p + 2]) % CA;
                            } else if (ires >= 0){
                                bots[i].pointer = (p + bots[i].genome[p + 4]) % CA;
                            } else {
                                printf("\nbot %d act %d BOT ID ERROR\n", i, act);
                                bots[i].pointer = (p + 1) % CA;
                            }
                            break;
                        case 28:
                            bot__eat(i, bots[i].genome[p + 1]);
                            bots[i].pointer = (p + 2) % CA;
                            break;
                        case 30:
                            ires = bot__look(i, bots[i].genome[p + 1]);
                            if (ires == -2){
                                bots[i].pointer = (p + bots[i].genome[p + 3]) % CA;
                            } else if (ires == -1){
                                bots[i].pointer = (p + bots[i].genome[p + 2]) % CA;
                            } else if (ires >= 0){
                                bots[i].pointer = (p + bots[i].genome[p + 4]) % CA;
                            }
                            break;
                        case 555:
                            bres = bot__division(i, bots[i].genome[p + 1]);
                            if (bres){
                                bots[i].pointer = (p + bots[i].genome[p + 2]) % CA;
                            } else {
                                bots[i].pointer = (p + bots[i].genome[p + 3]) % CA;
                            }
                            break;
                        case 38:
                            bres = bot__energy_check(i, bots[i].genome[p + 1]);
                            if (bres){
                                bots[i].pointer = (p + bots[i].genome[p + 3]) % CA;
                            } else {
                                bots[i].pointer = (p + bots[i].genome[p + 2]) % CA;
                            }
                            break;
                        default:
                            bots[i].pointer = (p + act) % CA;
                            again = true;
                            break;
                    }
                }
                bots[i].energy -= energy_perit;
                if (bots[i].energy <= 0){
                    bot__die(i);
                }
                if (bots[i].energy >= 200){
                    int dir = 0;
                    while (!bot__division(i, dir) && dir < 8){
                        dir++;
                    }
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
    int pixel_size;
    int indent;
    int olt; //OutlineThickness
    
    class BackgroundPicture{
    public:
        static const int qn = 2;
        sf::RectangleShape quads[qn];
        
        BackgroundPicture(){}
        
        BackgroundPicture(int pixel_size, int indent){
            //int wx = indent + pixel_size * xLen;
            //int wy = indent + pixel_size * yLen;
            quads[0].setSize(sf::Vector2f(pixel_size * xLen, pixel_size * yLen));
            quads[0].setPosition(indent, indent);
            quads[0].setOutlineThickness(indent);
            quads[0].setOutlineColor(frameColor);
            quads[0].setFillColor(bgColor);
            
        }
        
        void draw(sf::RenderWindow *window){
            for (int i = 0; i < qn; i++){
                window->draw(quads[i]);
            }
        }
    } bgpic;
    
    class Ghost{
    public:
        sf::RectangleShape bot;
        sf::RectangleShape organics;
        sf::ConvexShape custom;
        
        Ghost(){}
        
        Ghost(int ps, int olt){
            bot = sf::RectangleShape(sf::Vector2f(ps - olt*2, ps - olt*2));
            bot.setOutlineColor(sf::Color(82, 82, 82));
            bot.setOutlineThickness(olt);
            
            organics = sf::RectangleShape(sf::Vector2f(ps - olt*2, ps - olt*2));
            organics.setFillColor(sf::Color(202, 202, 202));
            organics.setOutlineColor(bgColor);
            organics.setOutlineThickness(olt);
            
            custom.setPointCount(3);
            custom.setPoint(0, sf::Vector2f(ps/2, 0));
            custom.setPoint(1, sf::Vector2f(ps, ps));
            custom.setPoint(2, sf::Vector2f(0, ps));
            custom.setFillColor(sf::Color::Black);
        }
    } ghost;
    
    WindowField(int pixel_size, int outline_thickness){
        this->pixel_size = pixel_size;
        olt = outline_thickness;
        indent = pixel_size/2;
        bgpic = BackgroundPicture(pixel_size, indent);
        ghost = Ghost(pixel_size, olt);
    }
    
    void draw(sf::RenderWindow *window, World world, sf::Text text){
        bgpic.draw(window);
        for (int x = 0; x < xLen; x++){
            for (int y = 0; y < yLen; y++){
                int i = world.field[x][y];
                int wx = indent + olt + pixel_size * x;
                int wy = indent + olt + pixel_size * y;
                switch (i){
                    case -1:
                        //pass
                        break;
                    case -2:
                        //organics
                        ghost.organics.setPosition(wx, wy);
                        window->draw(ghost.organics);
                        break;
                    default:
                        //bot
                        if (i </*==*/ 0){
                            //World::Bot bbb = world.bots[i]; // debug custom bot:
                            ghost.custom.setPosition(wx, wy);
                            //ghost.custom.setFillColor(world.bots[i].color);
                            window->draw(ghost.custom);
                        } else {
                            ghost.bot.setPosition(wx, wy);
                            //ghost.bot.setFillColor(world.bots[i].color);
                            ghost.bot.setFillColor(world.get_bot_color(i));
                            window->draw(ghost.bot);
                        }
                        if (debug == 2){
                            text.setString(std::to_string(world.bots[i].energy));
                            text.setFillColor(sf::Color::Black);
                            text.setPosition(wx - 2, wy - 9);
                            window->draw(text);
                        }
                        break;
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


void custom_bot(World::Bot *bot, int number){
    /*switch (number) {
        case 2: {
            int custom_genome[] = { 25, 25, 26, 0, CA - 2, 5, 5, \
                                    25, 25, 23, 1, CA - 10 };
            int i = sizeof(custom_genome)/sizeof(int);
            while (i--) bot.genome[i] = custom_genome[i];
            break;
        } case 3: {
            int custom_genome[] = { 25, 23, 1, 30, 0, 5, 12, 12, \
                                    38, 1, 5, CA - 11, \
                                    25, 25, CA - 14, \
                                    28, 0, CA - 17 };
            int i = sizeof(custom_genome)/sizeof(int);
            while (i--) bot.genome[i] = custom_genome[i];
            break;
        }
        default:
            break;
    }*/
    std::string filename = "custom_bots/cbg" + std::to_string(number) + ".txt";
    bot->fileread(filename);
}


void change_showMode(){
    switch (showMode) {
        case 'e':
            showMode = 'd';
            break;
        case 'd':
            showMode = 'e';
            break;
    }
}


int main(){
    //srand(static_cast<unsigned int>(time(nullptr)));
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(width, height), "Gen Alg", sf::Style::Default, settings);
    sf::Text text;
    sf::Font font;
    set_text_settings(&text, &font, 30, sf::Color::Green);
    label_Start:
    iteration = 0;
    srand(static_cast<unsigned int>(time(nullptr)));
    unsigned int seed = rand();
    srand(seed);
    World world(100);
    WindowField winfld(14, 2);
    //custom_bot(&world.bots[0], 5);
    //world.bots[0].filesave("test.txt");
    //world.bots[0].fileread("custom_bots/cbg1.txt");
    
    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::R:
                            goto label_Start;
                            break;
                        case sf::Keyboard::S:
                            printf("seed is %u\n", seed);
                            break;
                        case sf::Keyboard::Q:
                            window.close();
                            break;
                        case sf::Keyboard::M:
                            //window.close();
                            change_showMode();
                            break;
                        default:
                            break;
                    }
                default:
                    break;
            }
        }
        
        iteration ++;
        world.bots_act();
        if (iteration % 15 == 0){
            window.clear();
            //world.bots_act();
            winfld.draw(&window, world, text);
            if (debug == 1){
                text.setString(get_debug_info(world));
                window.draw(text);
            }
            window.display();
            sf::sleep(sf::milliseconds(10));
        }
        //world.bots_act();
    }
    return 0;
}
