#include "price.h"
//#include "db_head.h"

//сохранение DB в файл
int save_to_file(const char* filename, PRICE* prices, int count);
//ввод данных для одной структуры
int input_price(PRICE* price, int iteration, int n);
//создание DB
int create_db(const char* filename, PRICE* prices, int* price_count);
//средняя цена товара
int average_price();
//записать новый продукт
int add_product();
//показать DB
int show_db();
//сценарий когда DB существует
int db_exists();