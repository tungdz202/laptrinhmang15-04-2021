#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

FILE* db;
errno_t err;
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

	char buf[256], line[256], user[32], pass[32], tmp[32];
	while (1) {
		FD_ZERO(&fdread);
		FD_SET(listener, &fdread);

		ret = select(0, &fdread, 0, 0, 0);
		if (ret < 0) break;
		if (FD_ISSET(listener, &fdread)) {
			SOCKET client = accept(listener, 0, 0);
			const char* loginRequest = "Enter your username and password!\n";
			send(client, loginRequest, strlen(loginRequest), 0);
			buf[ret] = 0;
			int n = sscanf(buf, "%s %s %s", user, pass, tmp);
			if (n == 2) {
				err = fopen_s(&db, "C:\\Users\\Admin\\Downloads\\NetworkProgramming\\db.txt", "r");
				int found = 0;
				while (!feof(db)) {
					fgets(line, sizeof(buf), db);
					if (!strcmp(buf, line)) {
						found = 1;
						break;
					}
				}
				fclose(db);
				if (found) {
					const char* msg = "Dang nhap thanh cong. Hay nhap lenh de thuc hien.\n";
					send(client, msg, strlen(msg), 0);
					ret = recv(client, buf, sizeof(buf), 0);
					if (ret <= 0) {
						closesocket(client);
						break;
					}
					buf[ret - 1] = 0;
					char cmd[256];
					sprintf(cmd, "%s > C:\\Users\\Admin\\Downloads\\NetworkProgramming\\out.txt");
					system(cmd);
					fclose(db);
					break;
				}
				else {
					const char* msg = "Sai tai khoan. Hay nhap lai.\n";
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
