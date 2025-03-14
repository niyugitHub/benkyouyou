#include<Windows.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<vector>
#include<assert.h>
#include<DirectXMath.h>
#include<d3dcompiler.h>
#ifdef _DEBUG
#include<iostream>
#endif // DEBUG

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace std;
using namespace DirectX;

namespace
{
	constexpr int window_width = 1280;
	constexpr int window_height = 720;
	//LPCWCHAR _T;
}

void DebugOutputFormatString(const char* format, ...)
{
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	vprintf(format, valist);
	va_end(valist);
#endif
}

//ウィンドウプロシ―ジャ
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);	//終了メッセージを送る
		break;
	default:
		return DefWindowProc(hWnd, msg, wparam, lparam);
	}
	return 0;
}

//フィーチャーレベル設定用関数
D3D_FEATURE_LEVEL GetSupportedFeatureLevel()
{
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	for (auto& lv : levels)
	{
		if (SUCCEEDED(D3D12CreateDevice(nullptr, lv, __uuidof(ID3D12Device), nullptr)))
		{
			return lv;
		}
	}
	return D3D_FEATURE_LEVEL_11_0;
}

//グラフィックボードの設定用関数
IDXGIAdapter* GetAdapter(IDXGIFactory6* dxgiFactory)
{
	//アダプターの列挙用
	std::vector<IDXGIAdapter*> adapters;

	//ここに特定の名前を持つアダプターオブジェクトが入る
	IDXGIAdapter* tmpAdapter = nullptr;

	for (int i = 0; dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc;
		adpt->GetDesc(&adesc);	// アダプターの説明オブジェクト取得

		std::wstring strDesc = adesc.Description;

#ifdef _DEBUG
		//アダプターの名前をデバッグ出力
		DebugOutputFormatString("Adapter : %s\n", std::string(strDesc.begin(), strDesc.end()).c_str());
#endif

		//アダプターの名前がNVIDIAで始まるものを選択
		if (strDesc.find(L"NVIDIA") != std::string::npos)
		{
			tmpAdapter = adpt;
			break;
		}
	}

	return tmpAdapter;
}

//デバッグレイヤーの設定用関数
void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

	debugLayer->EnableDebugLayer();	//デバッグレイヤーを有効化
	debugLayer->Release();			//有効化したらインターフェイスを開放
}

void CheckNullptr(HRESULT result)
{
	if (result == S_OK)
	{
		return;
	}
	else
	{
		assert(0 && "Error");
	}
}

IDXGIFactory6* _dxgiFactory = nullptr;
ID3D12Device* _dev = nullptr;
ID3D12CommandAllocator* _cmdAllocator = nullptr;
ID3D12GraphicsCommandList* _cmdList = nullptr;
ID3D12CommandQueue* _cmdQueue = nullptr;
IDXGISwapChain4* _swapchain = nullptr;

#ifdef _DEBUG
int main()
{
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR, int)
{
#endif // DEBUG
	DebugOutputFormatString("Show window test.");
	//getchar();

	//ウィンドウクラスの生成、登録
	WNDCLASSEX w = {};

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;	//コールバック関数の指定
    w.lpszClassName = L"DX12Sample";			//クラス名
	w.hInstance = GetModuleHandle(nullptr);		//ハンドルの取得

	RegisterClassEx(&w); //アプリケーションクラス(ウィンドウクラスの指定をOSに伝える)

	RECT wrc = { 0, 0, window_width, window_height };	//ウィンドウサイズを決める

	//関数を使ってウィンドウサイズを補正
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウオブジェクトの生成
	HWND hWnd = CreateWindow(
		w.lpszClassName,	//クラス名
		L"DX12テスト",		//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	//ウィンドウの種類
		CW_USEDEFAULT,			//表示する位置X(OSにお任せ)
		CW_USEDEFAULT,			//表示する位置Y(OSにお任せ)
		wrc.right - wrc.left,	//ウィンドウサイズ幅
		wrc.bottom - wrc.top,	//ウィンドウサイズ高さ
		nullptr,			//親ウィンドウのハンドル
		nullptr,			//メニューハンドル
		w.hInstance,		//呼び出しアプリケーションハンドル
		nullptr);			//追加パラメータ

	MSG msg = {};

#ifdef _DEBUG
	//デバッグレイヤーをオンに
	EnableDebugLayer();
#endif

	//フィーチャーレベルの設定
	D3D_FEATURE_LEVEL featureLevel = GetSupportedFeatureLevel();

	//グラフィックボードの設定
#ifdef _DEBUG
	auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&_dxgiFactory));
#else
	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
#endif

	CheckNullptr(result);

	result = D3D12CreateDevice(
	GetAdapter(_dxgiFactory),
	GetSupportedFeatureLevel(),
	IID_PPV_ARGS(&_dev)
	);

	CheckNullptr(result);

	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&_cmdAllocator));

	CheckNullptr(result);

	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		_cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));
	CheckNullptr(result);

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};

	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;		//タイムアウトなし
	cmdQueueDesc.NodeMask = 0;								//アダプターを一つしか使わないときは0でよい
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;//プライオリティは特に指定なし
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;	//コマンドリストと合わせる
	//キュー生成
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));

	CheckNullptr(result);

	//スワップチェインの生成
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};

	swapchainDesc.Width = window_width;	//ウィンドウ幅
	swapchainDesc.Height = window_height;	//ウィンドウ高さ
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//フォーマット
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;	//マルチサンプリング設定
	swapchainDesc.SampleDesc.Quality = 0;	//マルチサンプリング設定
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;	//バッファの使用法
	swapchainDesc.BufferCount = 2;	//バッファ数
	
	//バックバッファーは伸び縮み可能
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;	//スケーリング設定
	
	//フリップ後は速やかに破棄
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	//スワップエフェクト
	
	//特に指定なし
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;	//アルファモード設定

	//ウィンドウからフルスクリーン切り替え可能(逆も可)
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	//フラグ設定

	result = _dxgiFactory->CreateSwapChainForHwnd(
		_cmdQueue, hWnd,
		&swapchainDesc, nullptr, nullptr, 
		(IDXGISwapChain1**)&_swapchain);	//本来はQueryInterfaceなどを用いて
											//IDXGISwapChain4*への変換チェックをするが、
											//今回は分かりやすさのためキャストで対応

	CheckNullptr(result);
											

	//ディスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	//レンダーターゲットビューなのでRTV
	heapDesc.NodeMask = 0;	//アダプターが一つの場合は0
	heapDesc.NumDescriptors = 2;	//裏表の二つ
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	//特に指定なし

	ID3D12DescriptorHeap* _rtvHeaps = nullptr;

	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_rtvHeaps));

	CheckNullptr(result);

	//レンダーターゲットビューの作成
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = _swapchain->GetDesc(&swcDesc);

	CheckNullptr(result);

	//スワップチェーンの数だけバックバッファを生成生成
	std::vector<ID3D12Resource*> _backBuffers(swcDesc.BufferCount);

	//ディスクリプタヒープの先頭アドレスを取得
	D3D12_CPU_DESCRIPTOR_HANDLE handle
		= _rtvHeaps->GetCPUDescriptorHandleForHeapStart();

	for (int idx = 0; idx < swcDesc.BufferCount; idx++)
	{
		//バックバッファにスワップチェーンのバッファを設定
		result = _swapchain->GetBuffer(idx, IID_PPV_ARGS(&_backBuffers[idx]));
		CheckNullptr(result);

		//レンダーターゲットビューを生成
		_dev->CreateRenderTargetView(
			_backBuffers[idx],
			nullptr,
			handle);

		handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	//フェンスの生成
	ID3D12Fence* _fence = nullptr;
	UINT64 _fenceVal = 0;

	result = _dev->CreateFence(
		_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));

	//ウィンドウ表示
	ShowWindow(hWnd, SW_SHOW);

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
	};

	//頂点座標の定義
	Vertex vertices[] =
	{
		{{-0.4f, -0.7f, 0.0f} ,{0.0f,1.0f}},	//左下
		{{-0.4f,  0.7f, 0.0f} ,{0.0f,0.0f}},	//左上
		{{0.4f, -0.7f, 0.0f} ,	{1.0f,1.0f}},//右下
		{{0.4f,  0.7f, 0.0f} ,	{1.0f,0.0f}},//右上
	};

	struct TexRGBA
	{
		unsigned char R, G, B, A;
	};

	//テクスチャデータの生成
	std::vector<TexRGBA> texturedata(256 * 256);

	for (auto& rgba : texturedata)
	{
		rgba.R = rand() % 256;
		rgba.G = rand() % 256;
		rgba.B = rand() % 256;
		rgba.A = 255; // aは1.0fとする
	}

	// WriteToSubresourceで転送するためのヒープ設定
	D3D12_HEAP_PROPERTIES heapprop = {};

	// 特殊な設定なのでDEFAULTでもUPLOADでもない
	heapprop.Type = D3D12_HEAP_TYPE_CUSTOM;

	// ライトバック
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;

	// 転送はL0、つまりCPU側から直接行う
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	// 単一アダプターのため0
	heapprop.CreationNodeMask = 0;
	heapprop.VisibleNodeMask = 0;


	D3D12_RESOURCE_DESC resDesc = {};
	
	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // RGBAフォーマット
	resDesc.Width = 256; // 幅
	resDesc.Height = 256; // 高さ
	resDesc.DepthOrArraySize = 1; // 2Dで配列でもないので1
	resDesc.SampleDesc.Count = 1; // 通常テクスチャなのでアンチエイリアシングしない
	resDesc.SampleDesc.Quality = 0; // クオリティは最低
	resDesc.MipLevels = 1; // ミップマップしないのでミップ数は1つ
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2Dテクスチャ用
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // レイアウトは決定しない
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE; // 特にフラグはなし

	ID3D12Resource* texbuff = nullptr;

	result = _dev->CreateCommittedResource(
		&heapprop, D3D12_HEAP_FLAG_NONE, //特に指定なし
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(&texbuff));

	if (result != S_OK)
	{
		return 0;
	}

	result = texbuff->WriteToSubresource(
		0,
		nullptr,	//全領域へコピー
		texturedata.data(),	//元データアドレス
		sizeof(TexRGBA) * 256, //1ラインサイズ
		sizeof(TexRGBA) * texturedata.size());	//全サイズ

	if (result != S_OK)
	{
		return 0;
	}

	ID3D12DescriptorHeap* _texDescHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};

	//シェーダーから見えるように
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	//マスクは0
	descHeapDesc.NodeMask = 0;

	//ビューは今のことろ1つだけ
	descHeapDesc.NumDescriptors = 1;

	//シェーダーリソースビュー用
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	//生成
	result = _dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&_texDescHeap));

	if (result != S_OK)
	{
		return 0;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//RGBA(0.0f〜1.0fに正規化)
	srvDesc.Shader4ComponentMapping = 
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;	//後述
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2Dテクスチャ
	srvDesc.Texture1D.MipLevels = 1;	//ミップマップは使用しないので1

	_dev->CreateShaderResourceView(
		texbuff,	//ビューと関連付けるバッファー
		&srvDesc,	//先ほど設定したテクスチャ設定情報
		_texDescHeap->GetCPUDescriptorHandleForHeapStart()	//ヒープのどこに割り当てるか
	);







	//頂点バッファの生成
	D3D12_HEAP_PROPERTIES heapProp = {};

	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//アップロードヒープ
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	//CPUページプロパティ
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	//メモリプールプリファレンス

	D3D12_RESOURCE_DESC resdesc = {};

	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	//バッファ
	resdesc.Width = sizeof(vertices);	//バッファサイズ
	resdesc.Height = 1;	//高さ
	resdesc.DepthOrArraySize = 1;	//深さ
	resdesc.MipLevels = 1;	//ミップマップレベル
	resdesc.Format = DXGI_FORMAT_UNKNOWN;	//フォーマット
	resdesc.SampleDesc.Count = 1;	//サンプル数
	resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;	//フラグ
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	//レイアウト

	ID3D12Resource* vertBuff = nullptr;

	result = _dev->CreateCommittedResource(
		&heapProp, D3D12_HEAP_FLAG_NONE, &resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff));

	CheckNullptr(result);

	//頂点バッファのメモリコピー
	Vertex* vertMap = nullptr;

	result = vertBuff->Map(0, nullptr, (void**)&vertMap);

	std::copy(std::begin(vertices), std::end(vertices), vertMap);

	vertBuff->Unmap(0, nullptr);

	//頂点バッファービューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();	//バッファの仮想アドレス
	vbView.SizeInBytes = sizeof(vertices);			//全バイト数
	vbView.StrideInBytes = sizeof(vertices[0]);		//1頂点当たりのバイト数


	unsigned short indices[] = { 0,1,2, 2,1,3 };

	ID3D12Resource* idxBuff = nullptr;
	//設定は、バッファのサイズ以外頂点バッファの設定を使いまわして
	//OKだと思います。
	resdesc.Width = sizeof(indices);
	result = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&idxBuff));

	//作ったバッファにインデックスデータをコピー
	unsigned short* mappedIdx = nullptr;
	idxBuff->Map(0, nullptr, (void**)&mappedIdx);
	std::copy(std::begin(indices), std::end(indices), mappedIdx);
	idxBuff->Unmap(0, nullptr);

	//インデックスバッファビューを作成
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	ibView.BufferLocation = idxBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indices);


	//シェーダーオブジェクトの生成
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"BasicVertexShader.hlsl",
		nullptr,//defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルードはデフォルト
		"BasicVS",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用および最適化なし
		0,
		&vsBlob,
		&errorBlob);

	CheckNullptr(result);

	result = D3DCompileFromFile(
		L"BasicPixelShader.hlsl",
		nullptr,//defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルードはデフォルト
		"BasicPS",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用および最適化なし
		0,
		&psBlob,
		&errorBlob);

	//デバッグウィンドウ表示
#ifdef _DEBUG
	if (FAILED(result))
	{
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			OutputDebugStringA("ファイルが見つかりません");
			return 0;
		}
		else
		{
			std::string errstr;	//受け取り用stirng
			errstr.resize(errorBlob->GetBufferSize());//必要なサイズを確保

			//データコピー
			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());

			OutputDebugStringA(errstr.c_str());
		}
	}
#endif

	//頂点レイアウトの作成
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{	//座標追加
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	//UV追加
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}
	};

	//グラフィックスパイプラインステートの生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};

	gpipeline.pRootSignature = nullptr;	//ルートシグネチャ

	gpipeline.VS.pShaderBytecode = vsBlob->GetBufferPointer();	//頂点シェーダーバイトコード
	gpipeline.VS.BytecodeLength = vsBlob->GetBufferSize();		//頂点シェーダーバイトコードサイズ
	gpipeline.PS.pShaderBytecode = psBlob->GetBufferPointer();	//ピクセルシェーダーバイトコード
	gpipeline.PS.BytecodeLength = psBlob->GetBufferSize();		//ピクセルシェーダーバイトコードサイズ

	//デフォルトのサンプルマスクを表す定数
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//まだアンチエイリアスは使わないためfalse
	gpipeline.RasterizerState.AntialiasedLineEnable = false;

	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	//カリングなし
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	//塗りつぶし
	gpipeline.RasterizerState.DepthClipEnable = true;	//深度クリッピング有効

	//ブレンドステートの設定
	gpipeline.BlendState.AlphaToCoverageEnable = false;
	gpipeline.BlendState.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	renderTargetBlendDesc.BlendEnable = false;
	renderTargetBlendDesc.LogicOpEnable = false;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	gpipeline.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	//入力レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;		//レイアウト戦闘アドレス
	gpipeline.InputLayout.NumElements = _countof(inputLayout);	//レイアウト配列の要素数

	//三角形で構成
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;	//レンダーターゲット数
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;	//0~1に正規化されたRGBA

	gpipeline.SampleDesc.Count = 1;	//サンプリングは1ピクセルにつき1
	gpipeline.SampleDesc.Quality = 0;	//クオリティは最低

	D3D12_DESCRIPTOR_RANGE descTblRange = {};

	descTblRange.NumDescriptors = 1;	//テクスチャ1つ
	descTblRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	//種別はテクスチャ
	descTblRange.BaseShaderRegister = 0;	//0番スロットから
	descTblRange.OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;	//連続したディスクリプタレンジの直後に来るよう設定

	//サンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};

	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//横方向の繰り返し
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//縦方向の繰り返し
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//奥行きの繰り返し
	samplerDesc.BorderColor = 
		D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	//ボーダーは黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	//線形補完
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;	//ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;	//ミップマップ最小値
	samplerDesc.ShaderVisibility = 
		D3D12_SHADER_VISIBILITY_PIXEL;	//ピクセルシェーダーから見える
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;	//リサンプリングしない


	//ルートパラメーターの定義
	D3D12_ROOT_PARAMETER rootparam = {};

	rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

	//ピクセルシェーダーから見える
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//ディスクリプタレンジのアドレス
	rootparam.DescriptorTable.pDescriptorRanges = &descTblRange;

	//ディスクリプタレンジ数
	rootparam.DescriptorTable.NumDescriptorRanges = 1;

	//ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = &rootparam;	//ルートパラメーターの先頭アドレス
	rootSignatureDesc.NumParameters = 1;		//ルートパラメーター数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;	//サンプラーの設定
	rootSignatureDesc.NumStaticSamplers = 1;	//サンプラー数

	//ルートシグネチャバイナリコード作成
	ID3DBlob* rootSigBlob = nullptr;

	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob);

	CheckNullptr(result);

	//ルートシグネチャの生成
	ID3D12RootSignature* rootsignature = nullptr;
	result = _dev->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootsignature));

	CheckNullptr(result);

	//パイプラインステートにルートシグネチャを設定
	gpipeline.pRootSignature = rootsignature;

	//パイプラインステートの生成
	ID3D12PipelineState* _pipelineState = nullptr;

	result = _dev->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(&_pipelineState));

	CheckNullptr(result);

	//ビューポートの設定
	D3D12_VIEWPORT viewport = {};

	viewport.Width = window_width;
	viewport.Height = window_height;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//シザー矩形の設定
	D3D12_RECT scissor = {};

	scissor.left = 0;
	scissor.top = 0;
	scissor.right = scissor.left + window_width;
	scissor.bottom = scissor.top + window_height;

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			//アプリケーションが終わるときにmessageがWM_QUITになる
			if (msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//レンダーターゲットの設定
		auto bbIdx = _swapchain->GetCurrentBackBufferIndex();

		//リソースバリアの設定
		D3D12_RESOURCE_BARRIER BarrierDesc = {};

		BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	//遷移
		BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;	//特に指定なし
		BarrierDesc.Transition.pResource = _backBuffers[bbIdx];	//バックバッファ
		BarrierDesc.Transition.Subresource = 0;

		//パイプラインステートの設定
		_cmdList->SetPipelineState(_pipelineState);

		//レンダーターゲットビューをセット
		auto rtvH = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		_cmdList->OMSetRenderTargets(1, &rtvH, false, nullptr);

		//クリアカラー
		float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };

		//コマンドリストのクリア
		_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		//ビューポートとシザー矩形の設定
		_cmdList->RSSetViewports(1, &viewport);
		_cmdList->RSSetScissorRects(1, &scissor);
		//ルートシグネチャの設定
		_cmdList->SetGraphicsRootSignature(rootsignature);

		//ディスクリプタヒープの設定
		_cmdList->SetDescriptorHeaps(1, &_texDescHeap);

		//プリミティブとトポロジの設定
		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//頂点バッファの設定
		_cmdList->IASetVertexBuffers(0, 1, &vbView);

		//インデックスバッファの設定
		_cmdList->IASetIndexBuffer(&ibView);

		//描画命令を設定
		//_cmdList->DrawInstanced(4, 1, 0, 0);
		_cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);

		BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	//レンダーターゲット状態
		BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			//プレゼント状態

		//リソースバリアの設定
		_cmdList->ResourceBarrier(1, &BarrierDesc);

		//コマンドリストのクローズ
		result = _cmdList->Close();

		//コマンドリストの実行
		ID3D12CommandList* cmdLists[] = { _cmdList };
		_cmdQueue->ExecuteCommandLists(1, cmdLists);

		_cmdQueue->Signal(_fence, ++_fenceVal);

		//フェンスの待機
		if (_fence->GetCompletedValue() < _fenceVal)
		{
			//イベントハンドルの取得
			auto event = CreateEvent(nullptr, false, false, nullptr);

			_fence->SetEventOnCompletion(_fenceVal, event);
			//イベントが発生するまで待機
			WaitForSingleObject(event, INFINITE);
			//イベントハンドルを閉じる
			CloseHandle(event);
		}

		//コマンドアロケータークリア
		result = _cmdAllocator->Reset();
		CheckNullptr(result);

		//コマンドリストリセット
		result = _cmdList->Reset(_cmdAllocator, _pipelineState);
		CheckNullptr(result);

		//フリップ
		result = _swapchain->Present(1, 0);
		CheckNullptr(result);
	}

	//ウィンドウクラスの登録を解除
	UnregisterClass(w.lpszClassName, w.hInstance);

	return 0;
}
