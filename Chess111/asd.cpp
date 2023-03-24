#include <iostream>
#include <GL/glut.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

// 전역 변수들
int width[2];
int height[2];
int i = 0;
GLuint texID[2];

float white_pawn_x;
float white_pawn_y;

// 함수 선언
unsigned char* LoadMeshFromFile(const char* texFile);
void init();
void DrawChessBoard();
void DrawPawn();
void display();
void reshape(GLint w, GLint h);
void SpecialKeyboard(int key, int x, int y);
void check_soket(int value);

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("image");
    glutReshapeFunc(reshape);
    init();
    glutDisplayFunc(display);
    glutSpecialFunc(SpecialKeyboard);
    glutMainLoop();
    return 0;
}

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

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    free(pawn);

    // pawn 위치 설정
    white_pawn_x = 0.22 + 0.225 * 4;
    white_pawn_y = 0.45;
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
    glTexCoord2d(0.0, 1.0);      glVertex3d(white_pawn_x - 0.1f, white_pawn_y + 0.1f, 0.0);
    glTexCoord2d(0.0, 0.0);      glVertex3d(white_pawn_x - 0.1f, white_pawn_y - 0.1f, 0.0);
    glTexCoord2d(1.0, 0.0);      glVertex3d(white_pawn_x + 0.1f, white_pawn_y - 0.1f, 0.0);
    glTexCoord2d(1.0, 1.0);      glVertex3d(white_pawn_x + 0.1f, white_pawn_y + 0.1f, 0.0);
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
        if (white_pawn_y <= 0.225 * 8) {
            white_pawn_y += 0.225;
        }
    }
    if (key == GLUT_KEY_DOWN) {
        if (white_pawn_y > 0.25) {
            white_pawn_y -= 0.225;
        }
    }
    if (key == GLUT_KEY_LEFT) {
        if (white_pawn_x > 0.25) {
            white_pawn_x -= 0.225;
        }
    }
    if (key == GLUT_KEY_RIGHT) {
        if (white_pawn_x < 1.75) {
            white_pawn_x += 0.225;
        }
    }
    glutPostRedisplay();
}


void check_soket(int value) {


    glutTimerFunc(0.1, check_soket, 1);
}
