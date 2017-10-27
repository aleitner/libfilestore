#include <ctype.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <nettle/ripemd160.h>
#include <nettle/sha.h>
#include <nettle/base16.h>

#include <limits.h>
#include <sys/stat.h>

int get_file_hash(int fd, uint8_t **hash);
int store_file(int fd, char *hash, char *dir);
static void _mkdir(const char *dir);
