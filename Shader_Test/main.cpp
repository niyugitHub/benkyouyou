#include<DxLib.h>
#include <array>
#include<cassert>

void DrawGraphWithShader(int x, int y, int texH, int normH, int dep,  int psH, bool transFlg) {
	int w, h;
	int result = GetGraphSize(texH, &w, &h);
	assert(result != -1);
	std::array<VERTEX2DSHADER, 4> verts;
	//頂点をZの字になるように並べていく
	//左上
	verts[0].pos.x = x;
	verts[0].pos.y = y;
	verts[0].u = 0.0f;
	verts[0].v = 0.0f;

	//右上
	verts[1].pos.x = x + w;
	verts[1].pos.y = y;
	verts[1].u = 1.0f;
	verts[1].v = 0.0f;

	//左下
	verts[2].pos.x = x;
	verts[2].pos.y = y + h;
	verts[2].u = 0.0f;
	verts[2].v = 1.0f;

	//左下
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
	//どのシェーダを使うのか？
	SetUsePixelShader(psH);
	//どのテクスチャを何番に割り当てるのか？
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

	//通常レンダーターゲット
	int rt = MakeScreen(640, 480);
	int normRT = MakeScreen(640, 480);

	//チャンネル数を１にして一色当たりのビット数を３２にする
	//深度地を書き込むためには８ビットでは足りないため
	SetCreateGraphChannelBitDepth(32);
	SetCreateDrawValidGraphChannelNum(1);
	int depthRT = MakeScreen(640, 480);

	//ボケのためのレンダーターゲット
	int dep = GetCreateGraphChannelBitDepth();
	int cnum = GetCreateDrawValidGraphChannelNum();
	SetCreateGraphChannelBitDepth(0);
	SetCreateDrawValidGraphChannelNum(0);

	//ボケのための画像
	int boke0 = MakeScreen(640, 480);
	int boke1 = MakeScreen(640, 480);

	//定数バッファの作成(GPU上に作っている)
	auto cbufferH = CreateShaderConstantBuffer(sizeof(float) * 4);
	assert(cbufferH != -1);

	//中身をいじるには、このGPUのミラー(双子をゲットする必要がある)
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

		//DrawScreenを変更するたびになぜかカメラがリセットされるから
		//マイフレームカメラ設定を行う
		//3Dのカメラ設定
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

		//書き込んだ値をGPUに反映
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