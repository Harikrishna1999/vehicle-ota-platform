#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <unistd.h>

char server_response[4096];

/* ✅ Safe write callback */
size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t total_size = size * nmemb;

    if (strlen(server_response) + total_size < sizeof(server_response)) {
        strncat(server_response, ptr, total_size);
    }

    return total_size;
}

int check_update(char *url, char *hash)
{
    CURL *curl;
    CURLcode res;

    char signature[1024] = {0};

    memset(server_response, 0, sizeof(server_response));

    curl = curl_easy_init();
    if (!curl) {
        printf("[OTA][ERROR] CURL init failed\n");
        return 0;
    }

    printf("[OTA] Connecting to server...\n");

    /* ✅ HTTPS URL */
    curl_easy_setopt(curl, CURLOPT_URL, "https://127.0.0.1:5000/update");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

    /* ✅ TLS config */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    curl_easy_setopt(curl, CURLOPT_CAINFO, "/etc/ota/cert.pem");

    curl_easy_setopt(curl, CURLOPT_PINNEDPUBLICKEY,
                     "sha256//rlESURbwvCnx1iGkFhR3z+9WjfHH7f+5YX1rJfSZp7Y=");

    /* ✅ Retry mechanism */
    int retries = 3;

    while (retries--) {
        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            printf("[OTA] HTTP request successful\n");
            break;
        } else {
            printf("[OTA][WARN] HTTP failed: %s\n", curl_easy_strerror(res));
            printf("[OTA][INFO] Retrying... (%d left)\n", retries);
            sleep(2);
        }
    }

    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        printf("[OTA][ERROR] All retries failed\n");
        return 0;
    }

    printf("[OTA] Server Response: %s\n", server_response);

    /* ✅ JSON parsing */
    char *url_ptr  = strstr(server_response, "\"url\":\"");
    char *hash_ptr = strstr(server_response, "\"hash\":\"");
    char *sig_ptr  = strstr(server_response, "\"signature\":\"");

    if (url_ptr && hash_ptr && sig_ptr)
    {
        sscanf(url_ptr,  "\"url\":\"%[^\"]\"", url);
        sscanf(hash_ptr, "\"hash\":\"%[^\"]\"", hash);
        sscanf(sig_ptr,  "\"signature\":\"%[^\"]\"", signature);
    }
    else
    {
        printf("[OTA][ERROR] JSON parsing failed\n");
        return 0;
    }

    /* ✅ Convert HEX signature → binary file */
    FILE *sig_file = fopen("firmware.sig", "wb");

    if (!sig_file) {
        printf("[OTA][ERROR] Failed to create signature file\n");
        return 0;
    }

    for (int i = 0; i < strlen(signature); i += 2) {
        unsigned int byte;
        sscanf(&signature[i], "%2x", &byte);
        fputc(byte, sig_file);
    }

    fclose(sig_file);

    printf("[OTA] Signature saved to firmware.sig\n");

    /* ✅ Debug prints */
    printf("[OTA] Parsed URL: %s\n", url);
    printf("[OTA] Parsed Hash: %s\n", hash);
    printf("[OTA] Signature length: %lu\n", strlen(signature));

    return 1;
}
