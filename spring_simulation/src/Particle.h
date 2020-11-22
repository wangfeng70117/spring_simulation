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
	//质量
	float mass = 0.01f;
	//是否固定
	bool fixed = false;
	//位置
	glm::vec3 position;
	//速度
	glm::vec3 velocity;
	//法向量
	glm::vec3 normal;
	//受到的力
	glm::vec3 forces;
};
#pragma pack(pop)

#endif