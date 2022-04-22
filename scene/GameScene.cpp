#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include<time.h>

using namespace DirectX;

//コンストラクタ
GameScene::GameScene() {}

//デストラクタ
GameScene::~GameScene() { 
	//BG
	delete spriteBG_;
	//ステージ
	delete modelStage_;
	//プレイヤー
	delete modelPlayer_;
	//ビーム
	delete modelBeam_;
	//エネミー
	delete modelEnemy_;
}

#pragma region 初期化
//初期化
void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	//BG(2Dスプライト)
	textureHandleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0, 0});
	//ビュープロジェクションの初期化
	viewProjection_.eye = {0, 1, -6};
	viewProjection_.target = {0, 1, 0};
	viewProjection_.Initialize();

	//ステージの位置を下げる
	textureHandleStage_ = TextureManager::Load("stage.jpg");
	modelStage_ = Model::Create();
	//ステージ
	worldTransformStage_.translation_ = {0, -1.5f, 0};
	worldTransformStage_.scale_ = {4.5f, 1, 40};
	worldTransformStage_.Initialize();

	//プレイヤー
	textureHandlePlayer_ = TextureManager::Load("Player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();
	
	//ビーム
	textureHandleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();
	worldTransformBeam_.scale_ = {0.3f, 0.3f, 0.3f};
	worldTransformBeam_.Initialize();

	//エネミー
	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();
	worldTransformEnemy_.scale_ = {0.5f, 0.5f, 0.5f};
	//発生のための乱数の種(シード)
	srand(time(nullptr));
	worldTransformEnemy_.Initialize();
}
#pragma endregion 初期化


//更新
void GameScene::Update() { 
	PlayerUpdate();
	EnemyUpdate();
	BeamUpdate();
	std::string beamDebug = std::string("beamFlag:") + std::to_string(beamFlag_);
	debugText_->Print(beamDebug, 50, 50, 1.0f);
	std::string enemyDebug = std::string("enemyFlag:") + std::to_string(enemyFlag_);
	debugText_->Print(enemyDebug, 50, 60, 1.0f);
	/*std::string z = std::string("z:") + std::to_string(worldTransformBeam_.translation_.z);
	debugText_->Print(z, 50, 600, 1.0f);*/
}

//描画
void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	//背景
	spriteBG_->Draw();
	
	/// </summary>
	
	// スプライト描画後処理
	Sprite::PostDraw();

	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	//ステージ
	modelStage_->Draw(worldTransformStage_, viewProjection_, textureHandleStage_);
	//プレイヤー
	modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	//ビーム(フラグが1の時)
	if (beamFlag_ == 1) {
		modelBeam_->Draw(worldTransformBeam_, viewProjection_, textureHandleBeam_);
	}
	//エネミー(フラグが1の時)
	if (enemyFlag_ == 1) {
		modelEnemy_->Draw(worldTransformEnemy_, viewProjection_, textureHandleEnemy_);
	}
	/// </summary>
	
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

//プレイヤー更新
void GameScene::PlayerUpdate() {
	//x座標が3.5f以下の場合右へ移動(移動制限)
	if (input_->PushKey(DIK_RIGHT) && worldTransformPlayer_.translation_.x <= 3.5f) {
		worldTransformPlayer_.translation_.x += 0.1f;
	}
	//x座標が-3.5f以上の場合左へ移動(移動制限)
	if (input_->PushKey(DIK_LEFT) && worldTransformPlayer_.translation_.x >= -3.5f) {
		worldTransformPlayer_.translation_.x -= 0.1f;
	}
	//行列更新
	worldTransformPlayer_.UpdateMatrix();
}

//ビーム更新
void GameScene::BeamUpdate() {
	//発生
	BeamBorn();
	//移動(フラグが1の時呼び出し)
	if (beamFlag_ == 1) {
		BeamMove();
	}
	//行列更新
	worldTransformBeam_.UpdateMatrix();
}
//ビーム発生
void GameScene::BeamBorn() {
	//フラグが1かつスペース押下時
	if (input_->PushKey(DIK_SPACE)&& beamFlag_ == 0) {
		//弾を自機の位置に
		worldTransformBeam_.translation_.x = worldTransformPlayer_.translation_.x;
		//弾を自機の手前に
		worldTransformBeam_.translation_.z = worldTransformPlayer_.translation_.z + 0.3f;
		//フラグを1
		beamFlag_ = 1;
	}
}
//ビーム移動
void GameScene::BeamMove() {
	//回転
	worldTransformBeam_.rotation_.x += 0.1f;
	//奥に移動(z軸)
	worldTransformBeam_.translation_.z += 0.5f;
	//z座標が40以上でフラグが0
	if (worldTransformBeam_.translation_.z >= 40) {
		beamFlag_ = 0;
	}
}

//エネミー更新
void GameScene::EnemyUpdate() { 
	//発生
	EnemyBorn();
	//移動(フラグが1の時呼び出し)
	if (enemyFlag_ == 1) {
		EnemyMove();
	}
	//行列更新
	worldTransformEnemy_.UpdateMatrix();
}
//エネミー発生
void GameScene::EnemyBorn() { 
	if (enemyFlag_ == 0) {
		//z座標40に発生
		worldTransformEnemy_.translation_.z = 40; 
		//乱数
		int x = rand() % 80;
		float x2 = (float)x / 10 - 4;
		//乱数を代入
		worldTransformEnemy_.translation_.x = x2;
		//フラグを1
		enemyFlag_ = 1;
	}
}
//エネミー移動
void GameScene::EnemyMove() { 
	//回転
	worldTransformEnemy_.rotation_.x -= 0.1f;
	//奥に移動(z軸)
	worldTransformEnemy_.translation_.z -= 0.3f;
	// z座標が-10以下でフラグが0
	if (worldTransformEnemy_.translation_.z <= -10) {
		enemyFlag_ = 0;
	}
}
