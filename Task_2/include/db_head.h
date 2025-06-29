#pragma once

typedef struct{
    char signature[4];
    uint32_t transaction_num;
    uint32_t num_structures;
    uint32_t checksum;
} HEAD_DB;
