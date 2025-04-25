#define NOT_FOUND_HEADER "HTTP/1.1 404 Not Found\r\n""Content-Type: text/html\r\n""Content-Length: 0\r\n""\r\n"
#define HTML_HEADER "HTTP/1.1 200 OK\r\n""Content-Type: text/html\r\n""Content-Length: %ld\r\n""\r\n"

#define PDF_HEADER "HTTP/1.1 200 OK\r\n""Content-Type: application/pdf\r\n""Content-Length: %ld\r\n""\r\n"
#define ZIP_HEADER "HTTP/1.1 200 OK\r\n""Content-Type: application/zip\r\n""Content-Length: %ld\r\n""\r\n"

#define JPG_HEADER "HTTP/1.1 200 OK\r\n""Content-Type: img/jpg\r\n""Content-Length: %ld\r\n""\r\n"
#define JPEG_HEADER "HTTP/1.1 200 OK\r\n""Content-Type: img/jpeg\r\n""Content-Length: %ld\r\n""\r\n"
#define PNG_HEADER "HTTP/1.1 200 OK\r\n""Content-Type: img/png\r\n""Content-Length: %ld\r\n""\r\n"

#define MP3_HEADER "HTTP/1.1 200 OK\r\n""Content-Type: audio/mpeg\r\n""Content-Length: %ld\r\n""\r\n"
#define WAV_HEADER "HTTP/1.1 200 OK\r\n""Content-Type: audio/wav\r\n""Content-Length: %ld\r\n""\r\n"

#define MP4_HEADER "HTTP/1.1 200 OK\r\n""Content-Type: video/mp4\r\n""Content-Length: %ld\r\n""\r\n"
