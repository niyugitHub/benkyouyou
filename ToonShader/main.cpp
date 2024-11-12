#include <DxLib.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int ModelHandle;
	int GradTexHandle;
	int PixelShaderHandle;
	int VertexShaderHandle;
	int AnimIndex;
	float AnimCounter;

	// �E�C���h�E���[�h�ŋN��
	ChangeWindowMode(TRUE);

	// �c�w���C�u�����̏�����
	if (DxLib_Init() < 0)
	{
		// �G���[�����������璼���ɏI��
		return -1;
	}

	// �v���O���}�u���V�F�[�_�[���f���Q�D�O���g�p�ł��Ȃ��ꍇ�̓G���[��\�����ďI��
	if (GetValidShaderVersion() < 200)
	{
		// �G���[�\��
		DrawString(0, 0, L"�v���O���}�u���V�F�[�_�[�Q�D�O���g�p�ł��Ȃ����̂悤�ł�", GetColor(255, 255, 255));

		// �L�[���͑҂�
		WaitKey();

		// �c�w���C�u�����̌�n��
		DxLib_End();

		// �\�t�g�I��
		return 0;
	}


	// ���_�V�F�[�_�[��ǂݍ���
	VertexShaderHandle = LoadVertexShader(L"VertexShader.vso");

	// �s�N�Z���V�F�[�_�[��ǂݍ���
	PixelShaderHandle = LoadPixelShader(L"PixelShader.pso");


	// �O���f�[�V�����e�N�X�`����ǂݍ���
	GradTexHandle = LoadGraph(L"GradTex.bmp");

	// �X�L�j���O���b�V�����f����ǂݍ���
	ModelHandle = MV1LoadModel(L"DxChara.x");

	// ������₷���悤�ɕ��̃}�e���A����ΐF�ɂ���
	MV1SetMaterialDifColor(ModelHandle, 1, GetColorF(0.0f, 0.5f, 0.0f, 1.0f));


	// �A�j���[�V�����O���A�^�b�`
	AnimIndex = MV1AttachAnim(ModelHandle, 0);

	// �A�j���[�V�����J�E���^�����Z�b�g
	AnimCounter = 0.0f;


	// ���f���̕`��ɃI���W�i���V�F�[�_�[���g�p����ݒ���n�m�ɂ���
	MV1SetUseOrigShader(TRUE);

	// �g�p����e�N�X�`���P�ɃO���f�[�V�����e�N�X�`�����Z�b�g����
	SetUseTextureToShader(1, GradTexHandle);

	// �g�p���钸�_�V�F�[�_�[���Z�b�g
	SetUseVertexShader(VertexShaderHandle);

	// �g�p����s�N�Z���V�F�[�_�[���Z�b�g
	SetUsePixelShader(PixelShaderHandle);


	// �`���𗠉�ʂɂ���
	SetDrawScreen(DX_SCREEN_BACK);

	// ���f���̌�����ʒu�ɃJ������z�u
	SetCameraPositionAndTarget_UpVecY(VGet(0.0f, 700.0f, -1100.0f), VGet(0.0f, 350.0f, 0.0f));


	// ESC�L�[���������܂Ń��[�v
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{
		// ��ʂ�������
		ClearDrawScreen();

		// �A�j���[�V�������Ԃ�i�߂�
		AnimCounter += 100.0f;
		if (AnimCounter > MV1GetAnimTotalTime(ModelHandle, 0))
		{
			AnimCounter -= MV1GetAnimTotalTime(ModelHandle, 0);
		}
		MV1SetAttachAnimTime(ModelHandle, AnimIndex, AnimCounter);

		// ���f����`��
		MV1DrawModel(ModelHandle);

		// ����ʂ̓��e��\��ʂɔ��f������
		ScreenFlip();
	}

	// �g�p����e�N�X�`������O���f�[�V�����e�N�X�`�����O��
	SetUseTextureToShader(1, -1);

	// �O���f�[�V�����e�N�X�`�����폜
	DeleteGraph(GradTexHandle);

	// �ǂݍ��񂾒��_�V�F�[�_�[�̍폜
	DeleteShader(VertexShaderHandle);

	// �ǂݍ��񂾃s�N�Z���V�F�[�_�[�̍폜
	DeleteShader(PixelShaderHandle);

	// �ǂݍ��񂾃��f���̍폜
	MV1DeleteModel(ModelHandle);

	// �c�w���C�u�����̌�n��
	DxLib_End();

	// �\�t�g�̏I��
	return 0;
}