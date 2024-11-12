#include<DxLib.h>
#include<array>
#include<cassert>
#include<cmath>
using namespace std;

namespace
{
	constexpr int shadowMapSizeX = 6400;
	constexpr int shadowMapSizeY = 4800;

	constexpr VECTOR lightPos = { 2000, 3000, 1000 };
}

/// <summary>
/// DxLib::DrawGraph�̑���
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="texH"></param>
/// <param name="normH"></param>
/// <param name="psH"></param>
/// <param name="transFlg"></param>
void DrawGraphWithShader(int x, int y, int texH, int normH, int depH,int psH, bool transFlg) {
	int w, h;
	int result = GetGraphSize(texH, &w, &h);
	assert(result != -1);
	array<VERTEX2DSHADER, 4> verts;
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
	SetUseTextureToShader(1, normH);
	SetUseTextureToShader(2, depH);
	if (transFlg) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	}
	else {
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	DrawPrimitive2DToShader(verts.data(), verts.size(), DX_PRIMTYPE_TRIANGLESTRIP);
}


/// <summary>
/// �r���[�v���W�F�N�V�����s����擾���邽�߂̊֐�
/// </summary>
/// <param name="viewprojectionMatrix"></param>
MATRIX ViewProjectionMatrix(VECTOR cameraTarget);

void UpdatePlayer1(VECTOR& pos, float& angle);
void UpdatePlayer2(VECTOR& pos, float& angle);


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	ChangeWindowMode(true);
	if (DxLib_Init() == -1) {
		return -1;
	}
	SetDrawScreen(DX_SCREEN_BACK);

	int psH = LoadPixelShader(L"PixelShader.pso");
	assert(psH != -1);

	int sphereH = -1;

	//���̃��f�����g����
	int modelType = 2;

	if (modelType == 0)
	{
		sphereH = MV1LoadModel(L"model/bodyeater.mv1");
	}
	else if (modelType == 1)
	{
		sphereH = MV1LoadModel(L"model/sphere.mqo");
		MV1SetScale(sphereH, VGet(0.5f, 0.5f, 0.5f));
	}
	else if (modelType == 2)
	{
		sphereH = MV1LoadModel(L"model/Knight.mv1");
		MV1SetScale(sphereH, VGet(50, 50, 50));
	}
	else if (modelType == 3)
	{
		sphereH = MV1LoadModel(L"model/Bard.mv1");
		//MV1SetScale(sphereH, VGet(25, 25, 25));
	}
	else if (modelType == 4)
	{
		sphereH = MV1LoadModel(L"model/Character.mv1");
		MV1SetScale(sphereH, VGet(225, 225, 225));
	}
	else if (modelType == 5)
	{
		sphereH = MV1LoadModel(L"model/Skeleton.mv1");
		MV1SetScale(sphereH, VGet(225, 225, 225));
	}

	//�t�B�[���h���f�����[�h
	int modelField = MV1LoadModel(L"model/field3.mv1");
	MV1SetPosition(modelField, VGet(0, 0, 0));
	MV1SetScale(modelField, VGet(1.0f,1.0f,1.0f));

	assert(sphereH != -1);

	int distH = LoadGraph(L"turbulant_norm.png");

	int vs3DH1 = -1;
	int vs3DH2 = -1;
	int vs3DH3 = -1;
	int vs3DH4 = -1;
	int vs3DH5 = -1;
	int vs3DH6 = -1;
	//int vs3DH = LoadVertexShader(L"VertexShader3D.vso");
	int ps3DH = LoadPixelShader(L"PixelShader3D.pso");

	//�V���h�E�}�b�v�p�̃V�F�[�_�[���[�h
	int shadowPs = LoadPixelShader(L"PixelShadowMapShader.pso");

	int shadowVsField = LoadVertexShader(L"VertexShaderShadowMapField.vso");
	int shadowPsField = LoadPixelShader(L"PixelShaderShadowMapField.pso");

	//3D�̃J�����ݒ�(DxLib)
	auto v=GetCameraPosition();
	
	//�ʏ탌���_�[�^�[�Q�b�g
	int rt=MakeScreen(640, 480,false);
	int normRT = MakeScreen(640, 480, true);
	
	//�`�����l�������P�ɂ��ĂP�F������̃r�b�g����32�ɂ���
	SetCreateGraphChannelBitDepth(32);
	SetCreateDrawValidGraphChannelNum(1);
	int depthRT = MakeScreen(640, 480);
	//�`�����l�������r�b�g�������Z�b�g���܂�
	SetCreateGraphChannelBitDepth(0);
	SetCreateDrawValidGraphChannelNum(0);
	
	//�{�P�̂��߂̃����_�[�^�[�Q�b�g
	int boke0 = MakeScreen(640, 480);
	int boke1 = MakeScreen(640, 480);

	float amplitudeX = 0.0f;//��ʗh��萔
	struct KeyState {
		char data[256];
	};
	KeyState keyState = {};
	KeyState lastKeyState = {};

	//���̍��W�Z�b�g
	SetLightPosition(lightPos);

	//�萔�o�b�t�@�̍쐬(GPU��ɂ����Ă���)
	auto cbufferH=CreateShaderConstantBuffer(sizeof(MATRIX));
	assert(cbufferH != -1);

	MATRIX* mat = static_cast<MATRIX*>(GetBufferShaderConstantBuffer(cbufferH));

	vs3DH1 = LoadVertexShader(L"VertexShader3D.vso");
	vs3DH2 = LoadVertexShader(L"VertexShader3D2.vso");
	vs3DH3 = LoadVertexShader(L"VertexShader3D2.vso");
	vs3DH4 = LoadVertexShader(L"VertexShader.Normal4Frame.vso");
	vs3DH5 = LoadVertexShader(L"VertexShaderNomal8Frame.vso");
	vs3DH6 = LoadVertexShader(L"VertexShader3D2.vso");

	int tlVertType = -1;
	for (int i = 0; i < MV1GetTriangleListNum(sphereH); ++i) {
		tlVertType = MV1GetTriangleListVertexType(sphereH, i);
		if (tlVertType == DX_MV1_VERTEX_TYPE_1FRAME)
		{
			printfDx(L"�P");
			
		}
		else if (tlVertType == DX_MV1_VERTEX_TYPE_4FRAME)
		{
			printfDx(L"�Q");
			
		}
		else if (tlVertType == DX_MV1_VERTEX_TYPE_NMAP_1FRAME)
		{
			printfDx(L"�R");
		}
		else if (tlVertType == DX_MV1_VERTEX_TYPE_NMAP_4FRAME)
		{
			printfDx(L"�S");
		}
		else if (tlVertType == DX_MV1_VERTEX_TYPE_NMAP_8FRAME)
		{
			printfDx(L"5");
		}
		else if (tlVertType == DX_MV1_VERTEX_TYPE_NMAP_FREE_FRAME)
		{
			printfDx(L"6");
		}
		else
		{
			printfDx(L"7");
		}
	}

	//int shadowRT = MakeScreen(320, 240);
	int shadowRT = MakeScreen(shadowMapSizeX, shadowMapSizeY, true);
	
	//���g��M��ɂ́A����GPU�̃~���[(�o�q)���Q�b�g����K�v������
	//float* cbufferAddress = static_cast<float*>(GetBufferShaderConstantBuffer(cbufferH));

	VECTOR pos = VGet(0, 0, 0);
	VECTOR pos2 = VGet(100, 0, 100);
	float angle = 0.0f;
	float angle2 = 0.0f;

	while (ProcessMessage()!=-1)
	{

		GetHitKeyStateAll(keyState.data);
		//SetDrawScreen(rt);
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();

		MV1SetPosition(sphereH, pos);
		MV1SetRotationXYZ(sphereH, VGet(0,angle,0));
		//DrawBox(0, 0, 640, 480, 0xffffff, true);

		MV1SetUseOrigShader(true);

		//DrawScreen��ύX���邽�тɂȂ����J���������Z�b�g�����̂�
		//���t���[���J�����ݒ���s���B
		SetCameraPositionAndTarget_UpVecY(VGet(0, 700, -700), pos);
		SetupCamera_Perspective(DX_PI_F / 4.0f);
		SetCameraNearFar(5.1f, 1400.0f);
		for (int i = 0; i < MV1GetTriangleListNum(sphereH); ++i)
		{

			tlVertType = MV1GetTriangleListVertexType(sphereH, i);

			if (tlVertType == DX_MV1_VERTEX_TYPE_1FRAME)
			{
				//continue;
				SetUseVertexShader(vs3DH1);
			}
			if (tlVertType == DX_MV1_VERTEX_TYPE_4FRAME)
			{
				//continue;
				SetUseVertexShader(vs3DH2);
			}
			if (tlVertType == DX_MV1_VERTEX_TYPE_NMAP_1FRAME)
			{
				SetUseVertexShader(vs3DH2);
			}
			if (tlVertType == DX_MV1_VERTEX_TYPE_NMAP_4FRAME)
			{
				SetUseVertexShader(vs3DH4);
			}
			if (tlVertType == DX_MV1_VERTEX_TYPE_NMAP_8FRAME)
			{
				SetUseVertexShader(vs3DH5);
			}
			else
			{
				
				//assert(false);
			}
			SetUsePixelShader(ps3DH);

			
			MV1DrawTriangleList(sphereH, i);
		}

		MV1SetPosition(sphereH, pos2);
		MV1SetRotationXYZ(sphereH, VGet(0, angle2, 0));

		for (int i = 0; i < MV1GetTriangleListNum(sphereH); ++i)
		{

			tlVertType = MV1GetTriangleListVertexType(sphereH, i);

			if (tlVertType == DX_MV1_VERTEX_TYPE_1FRAME)
			{
				//continue;
				SetUseVertexShader(vs3DH1);
			}
			if (tlVertType == DX_MV1_VERTEX_TYPE_4FRAME)
			{
				//continue;
				SetUseVertexShader(vs3DH2);
			}
			if (tlVertType == DX_MV1_VERTEX_TYPE_NMAP_1FRAME)
			{
				SetUseVertexShader(vs3DH2);
			}
			if (tlVertType == DX_MV1_VERTEX_TYPE_NMAP_4FRAME)
			{
				SetUseVertexShader(vs3DH4);
			}
			if (tlVertType == DX_MV1_VERTEX_TYPE_NMAP_8FRAME)
			{
				SetUseVertexShader(vs3DH5);
			}
			else
			{

				//assert(false);
			}
			SetUsePixelShader(ps3DH);


			MV1DrawTriangleList(sphereH, i);
		}

		//�e�L�����X�V����
		UpdatePlayer1(pos, angle);
		UpdatePlayer2(pos2, angle2);

		MV1SetPosition(sphereH, pos);
		MV1SetRotationXYZ(sphereH, VGet(0, angle, 0));





		//�V���h�E�}�b�v�ɏ����݊J�n
		DeleteGraph(shadowRT);
		shadowRT = MakeScreen(shadowMapSizeX, shadowMapSizeY, true);
		SetDrawScreen(shadowRT);
		//�V���h�E�}�b�v�`��̂���
		//SetCameraPositionAndTarget_UpVecY(GetLightPosition(), pos);
		//SetCameraPositionAndTargetAndUpVec(GetLightPosition(), VGet(0,0,0), VGet(1, 0, 0));
		SetCameraPositionAndTargetAndUpVec(GetLightPosition(), pos, VGet(1, 0, 0));

		DrawBox(0, 0, shadowMapSizeX, shadowMapSizeY, 0xffffff, true);

		//�V���h�E�}�b�v�`��
		for (int i = 0; i < MV1GetTriangleListNum(sphereH); ++i)
		{
			tlVertType = MV1GetTriangleListVertexType(sphereH, i);

			if (tlVertType == DX_MV1_VERTEX_TYPE_1FRAME)
			{
				//continue;
				SetUseVertexShader(vs3DH1);
			}
			if (tlVertType == DX_MV1_VERTEX_TYPE_4FRAME)
			{
				//continue;
				SetUseVertexShader(vs3DH2);
			}
			
			//SetUsePixelShader(ps3DH);
			SetUsePixelShader(shadowPs);

			MV1DrawTriangleList(sphereH, i);
		}

		//VECTOR a = VSub(GetLightPosition(), VGet(-pos.x, 0, -pos.z));
		//SetCameraPositionAndTargetAndUpVec(GetLightPosition(), pos2, VGet(1, 0, 0));

		VECTOR c = VGet(0,0,0);
		c.x = pos.x - GetLightPosition().x / GetLightPosition().y * GetLightPosition().x;
		c.z = pos.z - GetLightPosition().z / GetLightPosition().y * GetLightPosition().y;
		VECTOR b = VAdd(VSub(pos2, c), pos2);
		b.y += 100;
		//MV1SetPosition(sphereH, pos2);
		//MV1SetPosition(sphereH, pos);
		MV1SetPosition(sphereH, VAdd(VSub(pos2, pos), pos2));
		//MV1SetPosition(sphereH, b);
		/*VECTOR b = VScale(pos, -1.0f);
		MV1SetPosition(sphereH, VAdd(b, pos2));*/

		MV1SetRotationXYZ(sphereH, VGet(0, angle2, 0));

		
		for (int i = 0; i < MV1GetTriangleListNum(sphereH); ++i)
		{
			tlVertType = MV1GetTriangleListVertexType(sphereH, i);

			if (tlVertType == DX_MV1_VERTEX_TYPE_1FRAME)
			{
				//continue;
				SetUseVertexShader(vs3DH1);
			}
			if (tlVertType == DX_MV1_VERTEX_TYPE_4FRAME)
			{
				//continue;
				SetUseVertexShader(vs3DH2);
			}

			//SetUsePixelShader(ps3DH);
			SetUsePixelShader(shadowPs);

			MV1DrawTriangleList(sphereH, i);
		}
		//SetRenderTargetToShader(0, shadowRT);
		SetDrawScreen(DX_SCREEN_BACK);
	

		SetCameraPositionAndTargetAndUpVec(GetLightPosition(), pos, VGet(1, 0, 0));
		//�����ӓ_
		//DrawScreen��ύX���邽�тɂȂ����J���������Z�b�g�����̂�
		//���t���[���J�����ݒ���s���B
		SetCameraPositionAndTarget_UpVecY(VGet(0, 700, -700), pos);
		SetupCamera_Perspective(DX_PI_F / 4.0f);
		SetCameraNearFar(5.1f, 1400.0f);



		//MV1SetUseOrigShader(false);

		//*mat = ViewProjectionMatrix(VGet(0.0f,0.0f,0.0f));
		*mat = ViewProjectionMatrix(pos);

		SetUseTextureToShader(1, shadowRT);

		//�������񂾒l��GPU�ɔ��f
		UpdateShaderConstantBuffer(cbufferH);
		SetShaderConstantBuffer(cbufferH, DX_SHADERTYPE_VERTEX, 4);

		SetUseVertexShader(shadowVsField);
		SetUsePixelShader(shadowPsField);


		MV1DrawModel(modelField);

		MV1SetUseOrigShader(false);

		VECTOR a = VSub(pos2, pos);
		DrawFormatString(0, 50, 0xffffff, L"x:%f y:%f z:%f", a.x, a.y, a.z);
		//DrawGraph(0, 0, shadowRT, true);
		ScreenFlip();
	}


	DxLib_End();
}

MATRIX ViewProjectionMatrix(VECTOR cameraTarget)
{
	//�r���[�s��̕ϐ�
	MATRIX viewMat = GetCameraViewportMatrix();

	//VECTOR cameraPos = VGet(300, 500, -500);
	VECTOR cameraPos = GetLightPosition();
	//VECTOR targetPos = VGet(cameraTarget.x, cameraTarget.y, cameraTarget.z);
	VECTOR targetPos = VGet(cameraTarget.x, cameraTarget.y, cameraTarget.z);
	//VECTOR UpVec = GetCameraUpVector();
	VECTOR UpVec = VGet(1,0,0);

	//�r���[�s����Z�o
	//CreateLookAtMatrix(&viewMat, &cameraPos, &targetPos, &UpVec);
	CreateLookAtMatrix(&viewMat, &cameraPos, &targetPos, &UpVec);
	//�ˉe�s����쐬
	MATRIX mat;

	float Fov = GetCameraFov();
	float Near = GetCameraNear();
	float Far = GetCameraFar();
	float Aspect = GetCameraDotAspect();

	//�ˉe�s��Z�o
	//�s��A����p�Aneir,far,(�A�X�y�N�g��)
	CreatePerspectiveFovMatrix(&mat, GetCameraFov(), GetCameraNear(), GetCameraFar(), GetCameraDotAspect());

	//�r���[�v���W�F�N�V�����s��̍쐬
	MATRIX viewProjectionMat = MMult(viewMat,mat);

	return viewProjectionMat;
}

void UpdatePlayer1(VECTOR& pos, float& angle)
{
	if (CheckHitKey(KEY_INPUT_LEFT))
	{
		pos.x -= 3.0f;
	}
	if (CheckHitKey(KEY_INPUT_RIGHT))
	{
		pos.x += 3.0f;
	}
	if (CheckHitKey(KEY_INPUT_UP))
	{
		pos.z += 3.0f;
	}
	if (CheckHitKey(KEY_INPUT_DOWN))
	{
		pos.z -= 3.0f;
	}
	if (CheckHitKey(KEY_INPUT_A))
	{
		angle += 0.02f;
	}
	if (CheckHitKey(KEY_INPUT_D))
	{
		angle -= 0.02f;
	}
}

void UpdatePlayer2(VECTOR& pos, float& angle)
{
	if (CheckHitKey(KEY_INPUT_1))
	{
		pos.x -= 3.0f;
	}
	if (CheckHitKey(KEY_INPUT_2))
	{
		pos.x += 3.0f;
	}
	if (CheckHitKey(KEY_INPUT_3))
	{
		pos.z += 3.0f;
	}
	if (CheckHitKey(KEY_INPUT_4))
	{
		pos.z -= 3.0f;
	}
	if (CheckHitKey(KEY_INPUT_5))
	{
		angle += 0.02f;
	}
	if (CheckHitKey(KEY_INPUT_6))
	{
		angle -= 0.02f;
	}
}
