#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "price.h"
#include "db_head.h"

#include "functions.h"

#define MAX_GOODS 100

int main(){
    PRICE prices[MAX_GOODS];
    int price_count = 0;
    const char *filename = "prices.db";
    
    // Проверка существования файла
    FILE *file = fopen(filename, "rb");
    if (file) {
        fclose(file);
        printw("Файл базы данных уже существует!\n");
        printw("Нажмите любую клавишу для выхода...");
        refresh();
        getch();
        return 0;
    }
    

    initscr(); //инициализация ncurses
    cbreak(); //включение мгновенной обработки клавиш
    noecho(); //выключение эхо-режима
    keypad(stdscr, TRUE); //поддержка функциональных клавиш и клавиш со стрелками

    int n = 1;
    //ввод количества записей
    do{
        if(n < 1 || n > MAX_GOODS){
                mvprintw(3, 1, "Недопустимое количество товаров!");
                mvprintw(4, 1, "Нажмите любую клавишу для повторного ввода.");
                refresh();
                getch();
            move(3, 1); clrtoeol(); //очищение от текущей позиции курсора до конца строки
                move(4, 1); clrtoeol();
            }

            mvprintw(1, 1, "Введите количество товаров (1-%d): ", MAX_GOODS);
            char buffer[10];
            echo();
            getnstr(buffer, 9);
            n = atoi(buffer); //преобразует строку символов в целое число
            noecho();
    } while(n < 1 || n > MAX_GOODS);
    
    //ввод данных
    for(int i = 0; i < n; i++){
        clear();
        mvprintw(0, 0, "Ввод товара %d из %d", i + 1, n);
        while(!input_price(2, 2, &prices[price_count])){
            mvprintw(10, 2, "Ошибка ввода! Нажмите любую клавишу для повторного ввода.");
            refresh();
            getch();
            clear();
	    refresh();
        }
        price_count++;
        refresh();
    }
    
    //сохранение в файл
    if(save_to_file(filename, prices, price_count) != 0){
        mvprintw(10, 2, "Ошибка сохранения файла! Нажмите любую клавишу для выхода...");
        refresh();
        getch();
        endwin();
        return 1;
    }
    
    //успешное завершение
    clear();
    mvprintw(1, 1, "Данные успешно сохранены в %s!", filename);
    mvprintw(3, 1, "Нажмите любую клавишу для выхода...");
    refresh();
    getch();
    
    endwin();
    return 0;
}