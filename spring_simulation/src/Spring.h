#pragma once

class Spring {
public:
	//�������ӵĵ�һ���͵ڶ����ʵ��±�
	int firstParticle;
	int endParticle;
	//�˶�ʱ�ĵ���
	float elasticForce;
	//���˶���ϵ��
	float stiffness = 15.0f;
	//��ʼ����
	float initLength;
};