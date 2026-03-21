#include <stdio.h>
#include <curl/curl.h>

/*
 * Callback function:
 * This function is called by libcurl whenever data is received.
 * It writes the downloaded data into a file.
 */
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return fwrite(ptr, size, nmemb, stream);
}

/*
 * Function: download_firmware
 * Purpose:
 *  - Download firmware file from server
 *  - Save it as firmware.bin
 */
int download_firmware()
{
    CURL *curl;
    FILE *fp;
    CURLcode res;

    /* Initialize curl session */
    curl = curl_easy_init();

    if(curl)
    {
        /* Open file to save firmware */
        fp = fopen("firmware.bin", "wb");

        if(fp == NULL)
        {
            printf("Error: Unable to create file\n");
            return 0;
        }

        /* Set URL to download firmware */
        curl_easy_setopt(curl, CURLOPT_URL,
            "https://speed.hetzner.de/100MB.bin");

        /* Set callback function to write data */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

        /* Pass file pointer to callback */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        /*
         * Disable SSL verification (ONLY for development)
         * In production, this must be enabled
         */
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        /* Perform download */
        res = curl_easy_perform(curl);

        /* Close file */
        fclose(fp);

        /* Check for errors */
        if(res != CURLE_OK)
        {
            printf("Download failed: %s\n",
                   curl_easy_strerror(res));

            curl_easy_cleanup(curl);
            return 0;
        }

        printf("Firmware downloaded successfully\n");

        /* Cleanup curl */
        curl_easy_cleanup(curl);
    }
    else
    {
        printf("Error: CURL initialization failed\n");
        return 0;
    }

    return 1;
}
