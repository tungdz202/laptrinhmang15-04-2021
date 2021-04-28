#include <stdio.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

int main() {
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	bind(listener, (SOCKADDR*)&addr, sizeof(addr));
	listen(listener, 5);

	fd_set fdread;
	int ret;
	SOCKET clients[64];
	int numClients = 0;
	char buf[256], cmd[32], id[32], tmp[32];

	while (1) {
		FD_ZERO(&fdread);
		FD_SET(listener, &fdread);
		int i = 0;
		while (i < numClients) {
			FD_SET(clients[i++], &fdread);
		}

		ret = select(0, &fdread, 0, 0, 0);
		if (ret < 0) break;

		if (FD_ISSET(listener, &fdread)) {
			SOCKET client = accept(listener, 0, 0);
			ret = recv(clients[i], buf, sizeof(buf), 0);
			if (ret <= 0) continue;

			buf[ret] = 0;
			int n = sscanf(buf, "%s %s %s", cmd, id, tmp);

			if (n == 2) {
				if (!strcmp(cmd, "client_id:")) {
					printf("New client connected: %d\n", client);
					const char* msg = "Dung cu phap. Hay nhap tin nhan de chuyen tiep.\n";
					send(client, msg, strlen(msg), 0);
					clients[numClients++] = client;

					while (1) {
						ret = recv(client, buf, sizeof(buf), 0);
						if (ret <= 0) {
							break;
						}

						buf[ret] = 0;

						char sendbuf[256];
						sprintf(sendbuf, "%s: %s", id, buf);

						for (i = 0; i < numClients; i++) {
							if (client != clients[i]) {
								send(clients[i], sendbuf, strlen(sendbuf), 0);
							}
						}
					}			
				}
				else {
					const char* msg = "Sai cu phap. Hay nhap lai.\n";
					send(client, msg, strlen(msg), 0);
				}
			}
			else {
				const char* msg = "Sai cu phap. Hay nhap lai.\n";
				send(client, msg, strlen(msg), 0);
			}			
		}
	}
	return 0;
}