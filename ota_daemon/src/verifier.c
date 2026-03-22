#include <openssl/evp.h>
#include <openssl/pem.h>

#include <openssl/evp.h>

int calculate_hash(const char *file_path, char *output_hash)
{
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        printf("[OTA][ERROR] Failed to open file for hashing\n");
        return 0;
    }

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        fclose(file);
        return 0;
    }

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1) {
        fclose(file);
        EVP_MD_CTX_free(ctx);
        return 0;
    }

    unsigned char buffer[1024];
    size_t len;

    while ((len = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        EVP_DigestUpdate(ctx, buffer, len);
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;

    EVP_DigestFinal_ex(ctx, hash, &hash_len);

    fclose(file);
    EVP_MD_CTX_free(ctx);

    // Convert to hex string
    for (unsigned int i = 0; i < hash_len; i++) {
        sprintf(output_hash + (i * 2), "%02x", hash[i]);
    }

    output_hash[hash_len * 2] = '\0';

    return 1;
}

int verify_signature(const char *file_path, const char *sig_path)
{
    FILE *fp = fopen(file_path, "rb");
    FILE *sig_fp = fopen(sig_path, "rb");

    if (!fp || !sig_fp) {
        printf("File open error\n");
        return 0;
    }

    FILE *pub = fopen("public.pem", "r");
    EVP_PKEY *pubkey = PEM_read_PUBKEY(pub, NULL, NULL, NULL);

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestVerifyInit(ctx, NULL, EVP_sha256(), NULL, pubkey);

    unsigned char buffer[1024];
    size_t len;

    while ((len = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        EVP_DigestVerifyUpdate(ctx, buffer, len);
    }

    unsigned char sig[256];
    size_t sig_len = fread(sig, 1, sizeof(sig), sig_fp);

    int result = EVP_DigestVerifyFinal(ctx, sig, sig_len);

    EVP_MD_CTX_free(ctx);
    fclose(fp);
    fclose(sig_fp);
    fclose(pub);

    return result == 1;
}
