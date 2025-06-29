#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <zlib.h>


#include "price.h"
#include "db_head.h"


//сохранение данных в файл
int save_to_file(const char *filename, PRICE *prices, int count){
    FILE *file = fopen(filename, "wb");
    if(!file){
        return 1;
    }

    HEAD_DB header = {
        .signature = {'D', 'B', 'P', 'R'},
        .transaction_num = 0,
        .num_structures = count,
        .checksum = crc32(0L, (const Bytef *)prices, count * sizeof(PRICE))
    };

    fwrite(&header, sizeof(HEAD_DB), 1, file);
    fwrite(prices, sizeof(PRICE), count, file);
    fclose(file);
    return 0;
}

//ввод данных для одной структуры
int input_price(int start_y, int start_x, PRICE *price){
    char buffer1[MAX_GOOD_LEN];
    char buffer2[MAX_STORE_LEN];
    char buffer3[20];
    
    mvprintw(start_y, start_x, "Введите информацию о товаре:");
    mvprintw(start_y + 2, start_x, "Название товара (макс. %d символов): ", MAX_GOOD_LEN - 1);
    mvprintw(start_y + 4, start_x, "Название магазина (макс. %d символов): ", MAX_STORE_LEN - 1);
    mvprintw(start_y + 6, start_x, "Цена товара: ");
    
    echo();

    //ввод названия товара
    move(start_y + 2, start_x + 35);
    getnstr(buffer1, MAX_GOOD_LEN - 1); // -1 чтобы гарантированно записать конец строки '\0'
    buffer1[MAX_GOOD_LEN] = '\0';
    strncpy(price->good, buffer1, MAX_GOOD_LEN);
    
    //ввод названия магазина
    move(start_y + 4, start_x + 37);
    getnstr(buffer2, MAX_STORE_LEN - 1);
    buffer2[MAX_STORE_LEN] = '\0';
    strncpy(price->store, buffer2, MAX_STORE_LEN);
    
    //ввод цены
    move(start_y + 6, start_x + 13);
    getnstr(buffer3, 20);
    price->good_price = atof(buffer3);
    
    noecho();
    
    return (strlen(price->good) > 0 && strlen(price->store) > 0 && price->good_price > 0);
}