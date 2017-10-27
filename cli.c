#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include "file_store.h"

#define HELP_TEXT "usage: filestore [<options>] <command> [<args>]\n\n"         \
    "These are common filestore commands for various situations:\n\n"           \
    "  store <config-path>       Store data in file system\n"                   \
    "  help [cmd]                display help for [cmd]\n\n"                    \
    "options:\n"                                                                \
    "  -h, --help                output usage information\n"                    \
    "  -v, --version             output the version number\n"                   \

#define CLI_VERSION "libfilestore-0.0.0alpha"

int main(int argc, char *argv[]) {

        int status = 0;
        int c;
        int index = 0;
        char *file_path;

        static struct option cmd_options[] = {
            {"version", no_argument,  0, 'v'},
            {"help", no_argument,  0, 'h'},
            {"path", required_argument,  0, 'p'},
            {0, 0, 0, 0}
        };

        opterr = 0;

        while ((c = getopt_long_only(argc, argv, "vhp:",
                                     cmd_options, &index)) != -1) {
            switch (c) {
                case 'p':
                    file_path = optarg;
                    break;
                case 'v':
                    printf(CLI_VERSION "\n\n");
                    exit(0);
                    break;
                case 'h':
                    printf(HELP_TEXT);
                    exit(0);
                    break;
            }
        }

        int command_index = optind;

        char *command = argv[command_index];
        if (!command || (strcmp("help",  command) == 0)) {
            printf(HELP_TEXT);
            return 0;
        }


        if (strcmp(command, "store") == 0) {
            char *data_path = argv[command_index + 1];
            FILE *data_file = NULL;
            char store_directory[BUFSIZ];
            memset(store_directory, '\0', BUFSIZ);

            if (data_path) {
                data_file = fopen(data_path, "r");
            } else {
                data_file = stdin;
            }

            if (!file_path) {
                sprintf(store_directory, "./");
            } else {
                sprintf(store_directory, "%s", file_path);
            }

            int ret = 0;
            uint8_t *data_hash = NULL;
            ret = get_file_hash(fileno(data_file), &data_hash);
            ret = store_file(fileno(data_file), (char *)data_hash, store_directory);

            if (ret == 0) {
                printf("Successfully Stored data\n");
            }

            if (data_file) {
                fclose(data_file);
            }
        }

        return status;
}
