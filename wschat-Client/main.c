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
		printf("Usage: %s <IP> <port>\n", argv[0]); // ����� IP, Port �Է� �ʿ�
		exit(1);
	}

	//�����
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

	printf("Ŭ���̾�Ʈ�� [ip: %s], [port: %s] �� �����\n", argv[1], argv[2]);

	strlen = recv(hSocket, msg, sizeof(msg), 0);
	if (strlen == -1)
		ErrorHandling("recv() error!");
	printf("%s \n", msg);

	//��ȭ����
	while (1) {
		//������
		printf("��: ");
		rewind(stdin);
		fgets(msg, sizeof(msg), stdin);
		removeEnter(msg);
		send(hSocket, msg, sizeof(msg), 0);

		if (strcmp(msg, "quit") == 0)
			break;

		//�ޱ�
		strlen = recv(hSocket, msg, sizeof(msg), 0);
		if (strlen == -1)
			printf("�������κ��� �޼����� ���������� ���ŵ��� �ʾҽ��ϴ�.\n");
		else if (strcmp(msg, "quit") == 0)
			break;
		else
			printf("����: %s \n", msg);
	}

	//���� ������
	printf("������ �����մϴ�.\n");
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