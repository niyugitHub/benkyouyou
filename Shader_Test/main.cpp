#include<DxLib.h>
#include <array>
#include<cassert>

void DrawGraphWithShader(int x, int y, int texH, int normH, int dep,  int psH, bool transFlg) {
	int w, h;
	int result = GetGraphSize(texH, &w, &h);
	assert(result != -1);
	std::array<VERTEX2DSHADER, 4> verts;
	//���_��Z�̎��ɂȂ�悤�ɕ��ׂĂ���
	//����
	verts[0].pos.x = x;
	verts[0].pos.y = y;
	verts[0].u = 0.0f;
	verts[0].v = 0.0f;

	//�E��
	verts[1].pos.x = x + w;
	verts[1].pos.y = y;
	verts[1].u = 1.0f;
	verts[1].v = 0.0f;

	//����
	verts[2].pos.x = x;
	verts[2].pos.y = y + h;
	verts[2].u = 0.0f;
	verts[2].v = 1.0f;

	//����
	verts[3].pos.x = x + w;
	verts[3].pos.y = y + h;
	verts[3].u = 1.0f;
	verts[3].v = 1.0f;

	for (auto& v : verts) {
		v.dif = GetColorU8(255, 255, 255, 255);
		v.spc = GetColorU8(255, 255, 255, 255);
		v.pos.z = 0.5f;
		v.rhw = 1.0f;
		v.su = 0.0f;
		v.sv = 0.0f;
	}
	//�ǂ̃V�F�[�_���g���̂��H
	SetUsePixelShader(psH);
	//�ǂ̃e�N�X�`�������ԂɊ��蓖�Ă�̂��H
	SetUseTextureToShader(0, texH);
	SetUseTextureToShader(1, dep);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	/*if (transFlg) {
	}
	else {
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}*/
	DrawPrimitive2DToShader(verts.data(), verts.size(), DX_PRIMTYPE_TRIANGLESTRIP);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) 
{
	ChangeWindowMode(true);
	if (DxLib_Init() == -1) {
		return -1;
	}
	SetDrawScreen(DX_SCREEN_BACK);

	int psH = LoadPixelShader(L"PixelShader.pso");
	assert(psH != -1);
	int sphereH = MV1LoadModel(L"Knight.mv1");
	//MV1SetScale(sphereH, VGet(0.1f, 0.1f, 0.1f));
	MV1SetScale(sphereH, VGet(10.0f, 10.0f, 10.0f));
	//MV1SetRotationXYZ(sphereH, VGet(0, 90, 0));
	assert(sphereH != -1);

	int vs3DH = LoadVertexShader(L"VertexShader3D.vso");
	int ps3DH = LoadPixelShader(L"PixelShader3D.pso");

	//�ʏ탌���_�[�^�[�Q�b�g
	int rt = MakeScreen(640, 480);
	int normRT = MakeScreen(640, 480);

	//�`�����l�������P�ɂ��Ĉ�F������̃r�b�g�����R�Q�ɂ���
	//�[�x�n���������ނ��߂ɂ͂W�r�b�g�ł͑���Ȃ�����
	SetCreateGraphChannelBitDepth(32);
	SetCreateDrawValidGraphChannelNum(1);
	int depthRT = MakeScreen(640, 480);

	//�{�P�̂��߂̃����_�[�^�[�Q�b�g
	int dep = GetCreateGraphChannelBitDepth();
	int cnum = GetCreateDrawValidGraphChannelNum();
	SetCreateGraphChannelBitDepth(0);
	SetCreateDrawValidGraphChannelNum(0);

	//�{�P�̂��߂̉摜
	int boke0 = MakeScreen(640, 480);
	int boke1 = MakeScreen(640, 480);

	//�萔�o�b�t�@�̍쐬(GPU��ɍ���Ă���)
	auto cbufferH = CreateShaderConstantBuffer(sizeof(float) * 4);
	assert(cbufferH != -1);

	//���g��������ɂ́A����GPU�̃~���[(�o�q���Q�b�g����K�v������)
	float* cbufferAddress = static_cast<float*>(GetBufferShaderConstantBuffer(cbufferH));

	float time = 0.0f;
	while (ProcessMessage() != -1)
	{
		cbufferAddress[0] = time;
		
		SetDrawScreen(rt);
		ClearDrawScreen();

		DrawBox(0, 0, 640, 480, 0xffffff, true);

		MV1SetUseOrigShader(true);
		SetUseVertexShader(vs3DH);
		SetUsePixelShader(ps3DH);

		SetRenderTargetToShader(0, rt);
		SetRenderTargetToShader(1, depthRT);
		SetRenderTargetToShader(2, normRT);

		//DrawScreen��ύX���邽�тɂȂ����J���������Z�b�g����邩��
		//�}�C�t���[���J�����ݒ���s��
		//3D�̃J�����ݒ�
		SetCameraPositionAndTarget_UpVecY(VGet(0, 0, -50), VGet(0, 0, 0));
		SetupCamera_Perspective(DX_PI_F / 4.0f);
		SetCameraNearFar(0.1f, 900.0f);

		MV1SetPosition(sphereH, VGet(-10, -10, 0));
		MV1DrawModel(sphereH);
		MV1SetPosition(sphereH, VGet(10, -10, -20));
		MV1DrawModel(sphereH);

		SetRenderTargetToShader(0, -1);
		SetRenderTargetToShader(1, -1);
		SetRenderTargetToShader(2, -1);

		SetUseTextureToShader(1, depthRT);

		SetDrawScreen(DX_SCREEN_BACK);

		//�������񂾒l��GPU�ɔ��f
		UpdateShaderConstantBuffer(cbufferH);
		SetShaderConstantBuffer(cbufferH, DX_SHADERTYPE_PIXEL, 8);


		GraphFilterBlt(rt, boke0, DX_GRAPH_FILTER_GAUSS, 8, 500);
		GraphFilterBlt(rt, boke1, DX_GRAPH_FILTER_GAUSS, 32, 500);

		SetUseTextureToShader(2, boke0);
		SetUseTextureToShader(3, boke1);
		SetUseTextureToShader(4, normRT);

		DrawGraphWithShader(0, 0, rt, normRT, depthRT, psH, true);

		SetUseTextureToShader(0, -1);
		SetUseTextureToShader(1, -1);
		SetUseTextureToShader(2, -1);
		SetUseTextureToShader(3, -1);
		SetUseTextureToShader(4, -1);

		SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 8);

		ScreenFlip();

		time += 1.0f / 100.0f;

		if (time > 1.0f)
		{
			time = 0.0f;
		}
	}
	DxLib_End();
}