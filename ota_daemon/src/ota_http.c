#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <unistd.h>

char server_response[4096];

// ✅ SAFE write callback
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

    memset(server_response, 0, sizeof(server_response));

    curl = curl_easy_init();

    if (!curl) {
        printf("[OTA][ERROR] CURL init failed\n");
        return 0;
    }

    printf("[OTA] Connecting to server...\n");

    // ✅ HTTPS URL
    curl_easy_setopt(curl, CURLOPT_URL, "https://127.0.0.1:5000/update");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

    // ✅ TLS (TEMPORARY DISABLE for self-signed)
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    // 🔁 Retry logic
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

    // ✅ JSON parsing
    char *url_ptr = strstr(server_response, "\"url\":\"");
    char *hash_ptr = strstr(server_response, "\"hash\":\"");

    if (url_ptr && hash_ptr) {
        sscanf(url_ptr, "\"url\":\"%[^\"]\"", url);
        sscanf(hash_ptr, "\"hash\":\"%[^\"]\"", hash);
    } else {
        printf("[OTA][ERROR] JSON parsing failed\n");
        return 0;
    }

    printf("[OTA] Parsed URL: %s\n", url);
    printf("[OTA] Parsed Hash: %s\n", hash);

    return 1;
}
