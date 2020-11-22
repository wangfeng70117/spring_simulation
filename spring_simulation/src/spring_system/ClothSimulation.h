#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
struct ClothPoint
{
	int position_index_1;
	int position_index_2;
};
struct ClothEdge
{
	ClothPoint first_point;
	ClothPoint end_point;
};

class ClothSimulation {
private:
	std::vector<std::vector<glm::vec3>> forces;
	float mass = 0.01f;
	float initLenth = 1.0f;
	float stiffness = 15.0f;
	float dampCoefficient = 0.9f;
	std::vector<std::vector<glm::vec3>> velocities;
public:
	int length = 15;
	std::vector<std::vector<glm::vec3>> ParticlePositions;
	std::vector<ClothEdge> edges;
	void InitPositions();
	void InitEdges();

	void CalculateForces();
	void InitForcesArray();
	void updateStates(float timeIntervalInSeconds);
};