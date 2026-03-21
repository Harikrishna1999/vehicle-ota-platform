#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "ota_http.h"
#include "downloader.h"
#include "verifier.h"
#include "slot_manager.h"

int main()
{
    char url[256];
    char expected_hash[65];
    char calculated_hash[65];

    while(1)
    {
        printf("\n=== OTA Daemon Running ===\n");

        /* Step 1: Check update from server */
        if(check_update(url, expected_hash))
        {
            printf("Update Info Received:\n");
            printf("URL: %s\n", url);
            printf("Expected Hash: %s\n", expected_hash);

            /* Step 2: Download firmware */
            printf("Downloading firmware...\n");

            if(download_firmware())
            {
                printf("Download complete\n");

                /* Step 3: Verify firmware */
                if(verify_firmware(calculated_hash))
                {
                    printf("Calculated Hash: %s\n", calculated_hash);
                    printf("Expected Hash: [%s]\n", expected_hash);

                    /* Step 4: Compare hash */
                    if(strcmp(calculated_hash, expected_hash) == 0)
                    {
                        printf("Firmware VALID\n");


                        /* Step 5: Install firmware */
                        if(install_firmware())
                        {
                            /* Step 6: Switch slot */
                            switch_slot();
                            printf("OTA Update Successful ✅\n");
                        }
                        else
                        {
                            printf("Installation Failed ❌\n");
                            rollback();
                        }
                    }
                    else
                    {
                        printf("Firmware INVALID ❌ → Rejecting update\n");
                    }
                }
                else
                {
                    printf("Verification Failed ❌\n");
                }
            }
            else
            {
                printf("Download Failed ❌\n");
            }
        }
        else
        {
            printf("No update available or server error\n");
        }

        /* Wait before next check */
        sleep(10);
    }

    return 0;
}
