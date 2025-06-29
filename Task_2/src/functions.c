#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <zlib.h>


#include "price.h"
#include "db_head.h"


//сохранение DB в файл
int save_to_file(const char* filename, PRICE* prices, int count){
    FILE *file = fopen(filename, "wb");
    if(!file){
        return 1;
    }

    HEAD_DB header = {
        .signature = {"AAAA"},
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
int input_price(PRICE* price, int iteration, int n){
    char buffer1[MAX_GOOD_LEN];
    char buffer2[MAX_STORE_LEN];
    char buffer3[20];
    
    mvprintw(0, 2, "Entering a good %d from %d", iteration, n);
    mvprintw(2, 2, "Enter product information:");
    mvprintw(2 + 2, 2, "Product name (max %d characters): ", MAX_GOOD_LEN - 1);
    mvprintw(2 + 4, 2, "Store name (max %d characters): ", MAX_STORE_LEN - 1);
    mvprintw(2 + 6, 2, "Product price: ");
    
    //ввод названия товара
    move(2 + 2, 2 + 34);
    getnstr(buffer1, MAX_GOOD_LEN - 1); // -1 чтобы гарантированно записать конец строки '\0'
    buffer1[MAX_GOOD_LEN] = '\0';
    strncpy(price->good, buffer1, MAX_GOOD_LEN);
    
    //ввод названия магазина
    move(2 + 4, 2 + 32);
    getnstr(buffer2, MAX_STORE_LEN - 1);
    buffer2[MAX_STORE_LEN] = '\0';
    strncpy(price->store, buffer2, MAX_STORE_LEN);
    
    //ввод цены
    move(2 + 6, 2 + 15);
    getnstr(buffer3, 20);
    price->good_price = atof(buffer3);
        
    //не дает писать текст в цене, числа и пустые строки в product и store 
    return (strlen(price->good) > 0 && strlen(price->store) > 0 && price->good_price > 0 && atof(price->good) <= 0 && atof(price->store) <= 0);
}

//создание DB
int create_db(const char* filename, PRICE* prices, int* price_count){
    clear();
    refresh();

    int n = 1;
    //ввод количества записей
    do{
        char buffer[10];

        if(n < 1 || n > MAX_GOODS){
            mvprintw(3, 2, "Invalid quantity of goods!");
            mvprintw(4, 2, "Press any key to re-enter.");
            refresh();
            getch();
            move(1, 2); clrtoeol(); //очищение от текущей позиции курсора до конца строки
            move(3, 2); clrtoeol(); 
            move(4, 2); clrtoeol();
        }

        mvprintw(1, 2, "Enter the quantity of products (1-%d): ", MAX_GOODS);

        getnstr(buffer, 9);
        n = atoi(buffer); //преобразует строку символов в целое число
    } while(n < 1 || n > MAX_GOODS);
    
    //ввод данных
    for(int i = 0; i < n; i++){
        clear();
        while(!input_price(&prices[*price_count], i+1, n)){
            mvprintw(10, 2, "Input error! Press any key to re-enter.");
            refresh();
            getch();
            clear();
            refresh();
        }
        (*price_count)++;
        refresh();
    }
    clear();
    refresh();

    //сохранение в файл
    if(save_to_file(filename, &prices[0], *price_count) != 0){
        mvprintw(10, 2, "Error saving file! Press any key to exit...");
        refresh();
        getch();
        endwin();
        return 1;
    }

    mvprintw(1, 2, "The data has been saved successfully to %s!", filename);
    mvprintw(3, 2, "Press any key to continue.");
    refresh();
    getch();
    clear();
    return 0;
}

//сценарий когда DB существует
int db_exists(const char* filename){

    refresh();
    getch();
    endwin();
    return 0;

    char choice_char;
    do{
        mvprintw(0, 2, "The database %s found!", filename);
        mvprintw(1, 2, "Select action with database:");
        mvprintw(2, 2, "1 Show database!");
        mvprintw(3, 2, "2 Add new product!");
        mvprintw(4, 2, "3 Find the average price of a product!");
        mvprintw(5, 2, "4 Exit the application");
        mvprintw(7, 2, "Select 1...4 and enter:");
        
        move(7, 2 + 28);
        choice_char = getch();

        switch(choice_char){
            case '1':
                show_db();

                if(!db_exists()) return 0;
                return 1;
            case '2':
                add_product();
                
                if(!db_exists()) return 0;
                return 1;
            case '3':
                average_price();
                
                if(!db_exists()) return 0;
                return 1;
            case '4':
                endwin();
                return 0;
            default:
                mvprintw(9, 2, "Input error! Press any key to re-enter.");
                refresh();
                getch();
                clear();
                refresh();
        }
    } while( !(choice_char == '1' || choice_char == '2' || choice_char == '3' || choice_char == '4') );
}