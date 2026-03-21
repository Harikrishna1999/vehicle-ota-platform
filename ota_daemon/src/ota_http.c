#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

char server_response[1024];

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    strncat(server_response, ptr, size * nmemb);
    return size * nmemb;
}

int check_update(char *url, char *hash)
{
    CURL *curl;
    CURLcode res;

    memset(server_response, 0, sizeof(server_response));

    curl = curl_easy_init();

    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:5000/update");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);

        if(res != CURLE_OK)
        {
            printf("HTTP failed\n");
            return 0;
        }

        printf("Server Response: %s\n", server_response);

        // ✅ Parsing MUST be inside function
        char *url_ptr = strstr(server_response, "\"url\":\"");
        char *hash_ptr = strstr(server_response, "\"hash\":\"");

        if(url_ptr && hash_ptr)
        {
            sscanf(url_ptr, "\"url\":\"%[^\"]\"", url);
            sscanf(hash_ptr, "\"hash\":\"%[^\"]\"", hash);
        }
        else
        {
            printf("JSON parsing failed\n");
            return 0;
        }

        return 1;
    }

    return 0;
}
