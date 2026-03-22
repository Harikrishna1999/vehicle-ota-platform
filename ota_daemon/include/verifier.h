#ifndef VERIFIER_H
#define VERIFIER_H

int verify_signature(const char *file_path, const char *sig_path);
int calculate_hash(const char *file_path, char *output_hash);

#endif
