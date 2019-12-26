#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <stdlib.h>

void ErrorHandling(char* message);
void removeEnter(char* str);

int main(int argc, char* argv[]) {

	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	char msg[50];
	int strlen;

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

	//대화시작
	while (1) {
		//보내기
		printf("나: ");
		rewind(stdin);
		fgets(msg, sizeof(msg), stdin);
		removeEnter(msg);
		send(hSocket, msg, sizeof(msg), 0);

		if (strcmp(msg, "quit") == 0)
			break;

		//받기
		strlen = recv(hSocket, msg, sizeof(msg), 0);
		if (strlen == -1)
			printf("상대방으로부터 메세지가 정상적으로 수신되지 않았습니다.\n");
		else if (strcmp(msg, "quit") == 0)
			break;
		else
			printf("상대방: %s \n", msg);
	}

	//연결 해제부
	printf("연결을 종료합니다.\n");
	closesocket(hSocket);
	WSACleanup();

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