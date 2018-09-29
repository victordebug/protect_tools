#ifndef __CRC32_H_
#define __CRC32_H_



unsigned long int crc32(const void *__data, int data_size);


unsigned long int get_file_crc32(char *_file_name);





#endif