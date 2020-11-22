#pragma once

class Spring {
public:
	//弹簧连接的第一个和第二个质点下标
	int firstParticle;
	int endParticle;
	//运动时的弹力
	float elasticForce;
	//胡克定律系数
	float stiffness = 15.0f;
	//初始长度
	float initLength;
};