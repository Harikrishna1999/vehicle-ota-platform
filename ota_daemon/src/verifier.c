#include <stdio.h>
#include <openssl/evp.h>
#include <string.h>

int verify_firmware(char *output_hash)
{
    FILE *file = fopen("firmware.bin", "rb");

    if(file == NULL)
    {
        printf("Error: firmware file not found\n");
        return 0;
    }

    EVP_MD_CTX *mdctx;
    const EVP_MD *md;

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;

    md = EVP_sha256();
    mdctx = EVP_MD_CTX_new();

    EVP_DigestInit_ex(mdctx, md, NULL);

    unsigned char buffer[1024];
    int bytesRead;

    while((bytesRead = fread(buffer, 1, sizeof(buffer), file)))
    {
        EVP_DigestUpdate(mdctx, buffer, bytesRead);
    }

    EVP_DigestFinal_ex(mdctx, hash, &hash_len);

    EVP_MD_CTX_free(mdctx);
    fclose(file);

    // Convert to hex string
    for(unsigned int i = 0; i < hash_len; i++)
    {
        sprintf(output_hash + (i * 2), "%02x", hash[i]);
    }

// Remove any accidental newline or spaces
for(int i = 0; output_hash[i]; i++)
{
    if(output_hash[i] == '\n' || output_hash[i] == ' ')
    {
        output_hash[i] = '\0';
        break;
    }
}
    return 1;
}
