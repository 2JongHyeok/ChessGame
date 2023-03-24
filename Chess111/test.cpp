#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <GL/glut.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

#pragma comment (lib, "WS2_32.LIB")

const char* SERVER_ADDR; // = "127.0.0.1"; // 서버 주소 ( 127.0.0.1 ) 은 내 컴퓨터의 주소임.
const short SERVER_PORT = 4000; // 포트는 가능하면 1000번대 이상으로. 1~30 ? 정도는 이미 있는 포트라 사용하지 말것.
SOCKET s_socket;	// 사용할 소켓의 이름 지정.
void check_soket(int value);	// 소켓을 체크해 주는 함수임.


struct move_pawn {
	int type = 0;       // 1.up 2.down 3.left 4.right
	float x, y;		// 현재 위치를 가지고 있고, 보내줄 것임.
};

move_pawn p;	// 캐릭터 위치들을 잡아줄 p라는 변수 생성

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



// image width and height
int width[2];
int height[2];
int i = 0;
GLuint texID[2];

//이미지 파일 열기
unsigned char* LoadMeshFromFile(const char* texFile)
{
	GLuint texture;
	glGenTextures(1, &texture);
	FILE* fp = NULL;
	if (fopen_s(&fp, texFile, "rb")) {
		printf("ERROR : No %s. \n fail to bind %d\n", texFile, texture);
		return (unsigned char*)false;
	}
	int channel;
	unsigned char* image = stbi_load_from_file(fp, &width[i], &height[i], &channel, 4);
	i = (i + 1) % 2;
	fclose(fp);
	return image;
}

// texture 설정
void init()
{
	stbi_set_flip_vertically_on_load(true);

	glEnable(GL_BLEND);            // 블렌딩 기능을 활성화한다.
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    //지수를 원본 컬러 벡터 Csource의 알파값으로 설정.


	unsigned char* bitmap;
	bitmap = LoadMeshFromFile((char*)"ChessBoard.png");
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texID[0]);
	glBindTexture(GL_TEXTURE_2D, texID[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width[0], height[0], 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
	free(bitmap);

	unsigned char* pawn;
	pawn = LoadMeshFromFile((char*)"WhitePawn.png");
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texID[1]);
	glBindTexture(GL_TEXTURE_2D, texID[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width[1], height[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, pawn);

	free(pawn);

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
	connect(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));

	WSABUF send_buf[1];
	send_buf[0].buf = reinterpret_cast<char*>(&p);
	send_buf[0].len = sizeof(move_pawn);
	DWORD sent_byte;
	int ret = WSASend(s_socket, send_buf, 1, &sent_byte, 0, 0, 0);

	move_pawn recv_move;
	WSABUF recv_buf[1];
	recv_buf[0].buf = reinterpret_cast<char*>(&recv_move);
	recv_buf[0].len = sizeof(move_pawn);
	DWORD recv_byte;
	DWORD recv_flag = 0;
	ret = WSARecv(s_socket, recv_buf, 1, &recv_byte, &recv_flag, 0, 0);

	p.x = recv_move.x;
	p.y = recv_move.y;
}

void DrawChessBoard()
{
	glBegin(GL_POLYGON);
	glTexCoord2d(0.0, 1.0);      glVertex3d(0, 2.0, 0.0);
	glTexCoord2d(0.0, 0.0);      glVertex3d(0.0, 0.0, 0.0);
	glTexCoord2d(1.0, 0.0);      glVertex3d(2.0, 0.0, 0.0);
	glTexCoord2d(1.0, 1.0);      glVertex3d(2.0, 2.0, 0.0);
	glEnd();
}

void DrawPawn()
{
	glBegin(GL_POLYGON);
	glTexCoord2d(0.0, 1.0);      glVertex3d(p.x - 0.1f, p.y + 0.1f, 0.0);
	glTexCoord2d(0.0, 0.0);      glVertex3d(p.x - 0.1f, p.y - 0.1f, 0.0);
	glTexCoord2d(1.0, 0.0);      glVertex3d(p.x + 0.1f, p.y - 0.1f, 0.0);
	glTexCoord2d(1.0, 1.0);      glVertex3d(p.x + 0.1f, p.y + 0.1f, 0.0);
	glEnd();
}

void display()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-1.0, -1.0, 0.0);

	glBindTexture(GL_TEXTURE_2D, texID[0]);
	DrawChessBoard();
	glBindTexture(GL_TEXTURE_2D, texID[1]);
	for (int i = 0; i < 8; ++i) {
		DrawPawn();
	}
	glutSwapBuffers();
}

void reshape(GLint w, GLint h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 30.0);
}

void SpecialKeyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_UP) {
		p.type = 1;
	}
	if (key == GLUT_KEY_DOWN) {
		p.type = 2;
	}
	if (key == GLUT_KEY_LEFT) {
		p.type = 3;

	}
	if (key == GLUT_KEY_RIGHT) {
		p.type = 4;
	}
	WSABUF send_buf[1];
	send_buf[0].buf = reinterpret_cast<char*>(&p);
	send_buf[0].len = sizeof(move_pawn);
	DWORD sent_byte;
	int ret = WSASend(s_socket, send_buf, 1, &sent_byte, 0, 0, 0);

	move_pawn recv_move;
	WSABUF recv_buf[1];
	recv_buf[0].buf = reinterpret_cast<char*>(&recv_move);
	recv_buf[0].len = sizeof(move_pawn);
	DWORD recv_byte;
	DWORD recv_flag = 0;
	ret = WSARecv(s_socket, recv_buf, 1, &recv_byte, &recv_flag, 0, 0);

	p.x = recv_move.x;
	p.y = recv_move.y;

	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	char server_add[15];
	cin >> server_add;
	SERVER_ADDR = server_add;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("image");
	glutReshapeFunc(reshape);
	init();
	glutDisplayFunc(display);
	glutSpecialFunc(SpecialKeyboard);
	//glutTimerFunc(0.1,check_soket,1);
	glutMainLoop();
	WSACleanup();
	return 0;
}

void check_soket(int value) {


	glutTimerFunc(0.1, check_soket, 1);
}