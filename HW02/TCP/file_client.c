#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

void error_handling(char *message);

int main(int argc, char **argv)
{
    int sock;
    char message[BUFSIZE];
    int str_len;
    struct sockaddr_in serv_addr;

    char file_type='x';
    char file_name[200];
    FILE *fp;


    if(argc!=4){
        printf("Usage : %s <IP> <port> <file name>\n", argv[0]);
        exit(1);
    }

    //socket call
    sock=socket(PF_INET, SOCK_STREAM, 0);

    if(sock == -1){
        error_handling("socket() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    //connect call
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1){
        error_handling("connect() error!");
    }

    /* 파일 입력, 전송 */
    //Input file name
    strcpy(file_name, argv[3]);
    str_len = strlen(file_name);


    // 서버쪽에서 파일 이름과 contents를 구분하기 위해 filename 길이를 보내준다.
    send(sock, &str_len, sizeof(int), 0);

    //  입력 받은 파일 이름을 전송
    send(sock, file_name, str_len, 0);

    fp = fopen(file_name, "rb");
    if(fp == NULL)
        error_handling("no file existance error");

    // 파일 전송
    /* send file */
    int fp_block_sz;
    memset(message, 0, BUFSIZE);
    printf("파일 전송을 시작합니다.\n");

    while((fp_block_sz = fread(message, sizeof(char), BUFSIZE, fp))>0) {
	     str_len = send(sock, message, fp_block_sz, 0);
       if(str_len < 0) {
         error_handling("sending file error");
       }
      memset(message, 0, BUFSIZE);
    }

    printf("파일 %s 이 성공적으로 전송되었습니다!\n", file_name);

    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
