#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>


int main(int argc, char *argv[]) {
    if (argc != 3) {
        return EXIT_FAILURE;
    }

    const char *ip_address = argv[1];
    int port_number = atoi(argv[2]);

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_details;
    memset(&server_details, 0, sizeof(server_details));
    server_details.sin_family = AF_INET;
    server_details.sin_port = htons(port_number);

    if (inet_pton(AF_INET, ip_address, &server_details.sin_addr) <= 0) {
        exit(EXIT_FAILURE);
    }

    if (connect(client_socket, (struct sockaddr *)&server_details, sizeof(server_details)) < 0) {
        exit(EXIT_FAILURE);
    }

    int32_t input_number;
    while (scanf("%d", &input_number) == 1) {
        int32_t little_endian_num = htole32(input_number);

        ssize_t bytes_written = write(client_socket, &little_endian_num, sizeof(little_endian_num));
        if (bytes_written != sizeof(little_endian_num)) {
            exit(EXIT_FAILURE);
        }

        int32_t server_response;
        ssize_t bytes_read = read(client_socket, &server_response, sizeof(server_response));
        if (bytes_read == 0) {
            break;
        } else if (bytes_read < 0) {
            exit(EXIT_FAILURE);
        } else if (bytes_read != sizeof(server_response)) {
            continue;
        }

        int32_t result = le32toh(server_response);

        printf("%d ", result);
    }

    close(client_socket);
    return EXIT_SUCCESS;
}