#pragma once
struct Edge
{
	size_t first_point;
	size_t end_point;
};
class MassSpringSystem 
{
private:
	//假定的每个质点的质量
	float mass = 1.0f;
	//弹簧不被拉伸时的长度
	float restLength = 1.0f;
	//胡克定律系数
	float stiffness = 10.0f;

	size_t numberOfPoints;
public:
	//各个小球的位置信息
	std::vector<glm::vec3> positions;
	//各个小球的速度信息
	std::vector<glm::vec3> velocities;
	//各个小球的受力信息（所受到的全部力）
	std::vector<glm::vec3> forces;
	//各个边的信息
	std::vector<Edge> edges;
	MassSpringSystem(size_t numberOfPoints = 10);
	void calculateForce();
	void updateStates(float timeIntervalInSeconds);
};