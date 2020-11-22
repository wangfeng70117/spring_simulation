#pragma once
#include "glm/glm.hpp"
#ifndef ParticleH
#define ParticleH

//Make sure these structures are packed to a multiple of 4 bytes (sizeof(float)),
//since glMap2 takes strides as integer multiples of 4 bytes.
//How consistent...
#pragma pack(push)
#pragma pack(4)
class Particle {
public:
	//����
	float mass = 0.01f;
	//�Ƿ�̶�
	bool fixed = false;
	//λ��
	glm::vec3 position;
	//�ٶ�
	glm::vec3 velocity;
	//������
	glm::vec3 normal;
	//�ܵ�����
	glm::vec3 forces;
};
#pragma pack(pop)

#endif