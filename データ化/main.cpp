#include<iostream>

namespace
{
	//�f�[�^��
	const struct kData
	{
		//�f�[�^�̖��O
		const char* name;
		//�̗�
		int hp;
	};

	//�f�[�^�̎��
	enum kObjectKind
	{
		Player,		//�v���C���[
		Enemy,		//�G�l�~�[
		Shot,		//�V���b�g
		Field,		//�t�B�[���h

		KindNum,	//�f�[�^�̐�
	};

	//�f�[�^�i�[
	const kData data[KindNum] = 
	{
		//�v���C���[
		{ "Player�f�[�^", 100 },
		//�G�l�~�[
		{ "Enemy�f�[�^", 50 },
		//�V���b�g
		{ "Shot�f�[�^", 10 },
		//�t�B�[���h
		{ "Field�f�[�^", 1000 },
	};
}

//�f�[�^�֐�
kData GetData(kObjectKind kind)
{
	//�f�[�^�̎�ނɂ���ăf�[�^��Ԃ�
	switch (kind)
	{
		case kObjectKind::Player:	
			return { "Player�f�[�^", 100 };
		case kObjectKind::Enemy:
			return { "Enemy�f�[�^", 50 };
		case kObjectKind::Shot:		
			return { "Shot�f�[�^", 10 };
		case kObjectKind::Field:		
			return { "Field�f�[�^", 1000 };
		default:					
			return { "�f�[�^������܂���", 0 };
	}
}

int main()
{
	//////////////////////////////////////
	// �֐����g���ď�����������炷(������₷������)
	//////////////////////////////////////
	//�v���C���[�̃f�[�^���֐��Ŏ擾
	std::cout << GetData(kObjectKind::Player).name << "�̗�:" << GetData(kObjectKind::Player).hp << std::endl;


	//////////////////////////////////////
	// �f�[�^�̎�ނɂ���ăf�[�^��Ԃ�
	//////////////////////////////////////
	//�v���C���[�̃f�[�^���̃f�[�^����擾
	std::cout << data[kObjectKind::Player].name << "�̗�:" << data[kObjectKind::Player].hp << std::endl;

	return 0;
}