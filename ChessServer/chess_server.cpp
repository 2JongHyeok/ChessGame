#include <iostream>
#include <WS2tcpip.h>

// 코드에 필요한 변수들 정의
#define NUM_OF_TOKENS 10

using namespace std;
#pragma comment (lib, "WS2_32.LIB")

const short SERVER_PORT = 4000;
const int BUFSIZE = 256;

struct move_pawn {
    int type;       // 1.up 2.down 3.left 4.right
    float x, y;
};

struct my_tokens {
    float x;
    float y;
};

my_tokens token[NUM_OF_TOKENS];


// 필요 함수 목록
void init_tockens();

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
        move_pawn* p = reinterpret_cast<move_pawn*>(recv_buf);
        cout << "Client Sent move_pawn structure with x = " << p->x << " and y = " << p->y << endl;
        if (p->type == 0) {
            p->x = token[0].x;
            p->y = token[0].y;
        }
        else if (p->type == 1) {
            if (token[0].y <= 0.225 * 8) {
                token[0].y += 0.225;
                p->y = token[0].y;
            }
        }
        else if (p->type == 2) {
            if (token[0].y > 0.25) {
                token[0].y -= 0.225;
                p->y = token[0].y;
            }
        }
        else if (p->type == 3) {
            if (token[0].x > 0.25) {
                token[0].x -= 0.225;
                p->x = token[0].x;
            }
        }
        else if (p->type == 4) {
            if (token[0].x < 1.75) {
                token[0].x += 0.225;
                p->x = token[0].x;
            }
        }
        DWORD sent_byte;
        WSABUF mybuf[1];
        mybuf[0].buf = reinterpret_cast<char*>(p); mybuf[0].len = sizeof(move_pawn);
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
        token[i].x = 0.22 + 0.225 * i;
        token[i].y = 0.45;
    }
    for (int i = 8; i < 10; ++i) {  // 9 ~ 10번 말 위치 정해주기
        token[i].x = 0.22 + 0.225 * i;

    }
}