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

//�E�B���h�E�v���V�\�W��
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);	//�I�����b�Z�[�W�𑗂�
		break;
	default:
		return DefWindowProc(hWnd, msg, wparam, lparam);
	}
	return 0;
}

//�t�B�[�`���[���x���ݒ�p�֐�
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

//�O���t�B�b�N�{�[�h�̐ݒ�p�֐�
IDXGIAdapter* GetAdapter(IDXGIFactory6* dxgiFactory)
{
	//�A�_�v�^�[�̗񋓗p
	std::vector<IDXGIAdapter*> adapters;

	//�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	IDXGIAdapter* tmpAdapter = nullptr;

	for (int i = 0; dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc;
		adpt->GetDesc(&adesc);	// �A�_�v�^�[�̐����I�u�W�F�N�g�擾

		std::wstring strDesc = adesc.Description;

#ifdef _DEBUG
		//�A�_�v�^�[�̖��O���f�o�b�O�o��
		DebugOutputFormatString("Adapter : %s\n", std::string(strDesc.begin(), strDesc.end()).c_str());
#endif

		//�A�_�v�^�[�̖��O��NVIDIA�Ŏn�܂���̂�I��
		if (strDesc.find(L"NVIDIA") != std::string::npos)
		{
			tmpAdapter = adpt;
			break;
		}
	}

	return tmpAdapter;
}

//�f�o�b�O���C���[�̐ݒ�p�֐�
void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

	debugLayer->EnableDebugLayer();	//�f�o�b�O���C���[��L����
	debugLayer->Release();			//�L����������C���^�[�t�F�C�X���J��
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

	//�E�B���h�E�N���X�̐����A�o�^
	WNDCLASSEX w = {};

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;	//�R�[���o�b�N�֐��̎w��
    w.lpszClassName = L"DX12Sample";			//�N���X��
	w.hInstance = GetModuleHandle(nullptr);		//�n���h���̎擾

	RegisterClassEx(&w); //�A�v���P�[�V�����N���X(�E�B���h�E�N���X�̎w���OS�ɓ`����)

	RECT wrc = { 0, 0, window_width, window_height };	//�E�B���h�E�T�C�Y�����߂�

	//�֐����g���ăE�B���h�E�T�C�Y��␳
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//�E�B���h�E�I�u�W�F�N�g�̐���
	HWND hWnd = CreateWindow(
		w.lpszClassName,	//�N���X��
		L"DX12�e�X�g",		//�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,	//�E�B���h�E�̎��
		CW_USEDEFAULT,			//�\������ʒuX(OS�ɂ��C��)
		CW_USEDEFAULT,			//�\������ʒuY(OS�ɂ��C��)
		wrc.right - wrc.left,	//�E�B���h�E�T�C�Y��
		wrc.bottom - wrc.top,	//�E�B���h�E�T�C�Y����
		nullptr,			//�e�E�B���h�E�̃n���h��
		nullptr,			//���j���[�n���h��
		w.hInstance,		//�Ăяo���A�v���P�[�V�����n���h��
		nullptr);			//�ǉ��p�����[�^

	MSG msg = {};

#ifdef _DEBUG
	//�f�o�b�O���C���[���I����
	EnableDebugLayer();
#endif

	//�t�B�[�`���[���x���̐ݒ�
	D3D_FEATURE_LEVEL featureLevel = GetSupportedFeatureLevel();

	//�O���t�B�b�N�{�[�h�̐ݒ�
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

	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;		//�^�C���A�E�g�Ȃ�
	cmdQueueDesc.NodeMask = 0;								//�A�_�v�^�[��������g��Ȃ��Ƃ���0�ł悢
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;//�v���C�I���e�B�͓��Ɏw��Ȃ�
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;	//�R�}���h���X�g�ƍ��킹��
	//�L���[����
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));

	CheckNullptr(result);

	//�X���b�v�`�F�C���̐���
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};

	swapchainDesc.Width = window_width;	//�E�B���h�E��
	swapchainDesc.Height = window_height;	//�E�B���h�E����
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//�t�H�[�}�b�g
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;	//�}���`�T���v�����O�ݒ�
	swapchainDesc.SampleDesc.Quality = 0;	//�}���`�T���v�����O�ݒ�
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;	//�o�b�t�@�̎g�p�@
	swapchainDesc.BufferCount = 2;	//�o�b�t�@��
	
	//�o�b�N�o�b�t�@�[�͐L�яk�݉\
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;	//�X�P�[�����O�ݒ�
	
	//�t���b�v��͑��₩�ɔj��
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	//�X���b�v�G�t�F�N�g
	
	//���Ɏw��Ȃ�
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;	//�A���t�@���[�h�ݒ�

	//�E�B���h�E����t���X�N���[���؂�ւ��\(�t����)
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	//�t���O�ݒ�

	result = _dxgiFactory->CreateSwapChainForHwnd(
		_cmdQueue, hWnd,
		&swapchainDesc, nullptr, nullptr, 
		(IDXGISwapChain1**)&_swapchain);	//�{����QueryInterface�Ȃǂ�p����
											//IDXGISwapChain4*�ւ̕ϊ��`�F�b�N�����邪�A
											//����͕�����₷���̂��߃L���X�g�őΉ�

	CheckNullptr(result);
											

	//�f�B�X�N���v�^�q�[�v�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	//�����_�[�^�[�Q�b�g�r���[�Ȃ̂�RTV
	heapDesc.NodeMask = 0;	//�A�_�v�^�[����̏ꍇ��0
	heapDesc.NumDescriptors = 2;	//���\�̓��
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	//���Ɏw��Ȃ�

	ID3D12DescriptorHeap* _rtvHeaps = nullptr;

	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_rtvHeaps));

	CheckNullptr(result);

	//�����_�[�^�[�Q�b�g�r���[�̍쐬
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = _swapchain->GetDesc(&swcDesc);

	CheckNullptr(result);

	//�X���b�v�`�F�[���̐������o�b�N�o�b�t�@�𐶐�����
	std::vector<ID3D12Resource*> _backBuffers(swcDesc.BufferCount);

	//�f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾
	D3D12_CPU_DESCRIPTOR_HANDLE handle
		= _rtvHeaps->GetCPUDescriptorHandleForHeapStart();

	for (int idx = 0; idx < swcDesc.BufferCount; idx++)
	{
		//�o�b�N�o�b�t�@�ɃX���b�v�`�F�[���̃o�b�t�@��ݒ�
		result = _swapchain->GetBuffer(idx, IID_PPV_ARGS(&_backBuffers[idx]));
		CheckNullptr(result);

		//�����_�[�^�[�Q�b�g�r���[�𐶐�
		_dev->CreateRenderTargetView(
			_backBuffers[idx],
			nullptr,
			handle);

		handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	//�t�F���X�̐���
	ID3D12Fence* _fence = nullptr;
	UINT64 _fenceVal = 0;

	result = _dev->CreateFence(
		_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));

	//�E�B���h�E�\��
	ShowWindow(hWnd, SW_SHOW);


	//���_���W�̒�`
	XMFLOAT3 vertices[] =
	{
		XMFLOAT3(-0.4f, -0.7f, 0.0f),	//����
		XMFLOAT3(-0.4f,  0.7f, 0.0f),	//����
		XMFLOAT3(0.4f, -0.7f, 0.0f),	//�E��
		XMFLOAT3(0.4f,  0.7f, 0.0f),	//�E��
	};

	//���_�o�b�t�@�̐���
	D3D12_HEAP_PROPERTIES heapProp = {};

	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//�A�b�v���[�h�q�[�v
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	//CPU�y�[�W�v���p�e�B
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	//�������v�[���v���t�@�����X

	D3D12_RESOURCE_DESC resdesc = {};

	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	//�o�b�t�@
	resdesc.Width = sizeof(vertices);	//�o�b�t�@�T�C�Y
	resdesc.Height = 1;	//����
	resdesc.DepthOrArraySize = 1;	//�[��
	resdesc.MipLevels = 1;	//�~�b�v�}�b�v���x��
	resdesc.Format = DXGI_FORMAT_UNKNOWN;	//�t�H�[�}�b�g
	resdesc.SampleDesc.Count = 1;	//�T���v����
	resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;	//�t���O
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	//���C�A�E�g

	ID3D12Resource* vertBuff = nullptr;

	result = _dev->CreateCommittedResource(
		&heapProp, D3D12_HEAP_FLAG_NONE, &resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff));

	CheckNullptr(result);

	//���_�o�b�t�@�̃������R�s�[
	XMFLOAT3* vertMap = nullptr;

	result = vertBuff->Map(0, nullptr, (void**)&vertMap);

	std::copy(std::begin(vertices), std::end(vertices), vertMap);

	vertBuff->Unmap(0, nullptr);

	//���_�o�b�t�@�[�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();	//�o�b�t�@�̉��z�A�h���X
	vbView.SizeInBytes = sizeof(vertices);			//�S�o�C�g��
	vbView.StrideInBytes = sizeof(vertices[0]);		//1���_������̃o�C�g��


	unsigned short indices[] = { 0,1,2, 2,1,3 };

	ID3D12Resource* idxBuff = nullptr;
	//�ݒ�́A�o�b�t�@�̃T�C�Y�ȊO���_�o�b�t�@�̐ݒ���g���܂킵��
	//OK���Ǝv���܂��B
	resdesc.Width = sizeof(indices);
	result = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&idxBuff));

	//������o�b�t�@�ɃC���f�b�N�X�f�[�^���R�s�[
	unsigned short* mappedIdx = nullptr;
	idxBuff->Map(0, nullptr, (void**)&mappedIdx);
	std::copy(std::begin(indices), std::end(indices), mappedIdx);
	idxBuff->Unmap(0, nullptr);

	//�C���f�b�N�X�o�b�t�@�r���[���쐬
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	ibView.BufferLocation = idxBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indices);


	//�V�F�[�_�[�I�u�W�F�N�g�̐���
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"BasicVertexShader.hlsl",
		nullptr,//define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�̓f�t�H���g
		"BasicVS",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p����эœK���Ȃ�
		0,
		&vsBlob,
		&errorBlob);

	CheckNullptr(result);

	result = D3DCompileFromFile(
		L"BasicPixelShader.hlsl",
		nullptr,//define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�̓f�t�H���g
		"BasicPS",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p����эœK���Ȃ�
		0,
		&psBlob,
		&errorBlob);

	//�f�o�b�O�E�B���h�E�\��
#ifdef _DEBUG
	if (FAILED(result))
	{
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			OutputDebugStringA("�t�@�C����������܂���");
			return 0;
		}
		else
		{
			std::string errstr;	//�󂯎��pstirng
			errstr.resize(errorBlob->GetBufferSize());//�K�v�ȃT�C�Y���m��

			//�f�[�^�R�s�[
			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());

			OutputDebugStringA(errstr.c_str());
		}
	}
#endif

	//���_���C�A�E�g�̍쐬
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}
	};

	//�O���t�B�b�N�X�p�C�v���C���X�e�[�g�̐���
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};

	gpipeline.pRootSignature = nullptr;	//���[�g�V�O�l�`��

	gpipeline.VS.pShaderBytecode = vsBlob->GetBufferPointer();	//���_�V�F�[�_�[�o�C�g�R�[�h
	gpipeline.VS.BytecodeLength = vsBlob->GetBufferSize();		//���_�V�F�[�_�[�o�C�g�R�[�h�T�C�Y
	gpipeline.PS.pShaderBytecode = psBlob->GetBufferPointer();	//�s�N�Z���V�F�[�_�[�o�C�g�R�[�h
	gpipeline.PS.BytecodeLength = psBlob->GetBufferSize();		//�s�N�Z���V�F�[�_�[�o�C�g�R�[�h�T�C�Y

	//�f�t�H���g�̃T���v���}�X�N��\���萔
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//�܂��A���`�G�C���A�X�͎g��Ȃ�����false
	gpipeline.RasterizerState.AntialiasedLineEnable = false;

	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	//�J�����O�Ȃ�
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	//�h��Ԃ�
	gpipeline.RasterizerState.DepthClipEnable = true;	//�[�x�N���b�s���O�L��

	//�u�����h�X�e�[�g�̐ݒ�
	gpipeline.BlendState.AlphaToCoverageEnable = false;
	gpipeline.BlendState.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	renderTargetBlendDesc.BlendEnable = false;
	renderTargetBlendDesc.LogicOpEnable = false;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	gpipeline.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	//���̓��C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;		//���C�A�E�g�퓬�A�h���X
	gpipeline.InputLayout.NumElements = _countof(inputLayout);	//���C�A�E�g�z��̗v�f��

	//�O�p�`�ō\��
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;	//�����_�[�^�[�Q�b�g��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;	//0~1�ɐ��K�����ꂽRGBA

	gpipeline.SampleDesc.Count = 1;	//�T���v�����O��1�s�N�Z���ɂ�1
	gpipeline.SampleDesc.Quality = 0;	//�N�I���e�B�͍Œ�

	//���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//���[�g�V�O�l�`���o�C�i���R�[�h�쐬
	ID3DBlob* rootSigBlob = nullptr;

	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob);

	CheckNullptr(result);

	//���[�g�V�O�l�`���̐���
	ID3D12RootSignature* rootsignature = nullptr;
	result = _dev->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootsignature));

	CheckNullptr(result);

	//�p�C�v���C���X�e�[�g�Ƀ��[�g�V�O�l�`����ݒ�
	gpipeline.pRootSignature = rootsignature;

	//�p�C�v���C���X�e�[�g�̐���
	ID3D12PipelineState* _pipelineState = nullptr;

	result = _dev->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(&_pipelineState));

	CheckNullptr(result);

	//�r���[�|�[�g�̐ݒ�
	D3D12_VIEWPORT viewport = {};

	viewport.Width = window_width;
	viewport.Height = window_height;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//�V�U�[��`�̐ݒ�
	D3D12_RECT scissor = {};

	scissor.left = 0;
	scissor.top = 0;
	scissor.right = scissor.left + window_width;
	scissor.bottom = scissor.top + window_height;

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			//�A�v���P�[�V�������I���Ƃ���message��WM_QUIT�ɂȂ�
			if (msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//�����_�[�^�[�Q�b�g�̐ݒ�
		auto bbIdx = _swapchain->GetCurrentBackBufferIndex();

		//���\�[�X�o���A�̐ݒ�
		D3D12_RESOURCE_BARRIER BarrierDesc = {};

		BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	//�J��
		BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;	//���Ɏw��Ȃ�
		BarrierDesc.Transition.pResource = _backBuffers[bbIdx];	//�o�b�N�o�b�t�@
		BarrierDesc.Transition.Subresource = 0;

		//�p�C�v���C���X�e�[�g�̐ݒ�
		_cmdList->SetPipelineState(_pipelineState);

		//�����_�[�^�[�Q�b�g�r���[���Z�b�g
		auto rtvH = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		_cmdList->OMSetRenderTargets(1, &rtvH, false, nullptr);

		//�N���A�J���[
		float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };

		//�R�}���h���X�g�̃N���A
		_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		//�r���[�|�[�g�ƃV�U�[��`�̐ݒ�
		_cmdList->RSSetViewports(1, &viewport);
		_cmdList->RSSetScissorRects(1, &scissor);
		//���[�g�V�O�l�`���̐ݒ�
		_cmdList->SetGraphicsRootSignature(rootsignature);

		//�v���~�e�B�u�ƃg�|���W�̐ݒ�
		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//���_�o�b�t�@�̐ݒ�
		_cmdList->IASetVertexBuffers(0, 1, &vbView);

		//�C���f�b�N�X�o�b�t�@�̐ݒ�
		_cmdList->IASetIndexBuffer(&ibView);

		//�`�施�߂�ݒ�
		//_cmdList->DrawInstanced(4, 1, 0, 0);
		_cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);

		BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	//�����_�[�^�[�Q�b�g���
		BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			//�v���[���g���

		//���\�[�X�o���A�̐ݒ�
		_cmdList->ResourceBarrier(1, &BarrierDesc);

		//�R�}���h���X�g�̃N���[�Y
		result = _cmdList->Close();

		//�R�}���h���X�g�̎��s
		ID3D12CommandList* cmdLists[] = { _cmdList };
		_cmdQueue->ExecuteCommandLists(1, cmdLists);

		_cmdQueue->Signal(_fence, ++_fenceVal);

		//�t�F���X�̑ҋ@
		if (_fence->GetCompletedValue() < _fenceVal)
		{
			//�C�x���g�n���h���̎擾
			auto event = CreateEvent(nullptr, false, false, nullptr);

			_fence->SetEventOnCompletion(_fenceVal, event);
			//�C�x���g����������܂őҋ@
			WaitForSingleObject(event, INFINITE);
			//�C�x���g�n���h�������
			CloseHandle(event);
		}

		//�R�}���h�A���P�[�^�[�N���A
		result = _cmdAllocator->Reset();
		CheckNullptr(result);

		//�R�}���h���X�g���Z�b�g
		result = _cmdList->Reset(_cmdAllocator, _pipelineState);
		CheckNullptr(result);

		//�t���b�v
		result = _swapchain->Present(1, 0);
		CheckNullptr(result);
	}

	//�E�B���h�E�N���X�̓o�^������
	UnregisterClass(w.lpszClassName, w.hInstance);

	return 0;
}
