#include <openssl/ssl.h>
#include <openssl/err.h>

void init_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

SSL_CTX* create_context() {
    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);
    return ctx;
}

int main() {
    init_openssl();
    SSL_CTX *ctx = create_context();
    
    SSL *ssl = SSL_new(ctx);
    BIO *bio = BIO_new(BIO_s_connect());
    BIO_set_conn_hostname(bio, "example.com:4433");
    
    SSL_set_bio(ssl, bio, bio);
    SSL_connect(ssl);
    
    char *data = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
    SSL_write(ssl, data, strlen(data));
    
    char response[1024];
    SSL_read(ssl, response, 1024);
    printf("Response: %s\n", response);
    
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    return 0;
}
