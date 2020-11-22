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

//球体半径
float sphereRadius = 4.0f;
//背景颜色
COLOR backgroundColor(0.75f, 0.75f, 0.85f, 1.0f);
//网格中每行列的质点数量
const int gridSize = 15;
//阻尼系数
float dampFactor = 1.8f;
//弹簧初始长度
float initLength = 1.0f;
//质点数量
int particlNumber;
//布料拖拽系数
float dragCoefficient = 0.15f;
//质点存储数组
Particle* particleArray = NULL;
Particle* particleArray2 = NULL;

Particle* currentParticle = NULL;
Particle* nextParticle = NULL;
//重力
glm::vec3 gravity(0.0f, -0.98f, 0.0f);
//风力
glm::vec3 wind(0.7f, 0.0f, 0.0f);
//空气密度
float airDensity = 1.2f;
//弹簧数量
int springNumber;
//存储弹簧的数组
Spring* springArray = NULL;
//Floor texture
GLuint floorTexture;
unsigned char* m_LocalBuffer;
int width = 640;
int height = 640;

int m_Wideth = 340;
int m_Height = 305;
int m_BPP = 0;
//初始化弹簧质点系统
void initCloth() {
	//计算质点的数量
	particlNumber = gridSize * gridSize;

	//计算弹簧数量
	springNumber = (gridSize - 1) * gridSize * 2;
	springNumber += (gridSize - 1) * (gridSize - 1) * 2;
	springNumber += (gridSize - 2) * gridSize * 2;

	//创建数组空间
	particleArray = new Particle[particlNumber];
	particleArray2 = new Particle[particlNumber];
	springArray = new Spring[springNumber];
	//初始化质点位置
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

	//创造从左到右的弹簧
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

	//创造从上到下的弹簧
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
            //斜着的弹簧长度是横竖弹簧的1.414倍
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
//使用glu库绘制球体。这种绘制一定要在while中进行  不然绘制不出来
void drawSphere() {
    //绘制中心点在原点的球体。
    static GLUquadricObj* sphere = gluNewQuadric();
    glEnable(GL_LIGHTING);
    /*glMaterialfv函数的使用
    指定用于光照计算的当前材质属性。参数face的取值可以是GL_FRONT、GL_BACK或GL_FRONT_AND_BACK，指出材质属性将应用于物体的哪面。
    最后的参数是一个指向数组的指针
    第一个参数        默认值           作用
    GL_AMBIENT（0.2，0.2，0.2，1.0）材质的环境颜色
    GL_DIFFUSE（0.8，0.8，0.8，1.0）材质的散射颜色
    GL_AMBIENT_AND_DIFFUSE 材质的环境颜色和散射颜色
    GL_SPECULAR（0.0，0.0，0.0，1.0）材质的镜面反射颜色
    GL_SHININESS 0.0 镜面反射指数  指数越大，高光部分越小
    GL_EMISSION（0.0，0.0，0.1，1.0）材质的发射光颜色
    GL_COLOR_INDEXES（0， 1， 1） 环境颜色索引、散射颜色索引和镜面反射颜色索引*/
    glMaterialfv(GL_FRONT, GL_AMBIENT, COLOR(1.0f, 0.0f, 0.0f, 0.0f));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, COLOR(1.0f, 0.0f, 0.0f, 0.0f));
    glMaterialfv(GL_FRONT, GL_SPECULAR, blue);
    glMaterialf(GL_FRONT, GL_SHININESS, 32.0f);
    //开启或禁用多边形正面或者背面上的光照、阴影和颜色计算及操作，消除不必要的渲染计算。
    glEnable(GL_CULL_FACE);
    //三四个参数指定围绕z轴的细分数（类似于经度线）。  不是很懂
    gluSphere(sphere, sphereRadius, 48, 24);

	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, black);
}
//使用三角形绘制布料
void drawCloth() {
	//设置光照的各个参数
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
				//绘制两种三角形（合并成一个正方形）
				
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
//绘制地板
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
//设置MVP矩阵
void setMVPMatrix() {
	//视口
	glViewport(0, 0, width, height);

	//设置投影矩阵
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//设置视口比例  使图像不随着窗口压缩而变化
	//控制视景体  但是还是会拉伸???
	gluPerspective(45.0f, width / height, 0.1f, 100.0f);

	//模型矩阵
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//设置深度值(用于创建阴影.)
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	//设置光源位置
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

	//计算风力
	//风力F = -1/2 * 空气密度 * 相对速度绝对值 * 布料拖拽系数 * 三角形面积 *（相对速度向量*法向量向量）*法向量向量。
			//并未计算阻尼力，而是用的摩擦力代替。

	for (int i = 0; i < gridSize - 1; i++) {
		for (int j = 0; j < gridSize - 1; j++) {
			//求出速度平均值
			glm::vec3 aveVelocity = 
			(currentParticle[i * gridSize + j].velocity +
			currentParticle[i * gridSize + j + 1].velocity +
			currentParticle[(i + 1) * gridSize + j].velocity) / 3.0f;
			//计算相对速度
			glm::vec3 relVelocity = aveVelocity - wind;
			//计算三角形面积
			float a = glm::length(currentParticle[i * gridSize + j].position - currentParticle[i * gridSize + j + 1].position);
			float b = glm::length(currentParticle[i * gridSize + j].position - currentParticle[(i + 1) * gridSize + j].position);
			float c = glm::length(currentParticle[i * gridSize + j + 1].position - currentParticle[(i + 1) * gridSize + j].position);
			float p = (a + b + c) * (a + b - c) * (a + c - b) * (b + c - a)/4;
			float s = sqrt(p);
			//求解风力方程。
			currentParticle[i].forces 
				= currentParticle[i * gridSize + j + 1].forces 
				= currentParticle[(i + 1) * gridSize + j].forces
				= -0.5f * airDensity * glm::abs(relVelocity) * dragCoefficient * s
				* (relVelocity * currentParticle[i].normal) * currentParticle[i].normal;
		}
	}
	//计算弹力
	for (int i = 0; i < springNumber; ++i)
	{
		//运动过程中弹簧的长度
		float springLength = glm::length(currentParticle[springArray[i].firstParticle].position -
			currentParticle[springArray[i].endParticle].position);
		//长度的变化量
		float extension = springLength - springArray[i].initLength;
		//胡克定律 F = k * （L - L0）   不过后边为什么除以初始长度？
		//将计算的弹力赋值到弹簧上
		springArray[i].elasticForce = springArray[i].stiffness * extension / springArray[i].initLength;
	}
	//将当前的质点信息赋值给下一时刻的质点（挺绕的，看代码理解。）
	for (int i = 0; i < particlNumber; ++i)
	{
		//若果质点固定，则不更新位置，速度设置为0  
		//不固定，则计算速度和位置
		if (currentParticle[i].fixed)
		{
			nextParticle[i].position = currentParticle[i].position;
			nextParticle[i].velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		else
		{
			//计算质点受到的所有力。
			glm::vec3 force = gravity + particleArray[i].forces;
			/*遍历所有弹簧，如果当前遍历的弹簧的第一个端点是当前for循环的质点，则通过
			当前质点的下标算出相邻质点的下标，并求出质点之间的力的作用。
			至于为什么要算如下两次，请搞清楚创建弹簧时的具体信息
			*/
			for (int j = 0; j < springNumber; ++j)
			{
				//当弹簧的起点是当前点
				if (springArray[j].firstParticle == i)
				{
					//弹力方向是从当前点指向下一个点，并且对向量进行单位化处理
					glm::vec3 tensionDirection = glm::normalize(currentParticle[springArray[j].endParticle].position -
						currentParticle[i].position);
					//质点所受到的力    加上弹力
					force += springArray[j].elasticForce * tensionDirection;
					//计算阻尼力  f = 阻尼系数 * 速度差
					glm::vec3 FVD = currentParticle[springArray[j].endParticle].velocity -
						currentParticle[i].velocity;
					force += dragCoefficient * FVD;
				}
				//当弹簧的终点是当前点。操作和上一个类似
				if (springArray[j].endParticle == i)
				{
					glm::vec3 tensionDirection = glm::normalize(currentParticle[springArray[j].firstParticle].position -
						currentParticle[i].position);
					force += springArray[j].elasticForce * tensionDirection;
					//计算阻尼力  f = 阻尼系数 * 速度差
					glm::vec3 FVD = currentParticle[springArray[j].endParticle].velocity -
						currentParticle[i].velocity;
					force += dragCoefficient * FVD;
				}
			}

			
			//计算加速度
			glm::vec3 acceleration = force / currentParticle[i].mass;
			//更新质点速度
			nextParticle[i].velocity = currentParticle[i].velocity + acceleration * (float)deltaTime;

			//计算新的位置
			nextParticle[i].position = currentParticle[i].position +
				(nextParticle[i].velocity + currentParticle[i].velocity) * (float)deltaTime / 2.0f;

			/* 球体的碰撞检测
			球体的中心店在坐标圆点，所以只需要检测质点到坐标原点的距离是否小于圆的半径
			为了避免穿模，所以半径乘以一定值。
			*/
			float squareLength = nextParticle[i].position.x * nextParticle[i].position.x
				+ nextParticle[i].position.y * nextParticle[i].position.y
				+ nextParticle[i].position.z * nextParticle[i].position.z;
			if (squareLength < sphereRadius * 1.08f * sphereRadius * 1.08f)
				nextParticle[i].position = glm::normalize(nextParticle[i].position)* sphereRadius * 1.08f;

			//检测质点是否和地板发生碰撞。
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
	//初始化弹簧质点模型
	initCloth();
	//设置MVP投影矩阵
	setMVPMatrix();
    
    while (!glfwWindowShouldClose(window))
    {
		//Clear buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();										//reset modelview matrix
		//平移函数
		glTranslatef(0.0f, 0.0f, -28.0f);
        //这里假设每帧的时间都是0.01处理的。
		UpdateFrame(0.01f);
		//绘制地板
		//drawFloor();
        //绘制球体
        drawSphere();
		//三角形绘制布料
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