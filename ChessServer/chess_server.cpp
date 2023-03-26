#include <iostream>
#include <array>
#include <WS2tcpip.h>

// 총 플레이어의 수
#define MAX_PLAYER 10

using namespace std;
#pragma comment (lib, "WS2_32.LIB")

const short SERVER_PORT = 4000;
const int BUFSIZE = 256;

struct all_tokens {
	int playerNum;
	int type = 0;	// 1.up, 2.down, 3.left, 4.right
	array<float, MAX_PLAYER> x{ 0 };
	array<float, MAX_PLAYER> y{ 0 };
	array<int, MAX_PLAYER>loginPlayer{ 0 };
};

all_tokens p;

struct socket_info
{
	WSAOVERLAPPED overlapped;
	WSABUF dataBuffer;
	SOCKET socket;
	all_tokens myPawn;
	int receiveBytes;
	int sendBytes;
};

// 필요 함수 목록
void init_tockens();
void error_display(const char* msg, int err_no);
void CALLBACK  CompletionROUTINE(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);

// 메인문 시작
int main()
{
	init_tockens();  // 토큰 위치 초기화

	wcout.imbue(locale("korean"));
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERVER_PORT);

	bind(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(s_socket, SOMAXCONN);

	SOCKADDR_IN client_addr;
	int size_client_addr = sizeof(client_addr);
	SOCKET c_socket = accept(s_socket, reinterpret_cast<sockaddr*>(&client_addr), &size_client_addr);

	for (;;) {
		char recv_buf[BUFSIZE];
		WSABUF mybuf_r;
		mybuf_r.buf = recv_buf; mybuf_r.len = BUFSIZE;
		DWORD recv_byte;
		DWORD recv_flag = 0;
		WSARecv(c_socket, &mybuf_r, 1, &recv_byte, &recv_flag, 0, 0);
		cout << "Client Sent [" << recv_byte << "bytes] : " << recv_buf << endl;
		all_tokens* recv_p = reinterpret_cast<all_tokens*>(recv_buf);
		cout << recv_p->playerNum << "Player" << endl;
		cout << "Client Sent move_pawn structure with x = " << p.x[recv_p->playerNum - 1] << " and y = " << p.y[recv_p->playerNum - 1] << endl;
		if (recv_p->type == 0) {
		}
		else if (recv_p->type == 1) {
			if (p.y[recv_p->playerNum-1] <= 0.225 * 8)
				p.y[recv_p->playerNum-1] += 0.225;
		}
		else if (recv_p->type == 2) {
			if (p.y[recv_p->playerNum-1] > 0.25)
				p.y[recv_p->playerNum-1] -= 0.225;
		}
		else if (recv_p->type == 3) {
			if (p.x[recv_p->playerNum-1] > 0.25)
				p.x[recv_p->playerNum-1] -= 0.225;
		}
		else if (recv_p->type == 4) {
			if (p.x[recv_p->playerNum-1] < 1.75)
				p.x[recv_p->playerNum-1] += 0.225;
		}
		p.loginPlayer[recv_p->playerNum - 1] = 1;
		DWORD sent_byte;
		WSABUF mybuf[1];
		mybuf[0].buf = reinterpret_cast<char*>(&p); mybuf[0].len = sizeof(all_tokens);
		int ret = WSASend(c_socket, mybuf, 1, &sent_byte, 0, 0, 0);
		if (0 != ret) {
			int err_no = WSAGetLastError();
			error_display("WSASend : ", err_no);
		}
	}
	closesocket(c_socket);
	closesocket(s_socket);
	WSACleanup();
}

void init_tockens (){
	for (int i = 0; i < 8; ++i) {   // 1 ~ 8번 말 위치 정해주기
		p.x[i] = 0.22 + 0.225 * i;
		p.y[i] = 0.45;
	}
	for (int i = 8; i < 10; ++i) {  // 9 ~ 10번 말 위치 정해주기
		p.x[i] = 0.22 + 0.225 * i;
		p.y[i] = 0.45 - 0.225;
	}
}

void error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"에러 " << lpMsgBuf << std::endl;
	while (true);
	LocalFree(lpMsgBuf);
}

void CALLBACK  CompletionROUTINE(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	struct socket_info* socketInfo;
	DWORD sendBytes = 0;
	DWORD receiveBytes = 0;
	DWORD flags = 0;

	socketInfo = (struct socket_info*)lpOverlapped;
	memset(&(socketInfo->overlapped), 0x00, sizeof(WSAOVERLAPPED));

}