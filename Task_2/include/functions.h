#include "price.h"
//#include "db_head.h"

//сохранение DB в файл
int save_to_file(const char* filename, PRICE* prices, int count);
//ввод данных для одной структуры
int input_price(PRICE* price, int iteration, int n);
//создание DB
int create_db(const char* filename, PRICE* prices);
//средняя цена товара
int average_price(const char* filename, PRICE* prices);
//записать новый продукт
int add_product(const char* filename, PRICE* prices);
//показать DB
int show_db(const char* filename, PRICE* prices);
//сценарий когда DB существует
int db_exists(const char* filename, PRICE* prices);
//обновляет контрольную сумму
int update_checksum(FILE* file, PRICE* prices, HEAD_DB* header, int count);
//является ли строка повторной 
int is_copy(PRICE* prices, char* buffer, int n);