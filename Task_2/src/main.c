#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "price.h"
#include "db_head.h"

#include "functions.h"

int main(){
    PRICE prices[MAX_GOODS];
    const char *filename = "prices.db";
    
    initscr(); //инициализация ncurses
    cbreak(); //включение мгновенной обработки клавиш
    echo(); //включение эхо-режима
    keypad(stdscr, TRUE); //поддержка функциональных клавиш и клавиш со стрелками

    //проверка существования файла
    FILE *file = fopen(filename, "rb");
    //файл есть
    if(file){
        fclose(file);
        if(!db_exists(filename, &prices[0])) return 0;
        return 1;
    }

    //файла нет
    else{
        char choice_char;
        do{
            mvprintw(1, 2, "Database not found!");
            mvprintw(2, 2, "1 Exit the application and download it or go to the database creation mode!");
            mvprintw(3, 2, "2 Go to the database creation mode!");
            mvprintw(5, 2, "Select 1 or 2 and enter:");
            
            move(5, 2 + 25);
            choice_char = getch();

            switch(choice_char){
                case '1':
                    endwin();
                    return 0;
                case '2':
                    //создаем db
                    create_db(filename, &prices[0]);
                    //вызываем сценарий того что она есть 
                    if(!db_exists(filename, &prices[0])) return 0;
                    return 1;
                default:
                    mvprintw(7, 2, "Input error! Press any key to re-enter.");
                    refresh();
                    getch();
                    clear();
                    refresh();
            }
        } while( !(choice_char == '1' || choice_char == '2') );
    }
}