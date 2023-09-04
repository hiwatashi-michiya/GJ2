#include "Novice.h"
#include "DebugText.h"
#include "GameScene.h"
#include "ImGuiManager.h"
#include "Matrix4x4.h"
#include "TextureManager.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "WinApp.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <span>

#include <d3dcompiler.h>
#include <d3dx12.h>

#pragma comment(lib, "d3dcompiler.lib")

namespace {

std::wstring GetResourceRoot() {
	static const std::wstring kResourceRoot = L"./NoviceResources/";
	return kResourceRoot;
}

std::string GetResourceRootChar() {
	static const std::string kResourceRootChar = "./NoviceResources/";
	return kResourceRootChar;
}

const WORD kXInputButtons[] = {
    XINPUT_GAMEPAD_DPAD_UP,
    XINPUT_GAMEPAD_DPAD_DOWN,
    XINPUT_GAMEPAD_DPAD_LEFT,
    XINPUT_GAMEPAD_DPAD_RIGHT,
    XINPUT_GAMEPAD_START,
    XINPUT_GAMEPAD_BACK,
    XINPUT_GAMEPAD_LEFT_THUMB,
    XINPUT_GAMEPAD_RIGHT_THUMB,
    XINPUT_GAMEPAD_LEFT_SHOULDER,
    XINPUT_GAMEPAD_RIGHT_SHOULDER,
    XINPUT_GAMEPAD_A,
    XINPUT_GAMEPAD_B,
    XINPUT_GAMEPAD_X,
    XINPUT_GAMEPAD_Y,
};

bool IsPressXInputButton(const XINPUT_STATE& state, PadButton button) {
	// トリガーL2/R2特殊処理
	if (button == kPadButton10 && state.Gamepad.bLeftTrigger > 0x80) {
		return true;
	}
	if (button == kPadButton11 && state.Gamepad.bRightTrigger > 0x80) {
		return true;
	}
	// 普通のボタン
	if ((button < _countof(kXInputButtons)) && (state.Gamepad.wButtons & kXInputButtons[button])) {
		return true;
	}
	return false;
}

Sprite::BlendMode ToSpriteBlendMode(BlendMode blendMode) {
	Sprite::BlendMode result = Sprite::BlendMode::kNone;
	switch (blendMode) {
	case kBlendModeNone:
		result = Sprite::BlendMode::kNone;
		break;
	case kBlendModeNormal:
		result = Sprite::BlendMode::kNormal;
		break;
	case kBlendModeAdd:
		result = Sprite::BlendMode::kAdd;
		break;
	case kBlendModeSubtract:
		result = Sprite::BlendMode::kSubtract;
		break;
	case kBlendModeMultily:
		result = Sprite::BlendMode::kMultily;
		break;
	case kBlendModeScreen:
		result = Sprite::BlendMode::kScreen;
		break;
	default:
		assert(false);
		break;
	}
	return result;
}

std::wstring ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(
	    CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(
	    CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()),
	    &result[0], sizeNeeded);
	return result;
}

std::string ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(
	    CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(
	    CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL,
	    NULL);
	return result;
}

///
/// 入門用システム
///
class NoviceSystem {
	friend class Novice;

public:
	NoviceSystem() = default;
	~NoviceSystem() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// リセット
	/// </summary>
	void Reset();

private:
	// ボックスの最大数
	static const int32_t kMaxBoxCount = 4096;
	// ボックスの頂点数
	static const UINT kVertexCountBox = 4;
	// ボックスのインデックス数
	static const UINT kIndexCountBox = 6;
	// 三角形の最大数
	static const int32_t kMaxTriangleCount = 32768;
	// 三角形の頂点数
	static const UINT kVertexCountTriangle = 3;
	// 三角形のインデックス数
	static const UINT kIndexCountTriangle = 0;
	// 線分の最大数
	static const int32_t kMaxLineCount = 4096;
	// 線分の頂点数
	static const UINT kVertexCountLine = 2;
	// 線分のインデックス数
	static const UINT kIndexCountLine = 0;
	// 四角形の最大数
	static const int32_t kMaxQuadCount = 4096;
	// 四角形の頂点数
	static const UINT kVertexCountQuad = 4;
	// 四角形のインデックス数
	static const UINT kIndexCountQuad = 4;
	// スプライトの最大数
	static const int32_t kMaxSpriteCount = 512;
	// 書式付き文字列展開用バッファサイズ
	static const int32_t textBufferSize = 256;
	// 頂点データ構造体
	struct VertexPosColor {
		Vector3 pos;   // xyz座標
		Vector4 color; // RGBA
	};

	// 定数バッファ用データ構造体
	struct ConstBufferData {
		Matrix4x4 mat; // 3D変換行列
	};

	// パイプラインセット
	struct PipelineSet {
		// ルートシグネチャ
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		// パイプラインステートオブジェクト
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	};

	// メッシュ
	struct Mesh {
		// 頂点バッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;
		// インデックスバッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff;
		// 頂点バッファビュー
		D3D12_VERTEX_BUFFER_VIEW vbView{};
		// インデックスバッファビュー
		D3D12_INDEX_BUFFER_VIEW ibView{};
		// 頂点バッファマップ
		VertexPosColor* vertMap = nullptr;
		// インデックスバッファマップ
		uint16_t* indexMap = nullptr;
	};
	struct MeshForQuad {
		// 頂点バッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;
		// インデックスバッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff;
		// 頂点バッファビュー
		D3D12_VERTEX_BUFFER_VIEW vbView{};
		// インデックスバッファビュー
		D3D12_INDEX_BUFFER_VIEW ibView{};
		// 頂点バッファマップ
		Sprite::VertexPosUv* vertMap = nullptr;
		// インデックスバッファマップ
		uint16_t* indexMap = nullptr;
	};

	// Quad用
	struct MappedResource {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		Sprite::ConstBufferData* address;
	};

	NoviceSystem(const NoviceSystem&) = delete;
	NoviceSystem& operator=(const NoviceSystem&) = delete;

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	void CreateGraphicsPipelines();

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	std::unique_ptr<PipelineSet>
	    CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType, BlendMode blendMode);

	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// 各種メッシュ生成
	/// </summary>
	void CreateMeshes();

	/// <summary>
	/// メッシュ生成
	/// </summary>
	/// <param name="vertexCount">頂点数</param>
	/// <param name="indexCount">インデックス数</param>
	/// <returns></returns>
	std::unique_ptr<Mesh> CreateMesh(UINT vertexCount, UINT indexCount);

	/// <summary>
	/// Quad用メッシュ生成
	/// </summary>
	/// <param name="vertexCount">頂点数</param>
	/// <param name="indexCount">インデックス数</param>
	/// <returns></returns>
	std::unique_ptr<MeshForQuad> CreateMeshForQuad(UINT vertexCount, UINT indexCount);

	/// <summary>
	/// スプライト生成
	/// </summary>
	void CreateSprites();

	/// <summary>
	/// リソース生成
	/// </summary>
	/// <param name="size">サイズ</param>
	/// <returns>生成したリソース</returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateCommittedResource(UINT64 size);

	/// <summary>
	/// 色変換
	/// </summary>
	/// <param name="color">整数による色指定</param>
	/// <note>colorはsRGB空間の色が指定されたと仮定して、戻り値はLinearへの変換を行います</note>
	/// <returns>float変換後の色</returns>
	static Vector4 FloatColor(unsigned int color);

	void DrawBox(int x, int y, int w, int h, float angle, unsigned int color);
	void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned int color);
	void DrawTriangles(std::span<VertexPosColor> trianglePoints);
	void DrawLine(int x1, int y1, int x2, int y2, unsigned int color);
	void DrawLines(std::span<VertexPosColor> linePoints);
	void DrawSpriteRect(
	    int destX, int destY, int srcX, int srcY, int srcW, int srcH, int textureHandle,
	    float scaleX, float scaleY, float angle, unsigned int color);
	void DrawQuad(
	    int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int srcX, int srcY,
	    int srcW, int srcH, int textureHandle, unsigned int color);
	int CheckHitKey(int keyCode);
	void GetHitKeyStateAll(char* keyStateBuf);
	bool IsPressMouse(int buttonNumber);
	bool IsTriggerMouse(int buttonNumber);
	const Vector2& GetMousePosition();
	int GetWheel();
	void SetBlendMode(BlendMode blendMode);
	bool GetJoystickState(int stickNo, DIJOYSTATE2& out);
	bool GetJoystickStatePrevious(int stickNo, DIJOYSTATE2& out);
	bool GetJoystickState(int stickNo, XINPUT_STATE& out);
	bool GetJoystickStatePrevious(int stickNo, XINPUT_STATE& out);
	void SetJoystickDeadZone(int stickNo, int deadZoneL, int deadZoneR);
	int GetNumberOfJoysticks();
	void ScreenPrintf(int x, int y, const char* text, va_list& args);
	void ConsolePrintf(const char* text, va_list& args);
	int LoadAudio(const char* fileName);
	int PlayAudio(int soundHandle, int loopFlag, float volume);
	void StopAudio(int playHandle);
	void PauseAudio(int playHandle);
	void ResumeAudio(int playHandle);
	void SetAudioVolume(int playHandle, float volume);
	bool IsPlayingAudio(int playHandle);
	void SetFullscreen(bool fullscreen);
	void AllowWindowSizeChanged(bool allow);
	int ProcessMessage();
	void BeginFrame();
	void EndFrame();
	Matrix4x4 Matrix4Orthographic(
	    float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ);
	HWND GetWindowHandle();
	// 入力
	Input* input_ = nullptr;
	// デバッグテキスト
	DebugText* debugText_ = nullptr;
	// オーディオ
	Audio* audio_ = nullptr;
	// WindowsAPI
	WinApp* winApp_ = nullptr;
	// DirectX
	DirectXCommon* dxCommon_ = nullptr;
	// ImGui
	ImGuiManager* imGuiManager_ = nullptr;
	// パイプラインセット
	std::array<std::unique_ptr<PipelineSet>, kCountOfBlendMode> pipelineSetTriangles_;
	std::array<std::unique_ptr<PipelineSet>, kCountOfBlendMode> pipelineSetLines_;
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;
	// ボックス
	std::unique_ptr<Mesh> box_;
	// 三角形
	std::unique_ptr<Mesh> triangle_;
	// 線分
	std::unique_ptr<Mesh> line_;
	// 四角形
	std::unique_ptr<MeshForQuad> quad_;
	std::array<MappedResource, kMaxQuadCount> constBufferForQuads_;
	// スプライト
	std::array<std::unique_ptr<Sprite>, kMaxSpriteCount> sprites_;
	// 文字列バッファ
	std::array<char, textBufferSize> textBuffer{0};
	//  ボックスの使用インデックス
	uint32_t indexBox_ = 0;
	// 三角形の使用インデックス
	uint32_t indexTriangle_ = 0;
	// 線分の使用インデックス
	uint32_t indexLine_ = 0;
	// 四角形の使用インデックス
	uint32_t indexQuad_ = 0;
	// スプライトの使用インデックス
	uint32_t indexSprite_ = 0;
	// ブレンドモード
	BlendMode blendMode_ = kBlendModeNormal;
};

void NoviceSystem::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();
	audio_ = Audio::GetInstance();
	winApp_ = WinApp::GetInstance();
	imGuiManager_ = ImGuiManager::GetInstance();

	// 定数バッファ生成
	CreateConstBuffer();
	// パイプライン生成
	CreateGraphicsPipelines();
	// メッシュ生成
	CreateMeshes();
	// スプライト生成
	CreateSprites();
}

void NoviceSystem::Reset() {
	indexBox_ = 0;
	indexTriangle_ = 0;
	indexLine_ = 0;
	indexQuad_ = 0;
	indexSprite_ = 0;
}

void NoviceSystem::CreateGraphicsPipelines() {
	pipelineSetTriangles_[kBlendModeNone] =
	    CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, kBlendModeNone);
	pipelineSetTriangles_[kBlendModeNormal] =
	    CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, kBlendModeNormal);
	pipelineSetTriangles_[kBlendModeAdd] =
	    CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, kBlendModeAdd);
	pipelineSetTriangles_[kBlendModeSubtract] =
	    CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, kBlendModeSubtract);
	pipelineSetTriangles_[kBlendModeMultily] =
	    CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, kBlendModeMultily);
	pipelineSetTriangles_[kBlendModeScreen] =
	    CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, kBlendModeScreen);

	pipelineSetLines_[kBlendModeNone] =
	    CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, kBlendModeNone);
	pipelineSetLines_[kBlendModeNormal] =
	    CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, kBlendModeNormal);
	pipelineSetLines_[kBlendModeAdd] =
	    CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, kBlendModeAdd);
	pipelineSetLines_[kBlendModeSubtract] =
	    CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, kBlendModeSubtract);
	pipelineSetLines_[kBlendModeMultily] =
	    CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, kBlendModeMultily);
	pipelineSetLines_[kBlendModeScreen] =
	    CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, kBlendModeScreen);
}

std::unique_ptr<NoviceSystem::PipelineSet> NoviceSystem::CreateGraphicsPipeline(
    D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType, BlendMode blendMode) {

	std::unique_ptr<PipelineSet> pipelineSet = std::make_unique<PipelineSet>();

	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;    // 頂点シェーダオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;    // ピクセルシェーダオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト
	HRESULT result;

	// 頂点シェーダの読み込みとコンパイル
	std::wstring vsFile = GetResourceRoot() + L"shaders/ShapeVS.hlsl";
	result = D3DCompileFromFile(
	    vsFile.c_str(), // シェーダファイル名
	    nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", 0, 0, &vsBlob, &errorBlob);
	assert(SUCCEEDED(result));

	// ピクセルシェーダの読み込みとコンパイル
	std::wstring psFile = GetResourceRoot() + L"shaders/ShapePS.hlsl";
	result = D3DCompileFromFile(
	    psFile.c_str(), // シェーダファイル名
	    nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", 0, 0, &psBlob, &errorBlob);
	assert(SUCCEEDED(result));

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT,
	     D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	    {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	     D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	// カリングしない
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//  デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	// 常に上書き
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA全てのチャンネルを描画
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	switch (blendMode) {
	case kBlendModeNone:
		blenddesc.BlendEnable = false;
		break;
	case kBlendModeNormal:
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		break;
	case kBlendModeAdd:
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
		break;
	case kBlendModeSubtract:
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
		break;
	case kBlendModeMultily:
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_ZERO;
		blenddesc.DestBlend = D3D12_BLEND_SRC_COLOR;
		break;
	case kBlendModeScreen:
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
		break;
	default:
		break;
	}

	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定
	gpipeline.PrimitiveTopologyType = topologyType;

	gpipeline.NumRenderTargets = 1;                            // 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

	// ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[1] = {};
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(
	    _countof(rootparams), rootparams, 1, &samplerDesc,
	    D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(
	    &rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ルートシグネチャの生成
	result = dxCommon_->GetDevice()->CreateRootSignature(
	    0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
	    IID_PPV_ARGS(&pipelineSet->rootSignature));
	assert(SUCCEEDED(result));

	gpipeline.pRootSignature = pipelineSet->rootSignature.Get();

	// グラフィックスパイプラインの生成
	result = dxCommon_->GetDevice()->CreateGraphicsPipelineState(
	    &gpipeline, IID_PPV_ARGS(&pipelineSet->pipelineState));
	assert(SUCCEEDED(result));

	return pipelineSet;
}

void NoviceSystem::CreateConstBuffer() {
	HRESULT result;

	// 定数バッファのサイズ
	UINT sizeCB = (sizeof(ConstBufferData) + 0xff) & ~0xff;
	// 定数バッファバッファ生成
	constBuffer_ = CreateCommittedResource(sizeCB);

	//平行投影による射影行列の生成
	Matrix4x4 mat = Matrix4Orthographic(
	    0, float(dxCommon_->GetBackBufferWidth()), float(dxCommon_->GetBackBufferHeight()), 0, 0,
	    1);

	// 定数バッファとのデータリンク
	ConstBufferData* constMap = nullptr;
	result = constBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&constMap));
	assert(SUCCEEDED(result));
	constMap->mat = mat;
	constBuffer_->Unmap(0, nullptr);

	// Quad用定数バッファ生成
	for (auto& cbuffer : constBufferForQuads_) {
		UINT sizeCBForQuad = (sizeof(Sprite::ConstBufferData) + 0xff) & ~0xff;
		cbuffer.resource = CreateCommittedResource(sizeCBForQuad);
		result = cbuffer.resource->Map(0, nullptr, reinterpret_cast<void**>(&cbuffer.address));
		assert(SUCCEEDED(result));
	}
}

void NoviceSystem::CreateMeshes() {

	// ボックスメッシュ生成
	UINT boxVertexCount = kMaxBoxCount * kVertexCountBox;
	UINT boxIndexCount = kMaxBoxCount * kIndexCountBox;

	box_ = CreateMesh(boxVertexCount, boxIndexCount);

	// 三角形メッシュ生成
	UINT triangleVertexCount = kMaxTriangleCount * kVertexCountTriangle;
	UINT triangleIndexCount = kMaxTriangleCount * kIndexCountTriangle;

	triangle_ = CreateMesh(triangleVertexCount, triangleIndexCount);

	// 線分メッシュ生成
	UINT lineVertexCount = kMaxLineCount * kVertexCountLine;
	UINT lineIndexCount = kMaxLineCount * kIndexCountLine;

	line_ = CreateMesh(lineVertexCount, lineIndexCount);

	// 四角形メッシュ生成
	UINT quadVertexCount = kMaxQuadCount * kVertexCountQuad;
	UINT quadIndexCount = kMaxQuadCount * kIndexCountQuad;

	quad_ = CreateMeshForQuad(quadVertexCount, quadIndexCount);
}

std::unique_ptr<NoviceSystem::Mesh> NoviceSystem::CreateMesh(UINT vertexCount, UINT indexCount) {

	HRESULT result;
	std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

	if (vertexCount > 0) {
		// 頂点データのサイズ
		UINT sizeVB = static_cast<UINT>(sizeof(VertexPosColor) * vertexCount);
		// 頂点バッファ生成
		mesh->vertBuff = CreateCommittedResource(sizeVB);

		// 頂点バッファビューの作成
		mesh->vbView.BufferLocation = mesh->vertBuff->GetGPUVirtualAddress();
		mesh->vbView.SizeInBytes = sizeVB;
		mesh->vbView.StrideInBytes = sizeof(VertexPosColor);

		// 頂点バッファのマッピング
		result = mesh->vertBuff->Map(0, nullptr, reinterpret_cast<void**>(&mesh->vertMap));
		assert(SUCCEEDED(result));
	}

	if (indexCount > 0) {
		// インデックスデータのサイズ
		UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indexCount);
		// インデックスバッファ生成
		mesh->indexBuff = CreateCommittedResource(sizeIB);

		// インデックスバッファビューの作成
		mesh->ibView.BufferLocation = mesh->indexBuff->GetGPUVirtualAddress();
		mesh->ibView.Format = DXGI_FORMAT_R16_UINT;
		mesh->ibView.SizeInBytes = sizeIB;

		// インデックスバッファのマッピング
		result = mesh->indexBuff->Map(0, nullptr, reinterpret_cast<void**>(&mesh->indexMap));
		assert(SUCCEEDED(result));
	}

	return mesh;
}

std::unique_ptr<NoviceSystem::MeshForQuad>
    NoviceSystem::CreateMeshForQuad(UINT vertexCount, UINT indexCount) {
	HRESULT result;
	std::unique_ptr<MeshForQuad> mesh = std::make_unique<MeshForQuad>();

	if (vertexCount > 0) {
		// 頂点データのサイズ
		UINT sizeVB = static_cast<UINT>(sizeof(Sprite::VertexPosUv) * vertexCount);
		// 頂点バッファ生成
		mesh->vertBuff = CreateCommittedResource(sizeVB);

		// 頂点バッファビューの作成
		mesh->vbView.BufferLocation = mesh->vertBuff->GetGPUVirtualAddress();
		mesh->vbView.SizeInBytes = sizeVB;
		mesh->vbView.StrideInBytes = sizeof(Sprite::VertexPosUv);

		// 頂点バッファのマッピング
		result = mesh->vertBuff->Map(0, nullptr, reinterpret_cast<void**>(&mesh->vertMap));
		assert(SUCCEEDED(result));
	}

	if (indexCount > 0) {
		// インデックスデータのサイズ
		UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indexCount);
		// インデックスバッファ生成
		mesh->indexBuff = CreateCommittedResource(sizeIB);

		// インデックスバッファビューの作成
		mesh->ibView.BufferLocation = mesh->indexBuff->GetGPUVirtualAddress();
		mesh->ibView.Format = DXGI_FORMAT_R16_UINT;
		mesh->ibView.SizeInBytes = sizeIB;

		// インデックスバッファのマッピング
		result = mesh->indexBuff->Map(0, nullptr, reinterpret_cast<void**>(&mesh->indexMap));
		assert(SUCCEEDED(result));
	}

	return mesh;
}

void NoviceSystem::CreateSprites() {
	for (auto& sprite : sprites_) {
		sprite = std::make_unique<Sprite>();
		sprite->Initialize();
		sprite->SetAnchorPoint({0.0f, 0.0f});
	}
}

Microsoft::WRL::ComPtr<ID3D12Resource> NoviceSystem::CreateCommittedResource(UINT64 size) {
	HRESULT result;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

	// リソース生成
	result = dxCommon_->GetDevice()->CreateCommittedResource(
	    &heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
	    IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(result));

	return resource;
}

Vector4 NoviceSystem::FloatColor(unsigned int color) {
	Vector4 colorf = {
	    ((color >> 24) & 0xff) / 255.0f, // R
	    ((color >> 16) & 0xff) / 255.0f, // G
	    ((color >> 8) & 0xff) / 255.0f,  // B
	    ((color >> 0) & 0xff) / 255.0f}; // A

	// srgb to linear
	static auto sRGBToLinear = [](float sRGBColor) {
		if (0.0f <= sRGBColor && sRGBColor <= 0.04045f) {
			return sRGBColor / 12.92f;
		}
		return std::pow(((sRGBColor + 0.055f) / 1.055f), 2.4f);
	};
	colorf.x = sRGBToLinear(colorf.x);
	colorf.y = sRGBToLinear(colorf.y);
	colorf.z = sRGBToLinear(colorf.z);

	return colorf;
}

void NoviceSystem::DrawBox(int x, int y, int w, int h, float angle, unsigned int color) {

	assert(indexBox_ < kMaxBoxCount);

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	float left = 0;
	float top = 0;
	float right = (float)w;
	float bottom = (float)h;

	// 頂点データ
	std::array vertices = {
	    VertexPosColor{{left, bottom, 0.0f},  {1, 1, 1, 1}}, // 左下
	    VertexPosColor{{left, top, 0.0f},     {1, 1, 1, 1}}, // 左上
	    VertexPosColor{{right, top, 0.0f},    {1, 1, 1, 1}}, // 右上
	    VertexPosColor{{right, bottom, 0.0f}, {1, 1, 1, 1}}, // 右下
	};
	std::array<uint16_t, 6> indices = {0, 1, 2, 2, 3, 0};

	Vector4 colorf = FloatColor(color);

	// 回転
	for (auto& vertex : vertices) {
		// 回転
		vertex.pos = {
		    vertex.pos.x * cosf(angle) + vertex.pos.y * -sinf(angle),
		    vertex.pos.x * sinf(angle) + vertex.pos.y * cosf(angle), vertex.pos.z};
		// 平行移動
		vertex.pos.x += static_cast<float>(x);
		vertex.pos.y += static_cast<float>(y);
		// 色
		vertex.color = colorf;
	}

	size_t indexVertex = indexBox_ * kVertexCountBox;
	size_t indexIndex = indexBox_ * kIndexCountBox;

	assert(vertices.size() <= kVertexCountBox);
	assert(indices.size() <= kIndexCountBox);
	// 頂点バッファへのデータ転送
	std::copy(vertices.begin(), vertices.end(), &box_->vertMap[indexVertex]);
	// インデックスバッファへのデータ転送
	std::copy(indices.begin(), indices.end(), &box_->indexMap[indexIndex]);

	// パイプラインステートの設定
	commandList->SetPipelineState(pipelineSetTriangles_[blendMode_]->pipelineState.Get());
	// ルートシグネチャの設定
	commandList->SetGraphicsRootSignature(pipelineSetTriangles_[blendMode_]->rootSignature.Get());
	// プリミティブ形状を設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 頂点バッファの設定
	commandList->IASetVertexBuffers(0, 1, &box_->vbView);
	// インデックスバッファの設定
	commandList->IASetIndexBuffer(&box_->ibView);
	// CBVをセット（ワールド行列）
	commandList->SetGraphicsRootConstantBufferView(0, constBuffer_->GetGPUVirtualAddress());
	// 描画コマンド
	commandList->DrawIndexedInstanced(
	    kIndexCountBox, 1, static_cast<UINT>(indexIndex), static_cast<INT>(indexVertex), 0);
	// 使用カウント上昇
	indexBox_++;
}

void NoviceSystem::DrawTriangle(
    int x1, int y1, int x2, int y2, int x3, int y3, unsigned int color) {
	assert(indexTriangle_ < kMaxTriangleCount);

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 頂点データ
	std::array vertices = {
	    VertexPosColor{{static_cast<float>(x1), static_cast<float>(y1), 0.0f}, {1, 1, 1, 1}},
	    VertexPosColor{{static_cast<float>(x2), static_cast<float>(y2), 0.0f}, {1, 1, 1, 1}},
	    VertexPosColor{{static_cast<float>(x3), static_cast<float>(y3), 0.0f}, {1, 1, 1, 1}},
	};

	Vector4 colorf = FloatColor(color);

	// 回転
	for (auto& vertex : vertices) {
		// 色
		vertex.color = colorf;
	}

	size_t indexVertex = indexTriangle_ * kVertexCountTriangle;

	assert(vertices.size() <= kVertexCountTriangle);
	// 頂点バッファへのデータ転送
	std::memcpy(
	    &triangle_->vertMap[indexVertex], vertices.data(), sizeof(vertices[0]) * vertices.size());

	// パイプラインステートの設定
	commandList->SetPipelineState(pipelineSetTriangles_[blendMode_]->pipelineState.Get());
	// ルートシグネチャの設定
	commandList->SetGraphicsRootSignature(pipelineSetTriangles_[blendMode_]->rootSignature.Get());
	// プリミティブ形状を設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 頂点バッファの設定
	commandList->IASetVertexBuffers(0, 1, &triangle_->vbView);
	// CBVをセット（ワールド行列）
	commandList->SetGraphicsRootConstantBufferView(0, constBuffer_->GetGPUVirtualAddress());
	// 描画コマンド
	commandList->DrawInstanced(kVertexCountTriangle, 1, static_cast<INT>(indexVertex), 0);
	// 使用カウント上昇
	indexTriangle_++;
}

void NoviceSystem::DrawTriangles(std::span<VertexPosColor> trianglePoints) {
	assert(trianglePoints.size() % 3 == 0);
	[[maybe_unused]] auto numTriangles = trianglePoints.size() / 3;
	assert((indexTriangle_ + numTriangles) < kMaxTriangleCount);

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	size_t indexVertex = indexTriangle_ * kVertexCountTriangle;

	//  頂点バッファへのデータ転送
	std::memcpy(
	    &triangle_->vertMap[indexVertex], trianglePoints.data(),
	    sizeof(trianglePoints[0]) * trianglePoints.size());

	//  パイプラインステートの設定
	commandList->SetPipelineState(pipelineSetTriangles_[blendMode_]->pipelineState.Get());
	// ルートシグネチャの設定
	commandList->SetGraphicsRootSignature(pipelineSetTriangles_[blendMode_]->rootSignature.Get());
	// プリミティブ形状を設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 頂点バッファの設定
	commandList->IASetVertexBuffers(0, 1, &triangle_->vbView);
	// CBVをセット（ワールド行列）
	commandList->SetGraphicsRootConstantBufferView(0, constBuffer_->GetGPUVirtualAddress());
	// 描画コマンド
	commandList->DrawInstanced(UINT(trianglePoints.size()), 1, static_cast<INT>(indexVertex), 0);
	// 使用カウント上昇
	indexTriangle_ += uint32_t(trianglePoints.size() / 3);
}

void NoviceSystem::DrawLine(int x1, int y1, int x2, int y2, unsigned int color) {
	assert(indexLine_ < kMaxLineCount);

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 頂点データ
	std::array vertices = {
	    VertexPosColor{{static_cast<float>(x1), static_cast<float>(y1), 0.0f}, {1, 1, 1, 1}},
	    VertexPosColor{{static_cast<float>(x2), static_cast<float>(y2), 0.0f}, {1, 1, 1, 1}},
	};

	Vector4 colorf = FloatColor(color);

	// 回転
	for (auto& vertex : vertices) {
		// 色
		vertex.color = colorf;
	}

	size_t indexVertex = indexLine_ * kVertexCountLine;

	assert(vertices.size() <= kVertexCountLine);
	// 頂点バッファへのデータ転送
	std::copy(vertices.begin(), vertices.end(), &line_->vertMap[indexVertex]);

	// パイプラインステートの設定
	commandList->SetPipelineState(pipelineSetLines_[blendMode_]->pipelineState.Get());
	// ルートシグネチャの設定
	commandList->SetGraphicsRootSignature(pipelineSetLines_[blendMode_]->rootSignature.Get());
	// プリミティブ形状を設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	// 頂点バッファの設定
	commandList->IASetVertexBuffers(0, 1, &line_->vbView);
	// CBVをセット（ワールド行列）
	commandList->SetGraphicsRootConstantBufferView(0, constBuffer_->GetGPUVirtualAddress());
	// 描画コマンド
	commandList->DrawInstanced(kVertexCountLine, 1, static_cast<INT>(indexVertex), 0);
	// 使用カウント上昇
	indexLine_++;
}

void NoviceSystem::DrawLines(std::span<VertexPosColor> linePoints) {
	assert(linePoints.size() % 1 == 0);
	[[maybe_unused]] auto numLines = linePoints.size() / 2;
	assert((indexLine_ + numLines) < kMaxLineCount);

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	size_t indexVertex = indexLine_ * kVertexCountLine;

	//  頂点バッファへのデータ転送
	std::memcpy(
	    &line_->vertMap[indexVertex], linePoints.data(), sizeof(linePoints[0]) * linePoints.size());

	// パイプラインステートの設定
	commandList->SetPipelineState(pipelineSetLines_[blendMode_]->pipelineState.Get());
	// ルートシグネチャの設定
	commandList->SetGraphicsRootSignature(pipelineSetLines_[blendMode_]->rootSignature.Get());
	// プリミティブ形状を設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	// 頂点バッファの設定
	commandList->IASetVertexBuffers(0, 1, &line_->vbView);
	// CBVをセット（ワールド行列）
	commandList->SetGraphicsRootConstantBufferView(0, constBuffer_->GetGPUVirtualAddress());
	// 描画コマンド
	commandList->DrawInstanced(UINT(linePoints.size()), 1, static_cast<INT>(indexVertex), 0);
	// 使用カウント上昇
	indexLine_ += uint32_t(linePoints.size() / 2);
}

void NoviceSystem::DrawSpriteRect(
    int destX, int destY, int srcX, int srcY, int srcW, int srcH, int textureHandle, float scaleX,
    float scaleY, float angle, unsigned int color) {
	assert(indexSprite_ < kMaxSpriteCount);

	auto& sprite = sprites_[indexSprite_];

	const D3D12_RESOURCE_DESC& texDesc =
	    TextureManager::GetInstance()->GetResoureDesc(textureHandle);

	sprite->SetTextureHandle(textureHandle);
	sprite->SetPosition({(float)destX, (float)destY});
	sprite->SetRotation(angle);
	sprite->SetSize({texDesc.Width * scaleX, texDesc.Height * scaleY});
	sprite->SetColor(FloatColor(color));
	if (srcX < 0 || srcY < 0 || srcW < 0 || srcH < 0) {
		sprite->SetTextureRect({0.0f, 0.0f}, {(float)texDesc.Width, (float)texDesc.Height});
	} else {
		sprite->SetTextureRect({(float)srcX, (float)srcY}, {(float)srcW, (float)srcH});
	}

	// 描画
	Sprite::PreDraw(dxCommon_->GetCommandList(), ToSpriteBlendMode(blendMode_));
	sprite->Draw();
	Sprite::PostDraw();

	// 使用カウント上昇
	indexSprite_++;
}

void NoviceSystem::DrawQuad(
    int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int srcX, int srcY, int srcW,
    int srcH, int textureHandle, unsigned int color) {
	assert(indexQuad_ < kMaxQuadCount);

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	const D3D12_RESOURCE_DESC& resourceDesc =
	    TextureManager::GetInstance()->GetResoureDesc(textureHandle);

	float uvLeft = static_cast<float>(srcX) / static_cast<float>(resourceDesc.Width);
	float uvRight = static_cast<float>(srcX + srcW) / static_cast<float>(resourceDesc.Width);
	float uvTop = static_cast<float>(srcY) / static_cast<float>(resourceDesc.Height);
	float uvBottom = static_cast<float>(srcY + srcH) / static_cast<float>(resourceDesc.Height);

	// 頂点データ
	std::array vertices = {
	    Sprite::VertexPosUv{
	                        {static_cast<float>(x1), static_cast<float>(y1), 0.0f}, {uvLeft, uvTop}    },
	    Sprite::VertexPosUv{
	                        {static_cast<float>(x2), static_cast<float>(y2), 0.0f}, {uvRight, uvTop}   },
	    Sprite::VertexPosUv{
	                        {static_cast<float>(x3), static_cast<float>(y3), 0.0f}, {uvLeft, uvBottom} },
	    Sprite::VertexPosUv{
	                        {static_cast<float>(x4), static_cast<float>(y4), 0.0f}, {uvRight, uvBottom}},
	};
	std::array<uint16_t, 4> indices = {0, 1, 2, 3};

	Vector4 colorf = FloatColor(color);

	size_t indexVertex = indexQuad_ * kVertexCountQuad;
	size_t indexIndex = indexQuad_ * kIndexCountQuad;

	assert(vertices.size() <= kVertexCountQuad);
	assert(indices.size() <= kIndexCountQuad);
	// 頂点バッファへのデータ転送
	std::copy(vertices.begin(), vertices.end(), &quad_->vertMap[indexVertex]);
	// インデックスバッファへのデータ転送
	std::copy(indices.begin(), indices.end(), &quad_->indexMap[indexIndex]);

	// パイプラインステート等の設定
	Sprite::PreDraw(dxCommon_->GetCommandList(), ToSpriteBlendMode(blendMode_));
	// 色の設定
	constBufferForQuads_[indexQuad_].address->color = colorf;
	// 平行投影による射影行列の設定
	constBufferForQuads_[indexQuad_].address->mat = Matrix4Orthographic(
	    0, float(dxCommon_->GetBackBufferWidth()), float(dxCommon_->GetBackBufferHeight()), 0, 0,
	    1);
	// 頂点バッファの設定
	commandList->IASetVertexBuffers(0, 1, &quad_->vbView);
	// インデックスバッファの設定
	commandList->IASetIndexBuffer(&quad_->ibView);
	// CBVをセット（ワールド行列）
	commandList->SetGraphicsRootConstantBufferView(
	    0, constBufferForQuads_[indexQuad_].resource->GetGPUVirtualAddress());
	// シェーダリソースビューをセット
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, 1, textureHandle);
	// 描画コマンド
	commandList->DrawIndexedInstanced(
	    kIndexCountQuad, 1, static_cast<UINT>(indexIndex), static_cast<INT>(indexVertex), 0);
	Sprite::PostDraw();

	// 使用カウント上昇
	indexQuad_++;
}

int NoviceSystem::CheckHitKey(int keyCode) { return input_->PushKey((BYTE)keyCode) ? 1 : 0; }

void NoviceSystem::GetHitKeyStateAll(char* keyStateBuf) {
	const std::array<BYTE, 256>& keys = input_->GetAllKey();
	memcpy(keyStateBuf, keys.data(), size(keys));
}

bool NoviceSystem::IsPressMouse(int buttonNumber) { return input_->IsPressMouse(buttonNumber); }

bool NoviceSystem::IsTriggerMouse(int buttonNumber) { return input_->IsTriggerMouse(buttonNumber); }

const Vector2& NoviceSystem::GetMousePosition() { return input_->GetMousePosition(); }

int NoviceSystem::GetWheel() { return input_->GetWheel(); }

void NoviceSystem::SetBlendMode(BlendMode blendMode) { blendMode_ = blendMode; }

bool NoviceSystem::GetJoystickState(int stickNo, DIJOYSTATE2& out) {
	return input_->GetJoystickState(stickNo, out);
}

bool NoviceSystem::GetJoystickStatePrevious(int stickNo, DIJOYSTATE2& out) {
	return input_->GetJoystickStatePrevious(stickNo, out);
}

bool NoviceSystem::GetJoystickState(int stickNo, XINPUT_STATE& out) {
	return input_->GetJoystickState(stickNo, out);
}
bool NoviceSystem::GetJoystickStatePrevious(int stickNo, XINPUT_STATE& out) {
	return input_->GetJoystickStatePrevious(stickNo, out);
}

void NoviceSystem::SetJoystickDeadZone(int stickNo, int deadZoneL, int deadZoneR) {
	input_->SetJoystickDeadZone(stickNo, deadZoneL, deadZoneR);
}

int NoviceSystem::GetNumberOfJoysticks() { return int(input_->GetNumberOfJoysticks()); }

void NoviceSystem::ScreenPrintf(int x, int y, const char* text, va_list& args) {

	// 書式付き文字列を変換
	vsnprintf(textBuffer.data(), textBuffer.size() - 1, text, args);
	debugText_->Print(textBuffer.data(), (float)x, (float)y, 1.0f);
}

void NoviceSystem::ConsolePrintf(const char* text, va_list& args) {

	// 書式付き文字列を変換
	vsnprintf(textBuffer.data(), textBuffer.size() - 1, text, args);
	OutputDebugStringA(textBuffer.data());
}

int NoviceSystem::LoadAudio(const char* fileName) { return (int)audio_->LoadWave(fileName); }

int NoviceSystem::PlayAudio(int soundHandle, int loopFlag, float volume) {

	bool boolLoop = loopFlag ? true : false;
	return (int)audio_->PlayWave((uint32_t)soundHandle, boolLoop, volume);
}

void NoviceSystem::StopAudio(int playHandle) { audio_->StopWave((uint32_t)playHandle); }

void NoviceSystem::PauseAudio(int playHandle) { audio_->PauseWave((uint32_t)playHandle); }

void NoviceSystem::ResumeAudio(int playHandle) { audio_->ResumeWave((uint32_t)playHandle); }

void NoviceSystem::SetAudioVolume(int playHandle, float volume) {
	audio_->SetVolume((uint32_t)playHandle, (std::max)(0.0f, volume));
}

bool NoviceSystem::IsPlayingAudio(int playHandle) { return audio_->IsPlaying(playHandle); }

void NoviceSystem::SetFullscreen(bool fullscreen) { winApp_->SetFullscreen(fullscreen); }

void NoviceSystem::AllowWindowSizeChanged(bool allow) {
	winApp_->SetSizeChangeMode(
	    allow ? WinApp::SizeChangeMode::kFixedAspect : WinApp::SizeChangeMode::kNone);
}

int NoviceSystem::ProcessMessage() { return winApp_->ProcessMessage() ? 1 : 0; }

void NoviceSystem::BeginFrame() {
	imGuiManager_->Begin();
	input_->Update(); // DirectX描画前処理
	dxCommon_->PreDraw();
	SetBlendMode(kBlendModeNormal);
}

void NoviceSystem::EndFrame() {
	imGuiManager_->End();

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// スプライト描画前処理
	Sprite::PreDraw(commandList);
	// デバッグテキストの描画
	debugText_->DrawAll();
	// スプライト描画後処理
	Sprite::PostDraw();
	// ImGui描画
	imGuiManager_->Draw();
	// DirectX描画終了
	dxCommon_->PostDraw();

	Reset();
}

Matrix4x4 NoviceSystem::Matrix4Orthographic(
    float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ) {
	assert(fabsf(viewRight - viewLeft) > 0.00001f);
	assert(fabsf(viewTop - viewBottom) > 0.00001f);
	assert(fabsf(farZ - nearZ) > 0.00001f);

	float width = 1.0f / (viewRight - viewLeft);
	float height = 1.0f / (viewTop - viewBottom);
	float fRange = 1.0f / (farZ - nearZ);
	float sx = width * 2.0f;
	float sy = height * 2.0f;
	float sz = fRange;
	float tx = -(viewLeft + viewRight) * width;
	float ty = -(viewTop + viewBottom) * height;
	float tz = -fRange * nearZ;

	Matrix4x4 m{sx, 0.0f, 0.0f, 0.0f, 0.0f, sy, 0.0f, 0.0f, 0.0f, 0.0f, sz, 0.0f, tx, ty, tz, 1.0f};

	return m;
}

HWND NoviceSystem::GetWindowHandle() { return winApp_->GetHwnd(); }

WinApp* sWinApp = nullptr;
DirectXCommon* sDxCommon = nullptr;
ImGuiManager* sImGuiManager = nullptr;
// 汎用機能
Input* sInput = nullptr;
Audio* sAudio = nullptr;
DebugText* sDebugText = nullptr;
GameScene* sGameScene = nullptr;
NoviceSystem* sNoviceSystem = nullptr;

} // namespace

void Novice::Initialize(const char* title, int width, int height) {
	// 既に初期化済み
	assert(!sWinApp);
	assert(!sDxCommon);
	assert(!sInput);
	assert(!sAudio);
	assert(!sDebugText);
	assert(!sGameScene);
	assert(!sNoviceSystem);
	assert(!sImGuiManager);

	// ゲームウィンドウの作成
	auto&& titleString = ConvertString(title);
	sWinApp = WinApp::GetInstance();
	sWinApp->CreateGameWindow(
	    titleString.c_str(), WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME), width,
	    height);
	sWinApp->SetSizeChangeMode(WinApp::SizeChangeMode::kNone);

	// DirectX初期化処理
	sDxCommon = DirectXCommon::GetInstance();
	sDxCommon->Initialize(sWinApp, width, height);

	// ImGuiの初期化
	sImGuiManager = ImGuiManager::GetInstance();
	sImGuiManager->Initialize(sWinApp, sDxCommon);

	// 入力の初期化
	sInput = Input::GetInstance();
	sInput->Initialize();

	// オーディオの初期化
	sAudio = Audio::GetInstance();
	sAudio->Initialize(GetResourceRootChar());

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(sDxCommon->GetDevice(), GetResourceRootChar());
	TextureManager::Load("white1x1.png");

	// スプライト静的初期化
	Sprite::StaticInitialize(sDxCommon->GetDevice(), width, height, GetResourceRoot());

	// デバッグテキスト初期化
	sDebugText = DebugText::GetInstance();
	sDebugText->Initialize();

	// 入門用システム初期化
	sNoviceSystem = new NoviceSystem();
	sNoviceSystem->Initialize();

	// ゲームシーンの初期化
	sGameScene = new GameScene();
	sGameScene->Initialize();
}

void Novice::Finalize() {
	// 各種解放
	SafeDelete(sGameScene);
	SafeDelete(sNoviceSystem);
	sAudio->Finalize();
	sAudio = nullptr;
	sImGuiManager->Finalize();
	sImGuiManager = nullptr;

	// ゲームウィンドウの破棄
	sWinApp->TerminateGameWindow();
}

void Novice::DrawBox(
    int x, int y, int w, int h, float angle, unsigned int color, FillMode fillMode) {
	if (fillMode == kFillModeSolid) {
		sNoviceSystem->DrawBox(x, y, w, h, angle, color);
	} else {
		sNoviceSystem->DrawLine(x, y, x + w, y, color);         // 左上から右上
		sNoviceSystem->DrawLine(x, y, x, y + h, color);         // 左上から左下
		sNoviceSystem->DrawLine(x + w, y, x + w, y + h, color); // 右上から右下
		sNoviceSystem->DrawLine(x, y + h, x + w, y + h, color); // 左下から右下
	}
}

void Novice::DrawTriangle(
    int x1, int y1, int x2, int y2, int x3, int y3, unsigned int color, FillMode fillMode) {
	if (fillMode == kFillModeSolid) {
		sNoviceSystem->DrawTriangle(x1, y1, x2, y2, x3, y3, color);
	} else {
		sNoviceSystem->DrawLine(x1, y1, x2, y2, color); // 1から2
		sNoviceSystem->DrawLine(x2, y2, x3, y3, color); // 2から3
		sNoviceSystem->DrawLine(x3, y3, x1, y1, color); // 3から1
	}
}

void Novice::DrawLine(int x1, int y1, int x2, int y2, unsigned int color) {
	sNoviceSystem->DrawLine(x1, y1, x2, y2, color);
}

void Novice::DrawEllipse(
    int x, int y, int radiusX, int radiusY, float angle, unsigned int color, FillMode fillMode) {
	constexpr size_t kNumDivision = 48; // 分割数
	constexpr float kRadianPerDivision =
	    3.1415926535f * 2.0f / float(kNumDivision); // 分割1単位の角度

	Vector4 colorf = NoviceSystem::FloatColor(color);
	float angleCos = std::cos(angle);
	float angleSin = std::sin(angle);

	if (fillMode == kFillModeSolid) {
		// ポリゴン
		NoviceSystem::VertexPosColor points[kNumDivision * 3];
		for (size_t index = 0; index < kNumDivision; ++index) {
			points[index * 3].pos = {float(x), float(y), 0};
			points[index * 3].color = colorf;
			float nextBaseX = std::cos(kRadianPerDivision * (index + 1)) * radiusX;
			float nextBaseY = std::sin(kRadianPerDivision * (index + 1)) * radiusY;
			if (index == 0) {
				float baseX = std::cos(kRadianPerDivision * index) * radiusX;
				float baseY = std::sin(kRadianPerDivision * index) * radiusY;
				points[index * 3 + 1].pos = {
				    baseX * angleCos - baseY * angleSin + x,
				    baseY * angleCos + baseX * angleSin + y, 0};
			} else {
				points[index * 3 + 1].pos = points[(index - 1) * 3 + 2].pos;
			}

			points[index * 3 + 1].color = colorf;
			points[index * 3 + 2].pos = {
			    nextBaseX * angleCos - nextBaseY * angleSin + x,
			    nextBaseY * angleCos + nextBaseX * angleSin + y, 0};
			points[index * 3 + 2].color = colorf;
		}
		sNoviceSystem->DrawTriangles(points);
	} else {
		// ライン
		NoviceSystem::VertexPosColor points[kNumDivision * 2];
		for (size_t index = 0; index < kNumDivision; ++index) {
			float nextBaseX = std::cos(kRadianPerDivision * (index + 1)) * radiusX;
			float nextBaseY = std::sin(kRadianPerDivision * (index + 1)) * radiusY;
			if (index == 0) {
				float baseX = std::cos(kRadianPerDivision * index) * radiusX;
				float baseY = std::sin(kRadianPerDivision * index) * radiusY;
				points[index * 2].pos = {
				    baseX * angleCos - baseY * angleSin + x,
				    baseY * angleCos + baseX * angleSin + y, 0};
			} else {
				points[index * 2].pos = points[(index - 1) * 2 + 1].pos;
			}

			points[index * 2].color = colorf;
			points[index * 2 + 1].pos = {
			    nextBaseX * angleCos - nextBaseY * angleSin + x,
			    nextBaseY * angleCos + nextBaseX * angleSin + y, 0};
			points[index * 2 + 1].color = colorf;
		}
		sNoviceSystem->DrawLines(points);
	}
}

int Novice::LoadTexture(const char* fileName) {
	return static_cast<int>(TextureManager::Load(fileName));
}

void Novice::UnloadTexture(int textureHandle) { TextureManager::Unload(textureHandle); }

void Novice::DrawSprite(
    int x, int y, int textureHandle, float scaleX, float scaleY, float angle, unsigned int color) {
	sNoviceSystem->DrawSpriteRect(
	    x, y, -1, -1, -1, -1, textureHandle, scaleX, scaleY, angle, color);
}

void Novice::DrawSpriteRect(
    int destX, int destY, int srcX, int srcY, int srcW, int srcH, int textureHandle, float scaleX,
    float scaleY, float angle, unsigned int color) {
	sNoviceSystem->DrawSpriteRect(
	    destX, destY, srcX, srcY, srcW, srcH, textureHandle, scaleX, scaleY, angle, color);
}

void Novice::DrawQuad(
    int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int srcX, int srcY, int srcW,
    int srcH, int textureHandle, unsigned int color) {
	sNoviceSystem->DrawQuad(
	    x1, y1, x2, y2, x3, y3, x4, y4, srcX, srcY, srcW, srcH, textureHandle, color);
}

int Novice::LoadAudio(const char* fileName) { return sNoviceSystem->LoadAudio(fileName); }

int Novice::PlayAudio(int soundHandle, int loopFlag, float volume) {
	return sNoviceSystem->PlayAudio(soundHandle, loopFlag, volume);
}

void Novice::StopAudio(int playHandle) { sNoviceSystem->StopAudio(playHandle); }

void Novice::PauseAudio(int playHandle) { sNoviceSystem->PauseAudio(playHandle); }

void Novice::ResumeAudio(int playHandle) { sNoviceSystem->ResumeAudio(playHandle); }

void Novice::SetAudioVolume(int playHandle, float volume) {
	sNoviceSystem->SetAudioVolume(playHandle, volume);
}

int Novice::IsPlayingAudio(int playHandle) {
	return sNoviceSystem->IsPlayingAudio(playHandle) ? 1 : 0;
}

int Novice::CheckHitKey(int keyCode) { return sNoviceSystem->CheckHitKey(keyCode); }

void Novice::GetHitKeyStateAll(char* keyStateBuf) {

	assert(keyStateBuf);
	sNoviceSystem->GetHitKeyStateAll(keyStateBuf);
}

int Novice::IsPressMouse(int buttonNumber) {
	return sNoviceSystem->IsPressMouse(buttonNumber) ? 1 : 0;
}

int Novice::IsTriggerMouse(int buttonNumber) {
	return sNoviceSystem->IsTriggerMouse(buttonNumber) ? 1 : 0;
}

int Novice::GetMousePosition(int* positionX, int* positionY) {
	// ClientRect内の座標
	const Vector2& mousePosition = sNoviceSystem->GetMousePosition();
	if (positionX && positionY) {

		// swapchain解像度内での座標に変換する。swapchainの解像度は固定。
		RECT clientRect{};
		GetClientRect(sNoviceSystem->GetWindowHandle(), &clientRect);
		if (clientRect.left == clientRect.right || clientRect.top == clientRect.bottom) {
			return 0;
		}
		float normalizedX =
		    mousePosition.x / static_cast<float>(clientRect.right - clientRect.left);
		float normalizedY =
		    mousePosition.y / static_cast<float>(clientRect.bottom - clientRect.top);

		*positionX = static_cast<int>(normalizedX * sNoviceSystem->dxCommon_->GetBackBufferWidth());
		*positionY =
		    static_cast<int>(normalizedY * sNoviceSystem->dxCommon_->GetBackBufferHeight());
		return 1;
	}
	return 0;
}

void Novice::SetMouseCursorVisibility(int visibility) { ::ShowCursor(visibility); }

int Novice::GetWheel() { return sNoviceSystem->GetWheel(); }

void Novice::SetBlendMode(BlendMode blendMode) { sNoviceSystem->SetBlendMode(blendMode); }

void Novice::SetIcon(const char* fileName) {
	std::string iconPath;
	if (2 < strlen(fileName) && fileName[0] == '.' && fileName[1] == '/') {
		char fullpath[4096] = {0};
		GetFullPathNameA(fileName, sizeof(fullpath), fullpath, nullptr);
		iconPath = fullpath;
	} else {
		iconPath = GetResourceRootChar() + fileName;
	}
	HICON smallIconHandle = static_cast<HICON>(
	    LoadImageA(GetModuleHandle(NULL), iconPath.c_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE));
	HICON largeIconHandle = static_cast<HICON>(
	    LoadImageA(GetModuleHandle(NULL), iconPath.c_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE));
	SendMessage(
	    sWinApp->GetHwnd(), WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(smallIconHandle));
	SendMessage(
	    sWinApp->GetHwnd(), WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(largeIconHandle));
}

void Novice::SetWindowMode(WindowMode windowMode) {
	sNoviceSystem->SetFullscreen(windowMode == kFullscreen);
}

void Novice::AllowWindowSizeChanged(int allow) { sNoviceSystem->AllowWindowSizeChanged(allow); }
int Novice::GetJoystickState(int stickNo, DIJOYSTATE2* out) {
	if (out) {
		return sNoviceSystem->GetJoystickState(stickNo, *out) ? 1 : 0;
	}
	return 0;
}

int Novice::GetNumberOfJoysticks() { return sNoviceSystem->GetNumberOfJoysticks(); }

int Novice::IsTriggerButton(int stickNo, PadButton button) {
	DIJOYSTATE2 stateDInput{};
	DIJOYSTATE2 statePreviousDInput{};

	XINPUT_STATE stateXInput{};
	XINPUT_STATE statePreviousXInput{};
	if (sNoviceSystem->GetJoystickState(stickNo, stateDInput) &&
	    sNoviceSystem->GetJoystickStatePrevious(stickNo, statePreviousDInput)) {
		return ((stateDInput.rgbButtons[button] & 0x80) &&
		        !(statePreviousDInput.rgbButtons[button] & 0x80))
		           ? 1
		           : 0;
	} else if (
	    sNoviceSystem->GetJoystickState(stickNo, stateXInput) &&
	    sNoviceSystem->GetJoystickStatePrevious(stickNo, statePreviousXInput)) {
		return IsPressXInputButton(stateXInput, button) &&
		               !IsPressXInputButton(statePreviousXInput, button)
		           ? 1
		           : 0;
	}

	return 0;
}

int Novice::IsPressButton(int stickNo, PadButton button) {

	DIJOYSTATE2 stateDInput{};
	XINPUT_STATE stateXInput{};
	if (sNoviceSystem->GetJoystickState(stickNo, stateDInput)) {
		if (stateDInput.rgbButtons[button] & 0x80) {
			return 1;
		}
	} else if (sNoviceSystem->GetJoystickState(stickNo, stateXInput)) {
		return IsPressXInputButton(stateXInput, button) ? 1 : 0;
	}
	return 0;
}

int Novice::GetAnalogInputLeft(int stickNo, int* x, int* y) {
	if (!x || !y) {
		return 0;
	}
	DIJOYSTATE2 stateDInput{};
	XINPUT_STATE stateXInput{};
	if (sNoviceSystem->GetJoystickState(stickNo, stateDInput)) {
		*x = stateDInput.lX;
		*y = stateDInput.lY;
		return 1;
	} else if (sNoviceSystem->GetJoystickState(stickNo, stateXInput)) {
		*x = stateXInput.Gamepad.sThumbLX;
		*y = stateXInput.Gamepad.sThumbLY;
		*y = -*y;
		return 1;
	}
	*x = 0;
	*y = 0;
	return 0;
}

int Novice::GetAnalogInputRight(int stickNo, int* x, int* y) {
	if (!x || !y) {
		return 0;
	}
	DIJOYSTATE2 stateDInput{};
	XINPUT_STATE stateXInput{};
	if (sNoviceSystem->GetJoystickState(stickNo, stateDInput)) {
		*x = stateDInput.lZ;
		*y = stateDInput.lRz;
		return 1;
	} else if (sNoviceSystem->GetJoystickState(stickNo, stateXInput)) {
		*x = stateXInput.Gamepad.sThumbRX;
		*y = stateXInput.Gamepad.sThumbRY;
		*y = -*y;
		return 1;
	}
	*x = 0;
	*y = 0;
	return 0;
}

void Novice::SetJoystickDeadZone(int stickNo, int deadZoneL, int deadZoneR) {
	sNoviceSystem->SetJoystickDeadZone(stickNo, deadZoneL, deadZoneR);
}

void Novice::ScreenPrintf(int x, int y, const char* format, ...) {

	// 可変長引数をオブジェクトにまとめる
	va_list args;
	va_start(args, format);
	sNoviceSystem->ScreenPrintf(x, y, format, args);
	va_end(args);
}

void Novice::ConsolePrintf(const char* format, ...) {

	// 可変長引数をオブジェクトにまとめる
	va_list args;
	va_start(args, format);
	sNoviceSystem->ConsolePrintf(format, args);
	va_end(args);
}

int Novice::ProcessMessage() { return sNoviceSystem->ProcessMessage(); }

void Novice::BeginFrame() { sNoviceSystem->BeginFrame(); }

void Novice::EndFrame() { sNoviceSystem->EndFrame(); }
