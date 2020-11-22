#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <vector>
#include "MassSpringSystem.h"

MassSpringSystem::MassSpringSystem(size_t numberOfPoints)
	:numberOfPoints(10)
{
	size_t numberOfEdges = numberOfPoints - 1;
	//初始化各个数组的长度
	positions.resize(numberOfPoints);
	velocities.resize(numberOfPoints);
	forces.resize(numberOfPoints);
	edges.resize(numberOfEdges);

	for (size_t i = 0; i < numberOfPoints; i++) {
		//static_cast:将i的数据类型转化为float
		positions[i].x = static_cast<float>(i);
		positions[i].y = static_cast<float>(i);
	}
	//初始化每条边
	for (size_t i = 0; i < numberOfEdges; i++)
	{
		edges[i] = Edge{ i,i + 1 };
	}
};
void MassSpringSystem::calculateForce() {
	size_t numberOfPoints = positions.size();
	size_t numberOfEdges = edges.size();
	glm::vec3 gravity(0.0f, -9.8f, 0.0f);
	for (size_t i = 0; i < numberOfPoints; i++)
	{
		//重力
		forces[i] = mass * gravity;
	}
	for (size_t i = 0; i < numberOfEdges; i++) {
		size_t pointIndex0 = edges[i].first_point;
		size_t pointIndex1 = edges[i].end_point;
		//定义一个pos1是这样的格式吗？网上搜不到
		glm::vec3 pos0(positions[pointIndex0]);
		glm::vec3 pos1(positions[pointIndex1]);
		glm::vec3 r(pos0 - pos1);
		float distance = glm::distance(pos0,pos1);
		if (distance > 0) {
			glm::vec3 elasticForce
			(-stiffness * (distance - restLength) * glm::normalize(r));
			forces[pointIndex0] += elasticForce;
			forces[pointIndex1] -= elasticForce;
		}
	}
}
void MassSpringSystem::updateStates(float timeIntervalInSeconds) {
	for (size_t i = 0; i < numberOfPoints; i++)
	{
		if (i == 0)
			continue;
		
		glm::vec3 newAcceleration(forces[i] / mass);
		glm::vec3 newVelocity(velocities[i] + 
			(timeIntervalInSeconds * newAcceleration));
		glm::vec3 newPosition(positions[i] +
			(timeIntervalInSeconds * newVelocity));
		velocities[i] = newVelocity;
		positions[i] = newPosition;
	}
}