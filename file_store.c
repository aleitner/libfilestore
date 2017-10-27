#include "file_store.h"

int store_file(int fd, char *hash, char *dir) {
    // Initialize variables
    char directory[BUFSIZ];           // directory structure
    char store_file_location[BUFSIZ]; // Complete path where data will be stored
    char buf[BUFSIZ];                 // Buffer used for read/write operations
    int bytes_read = 0;               // Bytes read from read operations
    int bytes_written = 0;            // Bytes written from write operations

    // Create directory string
    memset(directory, '\0', BUFSIZ);
    sprintf(directory, "%s/%.2s/%.2s/", dir, hash, hash+2);

    // Create file store location string
    memset(store_file_location, '\0', BUFSIZ);
    sprintf(store_file_location, "%s%s", directory, hash+4);
    printf("Storing data to path: %s\n", store_file_location);

    // Make directories
    _mkdir(directory);

    // Make file
    FILE* file_ptr = fopen(store_file_location, "w");
    if (!file_ptr) {
        return 1;
    }

    // Read from original data and write to new location
    do {
        memset(buf, '\0', BUFSIZ);
        bytes_read = read(fd, buf, BUFSIZ);

        bytes_written = 0;
        do {
            bytes_written += write(fileno(file_ptr), buf + bytes_written, bytes_read - bytes_written);
        } while (bytes_written < bytes_read);

    } while(bytes_read > 0);
    
    // Close the file
    if (file_ptr) {
        fclose(file_ptr);
    }

    return 0;
}

static void _mkdir(const char *dir) {
        char tmp[PATH_MAX];
        char *p = NULL;
        size_t len;

        len = snprintf(tmp, sizeof(tmp),"%s",dir);
        if(tmp[len - 1] == '/')
                tmp[len - 1] = 0;
        for(p = tmp + 1; *p; p++)
                if(*p == '/') {
                        *p = 0;
                        mkdir(tmp, S_IRWXU);
                        *p = '/';
                }
        mkdir(tmp, S_IRWXU);
}


int get_file_hash(int fd, uint8_t **hash) {
    ssize_t read_len = 0;
    uint8_t read_data[BUFSIZ];
    uint8_t prehash_sha256[SHA256_DIGEST_SIZE];
    uint8_t hash_as_hex[RIPEMD160_DIGEST_SIZE];

    struct sha256_ctx sha256ctx;
    sha256_init(&sha256ctx);

    struct ripemd160_ctx ripemd160ctx;
    ripemd160_init(&ripemd160ctx);

    do {
        read_len = read(fd, read_data, BUFSIZ);

        if (read_len == -1) {
            printf("Error reading file for hashing\n");
            return 1;
        }

        sha256_update(&sha256ctx, read_len, read_data);

        memset(read_data, '\0', BUFSIZ);
    } while (read_len > 0);
    lseek(fd, 0, SEEK_SET);

    memset(prehash_sha256, '\0', SHA256_DIGEST_SIZE);
    sha256_digest(&sha256ctx, SHA256_DIGEST_SIZE, prehash_sha256);

    ripemd160_update(&ripemd160ctx, SHA256_DIGEST_SIZE, prehash_sha256);
    memset(hash_as_hex, '\0', RIPEMD160_DIGEST_SIZE);
    ripemd160_digest(&ripemd160ctx, RIPEMD160_DIGEST_SIZE, hash_as_hex);

    size_t encode_len = BASE16_ENCODE_LENGTH(RIPEMD160_DIGEST_SIZE);
    *hash = calloc(encode_len + 1, sizeof(uint8_t));
    if (!*hash) {
        printf("Error converting hash data to string\n");
        return 1;
    }

    base16_encode_update(*hash, RIPEMD160_DIGEST_SIZE, hash_as_hex);

    return 0;
}
