#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>

#include "price.h"
#include "db_head.h"

int is_copy(PRICE* prices, char* buffer, int n){
    for(int i=0; i < n; i++){
        if(!strncmp(prices[i].good, buffer, strlen(buffer))) return 1;
    }
    return 0;
}

int average_price(const char* filename, PRICE* prices){
    FILE *file = fopen(filename, "rb+");
    if(!file){
        mvprintw(0, 2, "Error opening file! Press any key to exit...");
        refresh();
        getch();
        endwin();
        return 1;
    }

    //чтение заголовка
    HEAD_DB header;
    if (fread(&header, sizeof(HEAD_DB), 1, file) != 1) {
        fclose(file);
        mvprintw(0, 2, "Error reading file! Press any key to exit...");
        refresh();
        getch();
        endwin();
        return 1;
    }

    header.transaction_num++;
    int count = header.num_structures;

    //чтение данных PRICE
    if (fread(prices, sizeof(PRICE), count, file) != count){
        fclose(file);
        mvprintw(0, 2, "Error reading file! Press any key to exit...");
        refresh();
        getch();
        endwin();
        return 1;
    }

    mvprintw(0, 2, "Select the product whose average price you want to know!");
    mvprintw(2, 2, "Products from %s:", filename);
    int y = 3;
    for(int i=0; i < count; i++){
        if(!is_copy(&prices[0], prices[i].good, i)){
            mvprintw(y, 2, "%d)Product: %s", i+1, prices[i].good);
            y++;
        }
    }

    char buffer[MAX_GOOD_LEN];
    mvprintw(++y, 2, "Enter name: ");
    move(y, 2+12);
    getnstr(buffer, MAX_GOOD_LEN - 1); // -1 чтобы гарантированно записать конец строки '\0'
    buffer[MAX_GOOD_LEN] = '\0';

    double sum = 0;
    int col_vo = 0;
    for(int i=0; i < count; i++){
        if(!strncmp(prices[i].good, buffer, strlen(buffer))){
            sum+= prices[i].good_price;
            col_vo++;
        }
    }

    if(col_vo == 0){
        clear();
        mvprintw(0, 2, "Product %s not found!", buffer);
    }

    else{
        clear();
        mvprintw(0, 2, "%s average price is %.2f!", buffer, sum/col_vo);
    }
    mvprintw(2, 2, "Press any key to exit to menu: ");
    refresh();
    getch();
    clear();

    //обновляем хедер
    fseek(file, 0, SEEK_SET); //SEEK_SET - начало файла
    fwrite(&header, sizeof(HEAD_DB), 1, file);

    fclose(file);
    return 0;
}

int update_checksum(FILE* file, PRICE* prices, HEAD_DB* header, int count){
    fseek(file, sizeof(HEAD_DB), SEEK_SET);
    //чтение данных PRICE
    if (fread(prices, sizeof(PRICE), count, file) != count){
        fclose(file);
        clear();
        mvprintw(0, 2, "Error reading file! Press any key to exit...");
        refresh();
        getch();
        endwin();
        return 1;
    }

    header->checksum = crc32(0L, (const Bytef *)prices, count * sizeof(PRICE));
    return 0;
}

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
    mvprintw(2 + 1, 2, "Product name (max %d characters): ", MAX_GOOD_LEN - 1);
    mvprintw(2 + 2, 2, "Store name (max %d characters): ", MAX_STORE_LEN - 1);
    mvprintw(2 + 3, 2, "Product price: ");
    
    //ввод названия товара
    move(2 + 1, 2 + 34);
    getnstr(buffer1, MAX_GOOD_LEN - 1); // -1 чтобы гарантированно записать конец строки '\0'
    buffer1[MAX_GOOD_LEN] = '\0';
    strncpy(price->good, buffer1, MAX_GOOD_LEN);
    
    //ввод названия магазина
    move(2 + 2, 2 + 32);
    getnstr(buffer2, MAX_STORE_LEN - 1);
    buffer2[MAX_STORE_LEN] = '\0';
    strncpy(price->store, buffer2, MAX_STORE_LEN);
    
    //ввод цены
    move(2 + 3, 2 + 15);
    getnstr(buffer3, 20);
    price->good_price = atof(buffer3);
        
    //не дает писать текст в цене, числа и пустые строки в product и store 
    return (strlen(price->good) > 0 && strlen(price->store) > 0 && price->good_price > 0 && atof(price->good) <= 0 && atof(price->store) <= 0);
}

//записать новый продукт
int add_product(const char* filename, PRICE* prices){
    FILE *file = fopen(filename, "rb+");
    if(!file){
        mvprintw(0, 2, "Error opening file! Press any key to exit...");
        refresh();
        getch();
        endwin();
        return 1;
    }

    //чтение заголовка
    HEAD_DB header;
    if (fread(&header, sizeof(HEAD_DB), 1, file) != 1) {
        fclose(file);
        mvprintw(0, 2, "Error reading file! Press any key to exit...");
        refresh();
        getch();
        endwin();
        return 1;
    }

    header.transaction_num++;
    int count = header.num_structures;
    
    //запись нового товара
    PRICE new_product;
    while(!input_price(&new_product, count, count)){
        mvprintw(6, 2, "Input error! Press any key to re-enter.");
        refresh();
        getch();
        clear();
        refresh();
    }
    fseek(file, count * sizeof(PRICE), SEEK_CUR); //SEEK_CUR - от текущей позиции
    fwrite(&new_product, sizeof(PRICE), 1, file);
    header.num_structures++;
    count++;

    mvprintw(6, 2, "Press any key to exit to menu: ");
    refresh();
    getch();
    clear();

    update_checksum(file, prices, &header, count);
    //обновляем хедер
    fseek(file, 0, SEEK_SET); //SEEK_SET - начало файла
    fwrite(&header, sizeof(HEAD_DB), 1, file);

    fclose(file);
    return 0;
}

//показать DB
int show_db(const char* filename, PRICE* prices){
    FILE *file = fopen(filename, "rb+");
    if(!file){
        mvprintw(0, 2, "Error opening file! Press any key to exit...");
        refresh();
        getch();
        endwin();
        return 1;
    }

    //чтение заголовка
    HEAD_DB header;
    if (fread(&header, sizeof(HEAD_DB), 1, file) != 1) {
        fclose(file);
        mvprintw(0, 2, "Error reading file! Press any key to exit...");
        refresh();
        getch();
        endwin();
        return 1;
    }

    header.transaction_num++;
    int count = header.num_structures;

    //чтение данных PRICE
    if (fread(prices, sizeof(PRICE), count, file) != count){
        fclose(file);
        mvprintw(0, 2, "Error reading file! Press any key to exit...");
        refresh();
        getch();
        endwin();
        return 1;
    }

    mvprintw(0, 2, "Products from %s:", filename);
    int y = 2;
    for(int i=0; i < count; i++, y+=2){
        mvprintw(y, 2, "Product %d: ", i+1);
        mvprintw(++y, 3, "Name: %s", prices[i].good);
        mvprintw(++y, 2, "Store: %s", prices[i].store);
        mvprintw(++y, 2, "Price: %.2f", prices[i].good_price);
    }

    mvprintw(y, 2, "Press any key to exit to menu: ");
    refresh();
    getch();
    clear();

    //обновляем хедер
    fseek(file, 0, SEEK_SET); //SEEK_SET - начало файла
    fwrite(&header, sizeof(HEAD_DB), 1, file);

    fclose(file);
    return 0;
}

//создание DB
int create_db(const char* filename, PRICE* prices){
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
    
    int price_count = 0;

    //ввод данных
    for(int i = 0; i < n; i++){
        clear();
        while(!input_price(&prices[price_count], i+1, n)){
            mvprintw(6, 2, "Input error! Press any key to re-enter.");
            refresh();
            getch();
            clear();
            refresh();
        }
        price_count++;
        refresh();
    }
    clear();
    refresh();

    //сохранение в файл
    if(save_to_file(filename, &prices[0], price_count) != 0){
        mvprintw(0, 2, "Error saving file! Press any key to exit...");
        refresh();
        getch();
        endwin();
        return 1;
    }

    mvprintw(0, 2, "The data has been saved successfully to %s!", filename);
    mvprintw(2, 2, "Press any key to continue.");
    refresh();
    getch();
    clear();
    return 0;
}

//сценарий когда DB существует
int db_exists(const char* filename, PRICE* prices){
    char choice_char;
    do{
        mvprintw(0, 2, "The database %s found!", filename);
        mvprintw(2, 2, "Select action with database:");
        mvprintw(3, 2, "1 Show database!");
        mvprintw(4, 2, "2 Add new product!");
        mvprintw(5, 2, "3 Find the average price of a product!");
        mvprintw(6, 2, "4 Exit the application");
        mvprintw(8, 2, "Select 1...4 and enter:");
        
        move(8, 2 + 24);
        choice_char = getch();

        switch(choice_char){
            case '1':
                clear();
                show_db(filename, &prices[0]);

                if(!db_exists(filename, &prices[0])) return 0;
                return 1;
            case '2':
                clear();
                add_product(filename, &prices[0]);
                
                if(!db_exists(filename, &prices[0])) return 0;
                return 1;
            case '3':
                clear();
                average_price(filename, &prices[0]);
                
                if(!db_exists(filename, &prices[0])) return 0;
                return 1;
            case '4':
                endwin();
                return 0;
            default:
                mvprintw(10, 2, "Input error! Press any key to re-enter.");
                refresh();
                getch();
                clear();
                refresh();
        }
    } while( !(choice_char == '1' || choice_char == '2' || choice_char == '3' || choice_char == '4') );
}