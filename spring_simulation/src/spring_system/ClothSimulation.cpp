#include "ClothSimulation.h"

void ClothSimulation::InitPositions()
	{
		int raw = 0;
		for (int i = 0; i < length; i++) {
			std::vector<glm::vec3> rawFactor;
			int colum = 0;
			for (int j = 0; j < length; j++)
			{
				if (i == length - 1 && j == length - 1){
					glm::vec3 position(76, 76, 0);
					colum += initLenth;
					rawFactor.push_back(position);
				}
				else if(i == length - 1 && j == 0)
				{
					glm::vec3 position(21, 76, 0);
					colum += initLenth;
					rawFactor.push_back(position);
				}
				else
				{
					glm::vec3 position(20 + colum, 20.0f + raw, 0);
					colum += initLenth;
					rawFactor.push_back(position);
				}
				
			}
			ParticlePositions.push_back(rawFactor);
			raw += initLenth;
		}
	}
void ClothSimulation::InitEdges() {
	//for (size_t i = 0; i < length; i++) {
	//	for (int j = 0; j < length; j++) {
	//		//static_cast:��i����������ת��Ϊfloat
	//		ParticlePositions[i][j].x = static_cast<float>(i);
	//		ParticlePositions[i][j].y = static_cast<float>(i);
	//	}
	//}
	for (int i = 0; i < length; i++)
	{
		for (int j = 0; j < length; j++) 
		{
			if (j + 1 == length || i + 1 == length)
				continue;
			//����
			edges.push_back(ClothEdge{ ClothPoint {i, j},ClothPoint{i,j + 1} });
			edges.push_back(ClothEdge{ ClothPoint {j, i},ClothPoint{j+1, i} });
			edges.push_back(ClothEdge{ ClothPoint {i, j},ClothPoint{i + 1, j + 1} });
			//edges.push_back(ClothEdge{ ClothPoint {j + 1, i},ClothPoint{i, j + 1} });
		}
	}
	for (int i = 0; i < length - 1; i++) {
		//����߽�
		edges.push_back(ClothEdge{ ClothPoint {i, length - 1},ClothPoint{i + 1, length - 1} });
		edges.push_back(ClothEdge{ ClothPoint {length - 1, i},ClothPoint{length - 1, i + 1} });
	}
}
void ClothSimulation::InitForcesArray() {
	forces.resize(length);
	velocities.resize(length);
	for (int i = 0; i < length; i++) {
		forces[i].resize(length);
		velocities[i].resize(length);
	}
}
void ClothSimulation::CalculateForces() {
	//��������
	glm::vec3 gravity(0.0f, -0.98f, 0.0f);
	for (int i = 0; i < length; i++) {
		for (int j = 0; j < length; j++) {
			forces[i][j] = mass * gravity;
		}
	}
	for (int i = 0; i < length; i++) 
	{
		for (int j = 0; j < length; j++) 
		{
			//ÿ����Ҫ������Χ�˸�������ĵ�������
			for (int k = i - 1; k < i + 2; k++) 
			{
				for (int n = j - 1; n < j + 2; n++) 
				{
					//����㣬����
					if(k == i && n == j)
						continue;
					//�߽�㣬����
					if (k < 0 || n < 0 || k > length - 1 || n > length - 1)
						continue;
					//���㵯����
					glm::vec3 pos0 = (ParticlePositions[i][j]);
					glm::vec3 pos1 = (ParticlePositions[k][n]);
					glm::vec3 r = pos0 - pos1;
				    float distance = glm::distance(pos0, pos1);
					//��������Ǵ�ģ���Ϊб�ŵı�Ҳ����1.0f������ġ�
					glm::vec3 elasticForce(-stiffness * (distance - initLenth) * glm::normalize(r));
					forces[i][j] += elasticForce;
					//����������
					glm::vec3 velocity1 = (velocities[i][j]);
					glm::vec3 velocity2 = (velocities[k][n]);
					glm::vec3 v = velocity1 - velocity2;
					glm::vec3 dampForce(-dampCoefficient * v);
					forces[i][j] += dampForce;
				}
			}
		}
	}
}
void ClothSimulation::updateStates(float timeIntervalInSeconds) {
	for (int i = 0; i < length; i++)
	{
		for (int j = 0; j < length; j++)
		{
			glm::vec3 newAcceleration(forces[i][j] / mass);
			glm::vec3 newVelocity(velocities[i][j] +
				(timeIntervalInSeconds * newAcceleration));
			//�ٶȵ���һ���̶�����ֹͣ
			if (glm::length(newVelocity) <= 0.3f)
				continue;
			//���������̶��ĵ�
			if (i == length - 1 && j == length - 1)
				continue;
			if (i == length - 1 && j == 0)
				continue;
			glm::vec3 pos0 = (ParticlePositions[i][j]);
			
			glm::vec3 newPosition(ParticlePositions[i][j] +
				(timeIntervalInSeconds * newVelocity));
			
			velocities[i][j] = newVelocity;
			ParticlePositions[i][j] = newPosition;
			
		}
	}
}