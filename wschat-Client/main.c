#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>

void ErrorHandling(char* message);
void removeEnter(char* str);
unsigned __stdcall Thread_Send(void* h);
unsigned __stdcall Thread_Recv(void* h);

typedef struct handler {
	SOCKET* socket;
	int* sendflag;
	int* recvflag;
} Handler;

int main(int argc, char* argv[]) {

	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	char msg[50];
	int strlen;
	int sendflag;
	int recvflag;

	if (argc != 3) {
		printf("Usage: %s <IP> <port>\n", argv[0]); // 실행시 IP, Port 입력 필요
		exit(1);
	}




	//연결부
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		ErrorHandling("socket() error!");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	servAddr.sin_port = htons(atoi(argv[2]));

	if (connect(hSocket, (SOCKADDR*)& servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("connect() error!");

	printf("클라이언트가 [ip: %s], [port: %s] 에 연결됨\n", argv[1], argv[2]);

	strlen = recv(hSocket, msg, sizeof(msg), 0);
	if (strlen == -1)
		ErrorHandling("recv() error!");
	printf("%s \n", msg);

	sendflag = 1;
	recvflag = 1;



	//대화부
	Handler HforTh;
	HforTh.socket = &hSocket;
	HforTh.sendflag = &sendflag;
	HforTh.recvflag = &recvflag;
	_beginthreadex(NULL, 0, Thread_Send, (void*)&HforTh, 0, NULL); //송신 스레드 생성
	_beginthreadex(NULL, 0, Thread_Recv, (void*)&HforTh, 0, NULL); //수신 스레드 생성

	while (sendflag == 1 || recvflag == 1);



	//연결 해제부
	printf("연결을 종료합니다.\n");
	closesocket(hSocket);
	WSACleanup();

	return 0;
}

unsigned __stdcall Thread_Send(void* h) {
	char msg[50];
	Handler* handler = (Handler*)h;

	while (*(handler->recvflag) == 1) {
		printf("나: ");
		rewind(stdin);
		fgets(msg, sizeof(msg), stdin);
		removeEnter(msg);
		send(*(handler->socket), msg, sizeof(msg), 0);

		if (strcmp(msg, "quit") == 0) {
			*(handler->sendflag) = 0;
			return 0;
		}
	}
	*(handler->sendflag) = 0;
	return 0;
}

unsigned __stdcall Thread_Recv(void* h) {
	char msg[50];
	int strlen;
	Handler* handler = (Handler*)h;

	while (*(handler->sendflag) == 1) {
		strlen = recv(*(handler->socket), msg, sizeof(msg), 0);
		if (strlen == -1 && *(handler->sendflag) == 1)
			printf("상대방으로부터 메세지가 정상적으로 수신되지 않았습니다.\n");
		else if (strcmp(msg, "quit") == 0 && *(handler->sendflag) == 1) {
			printf("상대방이 연결을 종료했습니다.\n");
			*(handler->recvflag) = 0;
			return 0;
		}
		else if (*(handler->sendflag) == 1)
			printf("상대방: %s \n", msg);
	}
	*(handler->recvflag) = 0;
	return 0;
}

void removeEnter(char* str) {
	str[strlen(str) - 1] = '\0';
}

void ErrorHandling(char* message) {
	fputs(message, stderr);
	fputs("\n", stderr);
	exit(1);
}