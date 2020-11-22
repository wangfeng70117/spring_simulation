#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "ClothSimulation.h"
#include "COLOR.h"
#include "Particle.h"
#include "Spring.h"
#include "IMAGE.h"
#include "stb_image/stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

//����뾶
float sphereRadius = 4.0f;
//������ɫ
COLOR backgroundColor(0.75f, 0.75f, 0.85f, 1.0f);
//������ÿ���е��ʵ�����
const int gridSize = 15;
//����ϵ��
float dampFactor = 1.8f;
//���ɳ�ʼ����
float initLength = 1.0f;
//�ʵ�����
int particlNumber;
//������קϵ��
float dragCoefficient = 0.15f;
//�ʵ�洢����
Particle* particleArray = NULL;
Particle* particleArray2 = NULL;

Particle* currentParticle = NULL;
Particle* nextParticle = NULL;
//����
glm::vec3 gravity(0.0f, -0.98f, 0.0f);
//����
glm::vec3 wind(0.7f, 0.0f, 0.0f);
//�����ܶ�
float airDensity = 1.2f;
//��������
int springNumber;
//�洢���ɵ�����
Spring* springArray = NULL;
//Floor texture
GLuint floorTexture;
unsigned char* m_LocalBuffer;
int width = 640;
int height = 640;

int m_Wideth = 340;
int m_Height = 305;
int m_BPP = 0;
//��ʼ�������ʵ�ϵͳ
void initCloth() {
	//�����ʵ������
	particlNumber = gridSize * gridSize;

	//���㵯������
	springNumber = (gridSize - 1) * gridSize * 2;
	springNumber += (gridSize - 1) * (gridSize - 1) * 2;
	springNumber += (gridSize - 2) * gridSize * 2;

	//��������ռ�
	particleArray = new Particle[particlNumber];
	particleArray2 = new Particle[particlNumber];
	springArray = new Spring[springNumber];
	//��ʼ���ʵ�λ��
	for (int i = 0; i < gridSize; ++i)
	{
		for (int j = 0; j < gridSize; ++j)
		{
			
			particleArray[i * gridSize + j].position = glm::vec3
				(2.0f,
				float(j) - float(gridSize - 1) / 2,
				float(i) - float(gridSize - 1) / 2);
			particleArray[i * gridSize + j].velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			particleArray[i * gridSize + j].forces = glm::vec3(0.0f, 0.0f, 0.0f);
			particleArray[i * gridSize + j].normal = glm::vec3(0.0f, 0.0f, 0.0f);
		}
	}
	particleArray[gridSize - 1].fixed = true;
	particleArray[gridSize * gridSize - 1].fixed = true;
	//Copy the balls into the other array
	for (int i = 0; i < particlNumber; ++i)
		particleArray2[i] = particleArray[i];
	//Set the currentParticle and nextParticle pointers
	currentParticle = particleArray;
	nextParticle = particleArray2;


	//Initialise the springArray
	Spring* currentSpring = &springArray[0];

	//��������ҵĵ���
	for (int i = 0; i < gridSize; ++i)
	{
		for (int j = 0; j < gridSize - 1; ++j)
		{
			currentSpring->firstParticle = i * gridSize + j;
			currentSpring->endParticle = i * gridSize + j + 1;
			currentSpring->initLength = initLength;
			++currentSpring;
		}
	}

	//������ϵ��µĵ���
	for (int i = 0; i < gridSize - 1; ++i)
	{
		for (int j = 0; j < gridSize; ++j)
		{
			currentSpring->firstParticle = i * gridSize + j;
			currentSpring->endParticle = (i + 1) * gridSize + j;
			currentSpring->initLength = initLength;
			++currentSpring;
		}
	}

	//The next (gridSize-1)*(gridSize-1) go from a ball to the one below and right
	//excluding those on the bottom or right
	for (int i = 0; i < gridSize - 1; ++i)
	{
		for (int j = 0; j < gridSize - 1; ++j)
		{
			currentSpring->firstParticle = i * gridSize + j;
			currentSpring->endParticle = (i + 1) * gridSize + j + 1;
            //б�ŵĵ��ɳ����Ǻ������ɵ�1.414��
			currentSpring->initLength = initLength * sqrt(2.0f);
			++currentSpring;
		}
	}

	//The next (gridSize-1)*(gridSize-1) go from a ball to the one below and left
	//excluding those on the bottom or right
	for (int i = 0; i < gridSize - 1; ++i)
	{
		for (int j = 1; j < gridSize; ++j)
		{
			currentSpring->firstParticle = i * gridSize + j;
			currentSpring->endParticle = (i + 1) * gridSize + j - 1;
			currentSpring->initLength = initLength * sqrt(2.0f);

			++currentSpring;
		}
	}

	//The first (gridSize-2)*gridSize springArray go from one ball to the next but one,
	//excluding those on or next to the right hand edge
	for (int i = 0; i < gridSize; ++i)
	{
		for (int j = 0; j < gridSize - 2; ++j)
		{
			currentSpring->firstParticle = i * gridSize + j;
			currentSpring->endParticle = i * gridSize + j + 2;
			currentSpring->initLength = initLength * 2;

			++currentSpring;
		}
	}

	//The next (gridSize-2)*gridSize springArray go from one ball to the next but one below,
	//excluding those on or next to the bottom edge
	for (int i = 0; i < gridSize - 2; ++i)
	{
		for (int j = 0; j < gridSize; ++j)
		{
			currentSpring->firstParticle = i * gridSize + j;
			currentSpring->endParticle = (i + 2) * gridSize + j;
			currentSpring->initLength = initLength * 2;

			++currentSpring;
		}
	}
}
//ʹ��glu��������塣���ֻ���һ��Ҫ��while�н���  ��Ȼ���Ʋ�����
void drawSphere() {
    //�������ĵ���ԭ������塣
    static GLUquadricObj* sphere = gluNewQuadric();
    glEnable(GL_LIGHTING);
    /*glMaterialfv������ʹ��
    ָ�����ڹ��ռ���ĵ�ǰ�������ԡ�����face��ȡֵ������GL_FRONT��GL_BACK��GL_FRONT_AND_BACK��ָ���������Խ�Ӧ������������档
    ���Ĳ�����һ��ָ�������ָ��
    ��һ������        Ĭ��ֵ           ����
    GL_AMBIENT��0.2��0.2��0.2��1.0�����ʵĻ�����ɫ
    GL_DIFFUSE��0.8��0.8��0.8��1.0�����ʵ�ɢ����ɫ
    GL_AMBIENT_AND_DIFFUSE ���ʵĻ�����ɫ��ɢ����ɫ
    GL_SPECULAR��0.0��0.0��0.0��1.0�����ʵľ��淴����ɫ
    GL_SHININESS 0.0 ���淴��ָ��  ָ��Խ�󣬸߹ⲿ��ԽС
    GL_EMISSION��0.0��0.0��0.1��1.0�����ʵķ������ɫ
    GL_COLOR_INDEXES��0�� 1�� 1�� ������ɫ������ɢ����ɫ�����;��淴����ɫ����*/
    glMaterialfv(GL_FRONT, GL_AMBIENT, COLOR(1.0f, 0.0f, 0.0f, 0.0f));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, COLOR(1.0f, 0.0f, 0.0f, 0.0f));
    glMaterialfv(GL_FRONT, GL_SPECULAR, blue);
    glMaterialf(GL_FRONT, GL_SHININESS, 32.0f);
    //��������ö����������߱����ϵĹ��ա���Ӱ����ɫ���㼰��������������Ҫ����Ⱦ���㡣
    glEnable(GL_CULL_FACE);
    //���ĸ�����ָ��Χ��z���ϸ�����������ھ����ߣ���  ���Ǻܶ�
    gluSphere(sphere, sphereRadius, 48, 24);

	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, black);
}
//ʹ�������λ��Ʋ���
void drawCloth() {
	//���ù��յĸ�������
	glEnable(GL_LIGHTING);
	glMaterialfv(GL_FRONT, GL_AMBIENT, COLOR(0.8f, 0.0f, 1.0f));
	glMaterialfv(GL_FRONT, GL_DIFFUSE, COLOR(0.8f, 0.0f, 1.0f));
	glMaterialfv(GL_BACK, GL_AMBIENT, COLOR(1.0f, 1.0f, 0.0f));
	glMaterialfv(GL_BACK, GL_DIFFUSE, COLOR(1.0f, 1.0f, 0.0f));
	glBegin(GL_TRIANGLES);
	{
		for (int i = 0; i < gridSize - 1; ++i)
		{
			for (int j = 0; j < gridSize - 1; ++j)
			{
				//�������������Σ��ϲ���һ�������Σ�
				
				glNormal3fv(glm::value_ptr(currentParticle[i * gridSize + j].normal));
				glVertex3fv(glm::value_ptr(currentParticle[i * gridSize + j].position));
				glNormal3fv(glm::value_ptr(currentParticle[i * gridSize + j + 1].normal));
				glVertex3fv(glm::value_ptr(currentParticle[i * gridSize + j + 1].position));
				glNormal3fv(glm::value_ptr(currentParticle[(i + 1) * gridSize + j].normal));
				glVertex3fv(glm::value_ptr(currentParticle[(i + 1) * gridSize + j].position));

				glNormal3fv(glm::value_ptr(currentParticle[(i + 1) * gridSize + j].normal));
				glVertex3fv(glm::value_ptr(currentParticle[(i + 1) * gridSize + j].position));
				glNormal3fv(glm::value_ptr(currentParticle[i * gridSize + j + 1].normal));
				glVertex3fv(glm::value_ptr(currentParticle[i * gridSize + j + 1].position));
				glNormal3fv(glm::value_ptr(currentParticle[(i + 1) * gridSize + j + 1].normal));
				glVertex3fv(glm::value_ptr(currentParticle[(i + 1) * gridSize + j + 1].position));
			}
		}
	}
	glEnd();
	glDisable(GL_LIGHTING);
}
//���Ƶذ�
void drawFloor() {
	//Draw floor
	glEnable(GL_TEXTURE_2D);
	m_LocalBuffer = stbi_load("res/textures/floor.png", &m_Wideth, &m_Height, 0, 4);
	glGenTextures(1, &floorTexture);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Wideth, m_Height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
	
	glActiveTexture(GL_TEXTURE0);
	glBegin(GL_TRIANGLE_STRIP);
	{
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-25.0f, -8.6f, 5.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(25.0f, -8.6f, 5.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-25.0f, -8.6f, -45.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(25.0f, -8.6f, -45.0f);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
//����MVP����
void setMVPMatrix() {
	//�ӿ�
	glViewport(0, 0, width, height);

	//����ͶӰ����
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//�����ӿڱ���  ʹͼ�����Ŵ���ѹ�����仯
	//�����Ӿ���  ���ǻ��ǻ�����???
	gluPerspective(45.0f, width / height, 0.1f, 100.0f);

	//ģ�;���
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//�������ֵ(���ڴ�����Ӱ.)
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	//���ù�Դλ��
	GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT1, GL_AMBIENT, white * 0.2f);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white);
	glEnable(GL_LIGHT1);

	//Use 2-sided lighting
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, true);
}

void UpdateFrame(float deltaTime) {

	//Calculate the normals on the current balls
	for (int i = 0; i < gridSize - 1; ++i)
	{
		for (int j = 0; j < gridSize - 1; ++j)
		{
			glm::vec3 p0 = currentParticle[i * gridSize + j].position;
			glm::vec3 p1 = currentParticle[i * gridSize + j + 1].position;
			glm::vec3 p2 = currentParticle[(i + 1) * gridSize + j].position;
			glm::vec3 p3 = currentParticle[(i + 1) * gridSize + j + 1].position;

			glm::vec3 n0 = currentParticle[i * gridSize + j].normal;
			glm::vec3 n1 = currentParticle[i * gridSize + j + 1].normal;
			glm::vec3 n2 = currentParticle[(i + 1) * gridSize + j].normal;
			glm::vec3 n3 = currentParticle[(i + 1) * gridSize + j + 1].normal;

			//Calculate the normals for the 2 triangles and add on
			glm::vec3 normal = glm::cross((p1 - p0), (p2 - p0));

			currentParticle[i * gridSize + j].normal += normal;
			currentParticle[i * gridSize + j + 1].normal += normal;
			currentParticle[(i + 1) * gridSize + j].normal += normal;

			normal = glm::cross((p1 - p2), (p3 - p2));

			currentParticle[i * gridSize + j + 1].normal += normal;
			currentParticle[(i + 1) * gridSize + j].normal += normal;
			currentParticle[(i + 1) * gridSize + j + 1].normal += normal;
		}
	}
	//Normalize normals
	for (int i = 0; i < particlNumber; ++i)
		glm::normalize(currentParticle[i].normal);

	//�������
	//����F = -1/2 * �����ܶ� * ����ٶȾ���ֵ * ������קϵ�� * ��������� *������ٶ�����*������������*������������
			//��δ�����������������õ�Ħ�������档

	for (int i = 0; i < gridSize - 1; i++) {
		for (int j = 0; j < gridSize - 1; j++) {
			//����ٶ�ƽ��ֵ
			glm::vec3 aveVelocity = 
			(currentParticle[i * gridSize + j].velocity +
			currentParticle[i * gridSize + j + 1].velocity +
			currentParticle[(i + 1) * gridSize + j].velocity) / 3.0f;
			//��������ٶ�
			glm::vec3 relVelocity = aveVelocity - wind;
			//�������������
			float a = glm::length(currentParticle[i * gridSize + j].position - currentParticle[i * gridSize + j + 1].position);
			float b = glm::length(currentParticle[i * gridSize + j].position - currentParticle[(i + 1) * gridSize + j].position);
			float c = glm::length(currentParticle[i * gridSize + j + 1].position - currentParticle[(i + 1) * gridSize + j].position);
			float p = (a + b + c) * (a + b - c) * (a + c - b) * (b + c - a)/4;
			float s = sqrt(p);
			//���������̡�
			currentParticle[i].forces 
				= currentParticle[i * gridSize + j + 1].forces 
				= currentParticle[(i + 1) * gridSize + j].forces
				= -0.5f * airDensity * glm::abs(relVelocity) * dragCoefficient * s
				* (relVelocity * currentParticle[i].normal) * currentParticle[i].normal;
		}
	}
	//���㵯��
	for (int i = 0; i < springNumber; ++i)
	{
		//�˶������е��ɵĳ���
		float springLength = glm::length(currentParticle[springArray[i].firstParticle].position -
			currentParticle[springArray[i].endParticle].position);
		//���ȵı仯��
		float extension = springLength - springArray[i].initLength;
		//���˶��� F = k * ��L - L0��   �������Ϊʲô���Գ�ʼ���ȣ�
		//������ĵ�����ֵ��������
		springArray[i].elasticForce = springArray[i].stiffness * extension / springArray[i].initLength;
	}
	//����ǰ���ʵ���Ϣ��ֵ����һʱ�̵��ʵ㣨ͦ�Ƶģ���������⡣��
	for (int i = 0; i < particlNumber; ++i)
	{
		//�����ʵ�̶����򲻸���λ�ã��ٶ�����Ϊ0  
		//���̶���������ٶȺ�λ��
		if (currentParticle[i].fixed)
		{
			nextParticle[i].position = currentParticle[i].position;
			nextParticle[i].velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		else
		{
			//�����ʵ��ܵ�����������
			glm::vec3 force = gravity + particleArray[i].forces;
			/*�������е��ɣ������ǰ�����ĵ��ɵĵ�һ���˵��ǵ�ǰforѭ�����ʵ㣬��ͨ��
			��ǰ�ʵ���±���������ʵ���±꣬������ʵ�֮����������á�
			����ΪʲôҪ���������Σ���������������ʱ�ľ�����Ϣ
			*/
			for (int j = 0; j < springNumber; ++j)
			{
				//�����ɵ�����ǵ�ǰ��
				if (springArray[j].firstParticle == i)
				{
					//���������Ǵӵ�ǰ��ָ����һ���㣬���Ҷ��������е�λ������
					glm::vec3 tensionDirection = glm::normalize(currentParticle[springArray[j].endParticle].position -
						currentParticle[i].position);
					//�ʵ����ܵ�����    ���ϵ���
					force += springArray[j].elasticForce * tensionDirection;
					//����������  f = ����ϵ�� * �ٶȲ�
					glm::vec3 FVD = currentParticle[springArray[j].endParticle].velocity -
						currentParticle[i].velocity;
					force += dragCoefficient * FVD;
				}
				//�����ɵ��յ��ǵ�ǰ�㡣��������һ������
				if (springArray[j].endParticle == i)
				{
					glm::vec3 tensionDirection = glm::normalize(currentParticle[springArray[j].firstParticle].position -
						currentParticle[i].position);
					force += springArray[j].elasticForce * tensionDirection;
					//����������  f = ����ϵ�� * �ٶȲ�
					glm::vec3 FVD = currentParticle[springArray[j].endParticle].velocity -
						currentParticle[i].velocity;
					force += dragCoefficient * FVD;
				}
			}

			
			//������ٶ�
			glm::vec3 acceleration = force / currentParticle[i].mass;
			//�����ʵ��ٶ�
			nextParticle[i].velocity = currentParticle[i].velocity + acceleration * (float)deltaTime;

			//�����µ�λ��
			nextParticle[i].position = currentParticle[i].position +
				(nextParticle[i].velocity + currentParticle[i].velocity) * (float)deltaTime / 2.0f;

			/* �������ײ���
			��������ĵ�������Բ�㣬����ֻ��Ҫ����ʵ㵽����ԭ��ľ����Ƿ�С��Բ�İ뾶
			Ϊ�˱��⴩ģ�����԰뾶����һ��ֵ��
			*/
			float squareLength = nextParticle[i].position.x * nextParticle[i].position.x
				+ nextParticle[i].position.y * nextParticle[i].position.y
				+ nextParticle[i].position.z * nextParticle[i].position.z;
			if (squareLength < sphereRadius * 1.08f * sphereRadius * 1.08f)
				nextParticle[i].position = glm::normalize(nextParticle[i].position)* sphereRadius * 1.08f;

			//����ʵ��Ƿ�͵ذ巢����ײ��
			if (nextParticle[i].position.y < -8.5f)
				nextParticle[i].position.y = -8.5f;
		}
	}
	currentParticle = nextParticle;
	
}
int main(void)
{
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    /* Create a windowed mode window and its OpenGL context */

    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }


    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(5);


    if (!glewInit() == GLEW_OK)
        std::cout << "ERROR!" << std::endl;
	//��ʼ�������ʵ�ģ��
	initCloth();
	//����MVPͶӰ����
	setMVPMatrix();
    
    while (!glfwWindowShouldClose(window))
    {
		//Clear buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();										//reset modelview matrix
		//ƽ�ƺ���
		glTranslatef(0.0f, 0.0f, -28.0f);
        //�������ÿ֡��ʱ�䶼��0.01����ġ�
		UpdateFrame(0.01f);
		//���Ƶذ�
		//drawFloor();
        //��������
        drawSphere();
		//�����λ��Ʋ���
		drawCloth();
        /* Render here */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    //delete here
    glfwTerminate();
    return 0;
}