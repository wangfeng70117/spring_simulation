#pragma once
struct Edge
{
	size_t first_point;
	size_t end_point;
};
class MassSpringSystem 
{
private:
	//�ٶ���ÿ���ʵ������
	float mass = 1.0f;
	//���ɲ�������ʱ�ĳ���
	float restLength = 1.0f;
	//���˶���ϵ��
	float stiffness = 10.0f;

	size_t numberOfPoints;
public:
	//����С���λ����Ϣ
	std::vector<glm::vec3> positions;
	//����С����ٶ���Ϣ
	std::vector<glm::vec3> velocities;
	//����С���������Ϣ�����ܵ���ȫ������
	std::vector<glm::vec3> forces;
	//�����ߵ���Ϣ
	std::vector<Edge> edges;
	MassSpringSystem(size_t numberOfPoints = 10);
	void calculateForce();
	void updateStates(float timeIntervalInSeconds);
};