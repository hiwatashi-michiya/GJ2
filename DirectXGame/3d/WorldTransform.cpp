﻿#include "DirectXCommon.h"
#include "WorldTransform.h"
#include <cassert>
#include <d3dx12.h>

using namespace DirectX;

void WorldTransform::Initialize() {
	CreateConstBuffer();
	Map();
	UpdateMatrix();
}

void WorldTransform::CreateConstBuffer() {
	HRESULT result;

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc =
	  CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataWorldTransform) + 0xff) & ~0xff);

	// 定数バッファの生成
	result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
	  &heapProps, // アップロード可能
	  D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
	  IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));
}

void WorldTransform::Map() {
	// 定数バッファとのデータリンク
	HRESULT result = constBuff_->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));
}

void WorldTransform::UpdateMatrix() {
	XMMATRIX matScale, matRot, matTrans;

	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale_.x, scale_.y, scale_.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(rotation_.z);
	matRot *= XMMatrixRotationX(rotation_.x);
	matRot *= XMMatrixRotationY(rotation_.y);
	matTrans = XMMatrixTranslation(translation_.x, translation_.y, translation_.z);

	// ワールド行列の合成
	matWorld_ = XMMatrixIdentity(); // 変形をリセット
	matWorld_ *= matScale;          // ワールド行列にスケーリングを反映
	matWorld_ *= matRot;            // ワールド行列に回転を反映
	matWorld_ *= matTrans;          // ワールド行列に平行移動を反映

	// 親行列の指定がある場合は、掛け算する
	if (parent_) {
		matWorld_ *= parent_->matWorld_;
	}

	// 定数バッファに書き込み
	constMap->matWorld = matWorld_;
}
