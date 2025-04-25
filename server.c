#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <sys/socket.h>
#include <sys/types.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include "http_header.h"

#define BUFF_SIZE 8192

char path[512]; // 파일 디렉토리

long get_file_size(FILE *file) { // response로 보낼 파일의 크기를 구하는 함수
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}

void find_path(char* buff) { // 보낼 파일 경로 추출하는 함수
    sscanf(buff, "GET %s HTTP/1.1", path); // 파일 디렉토리를 저장하는 버퍼에 request로 받은 디렉토리 저장
    
    if (strcmp(path, "/") == 0) { // 경로가 루트일 경우 index.html
        strcpy(path, "index.html");
    }
    else { // html 뿐 아니라 .jpg 같은 경우에도 대응되게 수정 필요 /asdf.qwer 꼴이니 .을 기준으로 문자열을 판별하면 될듯?
        char tmp[512];
        strcat(path, ".html");
        strcpy(tmp, path+1);
        strcpy(path, tmp);
    }
}

void send_response(int clt_socket, char* request_buff) { // Response를 보내는 함수, 소켓 + 디렉토리
    // 만들다보니 전체적으로 함수가 너무 거대한데 기능을 따로 빼서 구현해야하나???

    find_path(request_buff); // request에서 경로 추출

    FILE *file = fopen(path, "rb"); // 클라이언트에게 보낼 html 파일
    if(file == NULL) { // 파일이 없을 경우 404 에러 페이지를 출력.
        char not_found[512];
        snprintf(not_found, sizeof(not_found), NOT_FOUND_HEADER);
        send(clt_socket, not_found, strlen(not_found), 0);
        close(clt_socket);
        return;
    }

    size_t file_size = get_file_size(file); // 파일 크기
    char response_buff[BUFF_SIZE]; // response가 담긴 버퍼
    char content_buff[BUFF_SIZE]; // 보낼 파일이 담길 버퍼 
    ssize_t read_bytes = 0; // 읽은 파일의 바이트 크기

    // HTTP 헤더 먼저 전송
    char header[512]; // 전송할 헤더용 버퍼
    snprintf(header, sizeof(header), HTML_HEADER, file_size); // html, jpg, png 등등 파일 종류 따라서 케이스 나누기
    send(clt_socket, header, strlen(header), 0);
    printf("Successfully Sending header:\n%sFile name is %s\nNow Transporting Data...\n", header, path);    
    
    // 데이터 전송
    char test_buff[25]; // 분할전송 테스트용 버퍼

    while((read_bytes = fread(content_buff, 1, BUFF_SIZE, file)) > 0) { // 파일 읽기
        ssize_t sent = 0; // 보낸 파일의 바이트 크기
        while(sent < read_bytes) {
            printf("%ld Bytes Data read\n", read_bytes);
            ssize_t sending_bytes = send(clt_socket, content_buff + sent, read_bytes - sent, 0); // 클라이언트에게 전송한 파일
            printf("%ld Bytes Data sent\n", sending_bytes);
            if (sending_bytes <= 0) { // 파일 전송 실패시 루프 탈출
                printf("send failed");
                break;
            }
            sent = sent + sending_bytes; // 보낸 바이트 크기 증가
        }
    }

    fclose(file);

    printf("\r\nSuccessfully sent response to client.\nClosing connection...\r\n");
    close(clt_socket);
}

int main(int argc, char **argv) {
    
    int svr_socket; //서버 소켓
    int clt_socket; // 클라이언트 소켓
    int sin_size; // 클라이언트 주소 구조체의 크기
    int recv_len; // receive의 길이
    char request_buff[BUFF_SIZE];

    svr_socket = socket(AF_INET, SOCK_STREAM, 0);
    // AF_INET = IPv4를 쓰겠다. SOCK_STREAM = TCP 통신을 쓰겠다.

    struct sockaddr_in svr_address; // 서버 주소 구조체
    struct sockaddr_in clt_address; // 클라이언트 주소 구조체

    svr_address.sin_family = AF_INET; // 서버 주소로 IPv4를 쓰겠다.
    svr_address.sin_addr.s_addr = htonl(INADDR_ANY);
    // htonl = host to network long, 바이트 순서를 통일 시키겠다. INADDR_ANY = 서버 주소로 모든 IP를 허용한다.
    svr_address.sin_port = htons(atoi(argv[1])); // 입력에 따라 포트 번호를 바꾸겠다.

    if(bind(svr_socket, (struct sockaddr *)&svr_address, sizeof(svr_address)) == -1) {
        printf("bind error!!\n");
        exit(1);
    }
    else {
        printf("binding success...\n");
    }
    // 소켓에 주소를 할당한다. bind에 실패할 경우 에러를 출력한다.

    if(listen(svr_socket, 5) == -1) {
        printf("listen error!!\n");
        exit(1);
    }
    else {
        printf("Port number %s is now listening....\n", argv[1]);
    }// 클라이언트의 접속을 기다린다. listen에 실패할 경우 에러를 출력한다.
    
   
    sin_size = sizeof(struct sockaddr_in); // 클라이언트 주소 구조체의 크기를 저장한다.


    while (1) {
        if ((clt_socket = accept(svr_socket, (struct sockaddr *)&clt_address, &sin_size)) == -1) {
            printf("socket error!!\n");
        } // 클라이언트의 접속을 받아들인다. accept에 실패할 경우 에러를 출력한다.
        printf("server : got connection from %s\n", inet_ntoa(clt_address.sin_addr));
        // 접속한 클라이언트의 IP 주소를 출력한다.
        
        recv_len = recv(clt_socket, request_buff, sizeof(request_buff), 0);
        printf("<Request>\n%s", request_buff); // Request를 출력한다.

        send_response(clt_socket, request_buff); // response를 보낸다.
    }  
    close(svr_socket);
    return 0;
}

/*  목표
    서버를 열자 - 성공!
    response를 구현하자
    -> html 파일을 읽어서 브라우저로 보내기?
    -> 에러 페이지?
    -> 특정 주소로 들어가면 다른 페이지로 어떻게 이동하지? -> send_response 함수를 수정하면 되나?
        ㄴ request로 오는 문자열에서 경로 정보를 따로 뽑아서 사용하자
    -> 분할전송 구현 성공
        ㄴ 그러면 이제 데이터 타입을 구분해서 보낼 수 있게??
*/

/*  만들면서 생겼던 일들

    blocking 함수?
    read와 recv의 차이는?
        ㄴ 네트워크 통신에서는 recv 사용. recv의 인자를 0으로 하면 read와 거의 동일하다.
    Disconnect 메세지 출력 안됨 아마 위쪽에서 멈춰있는 것 같은데... 
    -> 오 모바일로는 크롬창을 끄니까 서버 접속이 끊김!!
    PC로도 다른 브라우저로 하니까 접속이 끊긴다 성공적인듯.
    근데 메세지가 두번 나옴, 접속이 두번? 루프를 한번 더 돌았나?
        ㄴ 일단 한번 보내고 접속 종료하기
    버퍼 크기가 8KB인데 8KB를 넘는 이미지 같은건 어떻게 전송하지?
        ㄴ 8KB 단위로 나눠서 반복 전송?
    
*/
