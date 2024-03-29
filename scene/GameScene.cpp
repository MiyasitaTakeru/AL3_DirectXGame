﻿#include "GameScene.h"
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
	//シーン
	delete spriteTitle_;
	//ステージ
	delete modelStage_;
	//スコア数値(スプライト)
	for (int n = 0; n < 5; n++) {
		delete spriteNumber_[n];
	}
	//スコアテキスト
	delete spriteScore_;
	//ライフ(残機)
	for (int l = 0; l < 3; l++) {
		delete spriteLife_[l];
	}
	//プレイヤー
	delete modelPlayer_;
	//ビーム
	delete modelBeam_;
	//エネミー
	delete modelEnemy_;
}

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
	textureHandleStage_ = TextureManager::Load("stage2.jpg");
	modelStage_ = Model::Create();
	//ステージ
	for (int i = 0; i < 20; i++) {
		worldTransformStage_[i].translation_ = {0, -1.5f, 2.0f * i - 5};
		worldTransformStage_[i].scale_ = {4.5f, 1, 1};
		worldTransformStage_[i].Initialize();
	}

	//スコア数値(スプライト)
	textureHandleNumber_ = TextureManager::Load("number.png");
	for (int n = 0; n < 5; n++) {
		//表示位置と文字間隔
		spriteNumber_[n] = Sprite::Create(textureHandleNumber_, {250.0f + n * 26, 0});
	}
	//スコアテキスト
	textureHandleScore_ = TextureManager::Load("score.png");
	spriteScore_ = Sprite::Create(textureHandleScore_, {100, 0});

	//ライフ(残機)
	textureHandleLife_ = TextureManager::Load("player.png");
	for (int l = 0; l < 3; l++) {
		//表示位置と文字間隔
		spriteLife_[l] = Sprite::Create(textureHandleLife_, {1000.0f + l * 50, 10});
	}

	//タイトル
	textureHandleTitle_ = TextureManager::Load("title.png");
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {0, 0});
	//ゲームオーバー
	textureHandleGameOver_ = TextureManager::Load("gameover.png");
	spriteGameOver_ = Sprite::Create(textureHandleGameOver_, {0, 0});

	//エンターキー
	textureHandleEnter_ = TextureManager::Load("enter.png");
	spriteEnter_ = Sprite::Create(textureHandleEnter_, {400, 500});

	//プレイヤー
	textureHandlePlayer_ = TextureManager::Load("Player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();
	
	//ビーム
	textureHandleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();
	for (int b = 0; b < 10; b++) {
		worldTransformBeam_[b].scale_ = {0.3f, 0.3f, 0.3f};
		worldTransformBeam_[b].Initialize();
	}
	//エネミー
	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();
	for (int e = 0; e < 10; e++) {
		worldTransformEnemy_[e].scale_ = {0.5f, 0.5f, 0.5f};
		//発生のための乱数の種(シード)
		srand(time(nullptr));
		worldTransformEnemy_[e].Initialize();
	}

	//サウンドデータ読み込み
	soundDataHandleTitleBGM_ = audio_->LoadWave("Audio/Ring05.wav");
	soundDataHandleGamePlay_ = audio_->LoadWave("Audio/Ring08.wav");
	soundDataHandleGameOver_ = audio_->LoadWave("Audio/Ring09.wav");
	soundDataHandleEnemyHitSE_ = audio_->LoadWave("Audio/chord.wav");
	soundDataHandlePlayerHitSE_ = audio_->LoadWave("Audio/tada.wav");
	//タイトルBGMを再生(trueでループ)
	voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);
}


//ゲームプレイ更新
void GameScene::GamePlayUpdate() {
	StageUpdate();
	PlayerUpdate();
	EnemyUpdate();
	BeamUpdate();
	Collision();
	CollisionPlayerEnemy();
	CollisionBeamEnemy();
	//デバッグ
	/*std::string beamDebug = std::string("beamFlag:") + std::to_string(beamFlag_);
	debugText_->Print(beamDebug, 50, 50, 1.0f);
	std::string enemyDebug = std::string("enemyFlag:") + std::to_string(enemyFlag_);
	debugText_->Print(enemyDebug, 50, 60, 1.0f);*/
	/*std::string z = std::string("z:") + std::to_string(worldTransformBeam_.translation_.z);
	debugText_->Print(z, 50, 600, 1.0f);*/
}
//ゲームプレイ3D描画
void GameScene::GamePlayDraw3D() {
	//ステージ
	for (int i = 0; i < 20; i++) {
		modelStage_->Draw(worldTransformStage_[i], viewProjection_, textureHandleStage_);
	}
	//プレイヤー
	if (playerTimer_ % 4 < 2) {
		modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	}
	//ビーム(フラグが1の時)
	for (int b = 0; b < 10; b++) {
		if (beamFlag_[b] == 1) {
			modelBeam_->Draw(worldTransformBeam_[b], viewProjection_, textureHandleBeam_);
		}
	}
	//エネミー(フラグが1の時)
	for (int e = 0; e < 10; e++) {
		if (enemyFlag_[e] != 0) {
			modelEnemy_->Draw(worldTransformEnemy_[e], viewProjection_, textureHandleEnemy_);
		}
	}
}
//ゲームプレイ背景描画
void GameScene::GamePlayDraw2DBack() {
	//背景
	spriteBG_->Draw();
}
//ゲームプレイ前景描画
void GameScene::GamePlayDraw2DNear() {
	//スコア
	DrawScore();
	/*char score[100];
	sprintf_s(score, "SCORE : %d", gameScore_);
	debugText_->Print(score, 200, 10, 2);*/
	//ライフ
	DrawLife();
	/*char life[100];
	sprintf_s(life, "LIFE : %d", playerLife_);
	debugText_->Print(life, 1000, 10, 2);*/
}

//更新
void GameScene::Update() { 
	switch (sceneMode_) {
		case 0:
		GamePlayUpdate();
		    break;
		case 1:
		    TitleUpdate();
		    enterTimer_++;
		    break;
	    case 2:
		    GameOverUpdate();
		    enterTimer_++;
		    break;
	}
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
	/// </summary>
	switch (sceneMode_) {
	case 0:
		GamePlayDraw2DBack();
		break;
	case 2:
		GamePlayDraw2DBack();
		break;
	}


	// スプライト描画後処理
	Sprite::PostDraw();

	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region オブジェクト描画3D
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	switch (sceneMode_) {
	case 0:
		GamePlayDraw3D();
		break;
	case 2:
		GamePlayDraw3D();
		break;
	}

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
	switch (sceneMode_) {
	case 0:
		GamePlayDraw2DNear();
		debugText_->DrawAll(commandList);
		break;
	case 1:
		//タイトル表示
		TitleDraw2DNear();
		break;
	case 2:
		//ゲームオーバー表示
		GameOverDraw2DNear();
		break;
	}
	
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

//------------------関数定義------------------//
//ゲーム初期化
void GameScene::GamePlayStart() { 
	sceneMode_ = 0;
	enterTimer_ = 0;
	gameScore_ = 0;
	gameTimer_ = 0;
	playerLife_ = 3;
	playerTimer_ = 0;
	for (int b = 0; b < 10; b++) {
		beamFlag_[b] = 0;
	}
	for (int e = 0; e < 10; e++) {
		enemyFlag_[e] = 0;
	}
	//プレイヤー初期位置
	worldTransformPlayer_.translation_.x = 0;
	worldTransformPlayer_.translation_.y = 0;
}

//タイトル更新
void GameScene::TitleUpdate() { 
	//エンターキーを押したら
	if (input_->TriggerKey(DIK_RETURN)) {
		//ゲームプレイを初期化
		GamePlayStart();
		//モードをゲームプレイに変更
		sceneMode_ = 0;
		//BGM切り替え
		//BGMを停止
		audio_->StopWave(voiceHandleBGM_);
		//ゲームプレイBGMを再生(trueでループ)
		voiceHandleBGM_ = audio_->PlayWave(soundDataHandleGamePlay_, true);
	}
}
//タイトル描画
void GameScene::TitleDraw2DNear() { 
	spriteTitle_->Draw(); 
	//エンター点滅
	if (enterTimer_ % 60 <= 20) {
		spriteEnter_->Draw();
	}
}
//ゲームオーバー更新
void GameScene::GameOverUpdate() {
	//エンターキーを押したら
	if (input_->TriggerKey(DIK_RETURN)) {
		//モードをタイトル
		sceneMode_ = 1;
		// BGMを停止
		audio_->StopWave(voiceHandleBGM_);
		//タイトルBGMを再生(trueでループ)
		voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);
	}
}
//ゲームオーバー描画
void GameScene::GameOverDraw2DNear() {
	spriteGameOver_->Draw();
	//エンター点滅
	if (enterTimer_ % 60 <= 20) {
		spriteEnter_->Draw();
	}
}

//スコア表示
void GameScene::DrawScore() {
	//テキスト表示
	spriteScore_->Draw();

	//各桁の値を取り出す
	char eachNumber[5] = {};
	int number = gameScore_;

	int digit = 10000;
	for (int n = 0; n < 5; n++) {
		eachNumber[n] = number / digit;
		number = number % digit;
		digit = digit / 10;
	}
	//各桁の数値を描画
	for (int n = 0; n < 5; n++) {
		spriteNumber_[n]->SetSize({32,64});
		spriteNumber_[n]->SetTextureRect({32.0f * eachNumber[n], 0}, {32, 64});
		spriteNumber_[n]->Draw();
	}
}

//ライフ(残機)
void GameScene::DrawLife() {
	//ライフ分だけ表示
	for (int l = 0; l < playerLife_; l++) {
		//サイズは40
		spriteLife_[l]->SetSize({40, 40});
		spriteLife_[l]->Draw();
	}
}

//ステージ更新
void GameScene::StageUpdate() { 
	//各ステージでループ
	for (int i = 0; i < 20; i++) {
		//手前へ移動
		worldTransformStage_[i].translation_.z -= 0.1f;
		//端まで来たら戻る
		if (worldTransformStage_[i].translation_.z < -5) {
			worldTransformStage_[i].translation_.z += 40;
		}
		//行列の更新
		worldTransformStage_[i].UpdateMatrix();
	}
}

//プレイヤー更新
void GameScene::PlayerUpdate() {
	//x座標が3.5f以下の場合右へ移動(移動制限)
	if (input_->PushKey(DIK_RIGHT) && worldTransformPlayer_.translation_.x <= 3.5f) {
		worldTransformPlayer_.translation_.x += 0.15f;
	}
	//x座標が-3.5f以上の場合左へ移動(移動制限)
	if (input_->PushKey(DIK_LEFT) && worldTransformPlayer_.translation_.x >= -3.5f) {
		worldTransformPlayer_.translation_.x -= 0.15f;
	}
	//プレイヤーライフが0でゲームオーバーへ
	if (playerLife_ <= 0) {
		sceneMode_ = 2;
		// BGMを停止
		audio_->StopWave(voiceHandleBGM_);
		//ゲームオーバーBGMを再生(trueでループ)
		voiceHandleBGM_ = audio_->PlayWave(soundDataHandleGameOver_, true);
	}
	//衝突時プレイヤー点滅
	if (playerTimer_ >= 0) {
		playerTimer_--;
	}
	//行列更新
	worldTransformPlayer_.UpdateMatrix();
}

//ビーム更新
void GameScene::BeamUpdate() {
	//発生
	BeamBorn();
	BeamMove();
	for (int b = 0; b < 10; b++) {
		//行列更新
		worldTransformBeam_[b].UpdateMatrix();
	}
}
//ビーム発生
void GameScene::BeamBorn() {
	for (int b = 0; b < 10; b++) {
		//発射タイマーが0なら
		if (beamTimer_ == 0) {
			//フラグが1かつスペース押下時
			if (input_->PushKey(DIK_SPACE) && beamFlag_[b] == 0) {
				//弾を自機の位置に
				worldTransformBeam_[b].translation_.x = worldTransformPlayer_.translation_.x;
				//弾を自機の手前に
				worldTransformBeam_[b].translation_.z = worldTransformPlayer_.translation_.z + 0.3f;
				//フラグを1
				beamFlag_[b] = 1;
				//タイマーを10
				beamTimer_ = 10;
			}
		} else {
			beamTimer_--;
			if (beamTimer_ <= 0) {
				beamTimer_ = 0;
			}
			break;
		}
	}
}
//ビーム移動
void GameScene::BeamMove() {
	for (int b = 0; b < 10; b++) {
		//回転
		worldTransformBeam_[b].rotation_.x += 0.1f;
		//奥に移動(z軸)
		worldTransformBeam_[b].translation_.z += 0.5f;
		// z座標が40以上でフラグが0
		if (worldTransformBeam_[b].translation_.z >= 40) {
			beamFlag_[b] = 0;
		}
	}
}

//エネミー更新
void GameScene::EnemyUpdate() { 
	//発生
	EnemyBorn();
	//移動
	EnemyMove();
	//ジャンプ
	EnemyJump();
	for (int e = 0; e < 10; e++) {
		//行列更新
		worldTransformEnemy_[e].UpdateMatrix();
	}
}
//エネミー発生
void GameScene::EnemyBorn() {
	//1~10までの間にランダムに発生
	if (rand() % 10 == 0) {
		for (int e = 0; e < 10; e++) {
			//フラグが0なら
			if (enemyFlag_[e] == 0) {
				//左右のスピードを初期化(乱数で2/1でどちらかにする)
				if (rand() % 2 == 0) {
					enemySpeed_[e] = 0.2f;
				} else {
					enemySpeed_[e] = -0.2f;
				}
				//z座標40に発生
				worldTransformEnemy_[e].translation_.z = 40;
				//乱数
				int x = rand() % 80;
				float x2 = (float)x / 10 - 4;
				//乱数を代入
				worldTransformEnemy_[e].translation_.x = x2;
				//エネミーのy座標を0
				worldTransformEnemy_[e].translation_.y = 0;
				//フラグを1
				enemyFlag_[e] = 1;
				break;
			}
		}
	}
}
//エネミー移動
void GameScene::EnemyMove() { 
	for (int e = 0; e < 10; e++) {
		if (enemyFlag_[e] == 1) {
			//回転
			worldTransformEnemy_[e].rotation_.x -= 0.1f;
			//手前に移動(z軸)
			worldTransformEnemy_[e].translation_.z -= 0.3f;
			//横に移動(x軸)
			worldTransformEnemy_[e].translation_.x += enemySpeed_[e];
			//x座標が5または-5で反転
			if (worldTransformEnemy_[e].translation_.x >= 5) {
				enemySpeed_[e] = -0.2f;
			}
			if (worldTransformEnemy_[e].translation_.x <= -5) {
				enemySpeed_[e] = 0.2f;
			}
			//z座標が-10以下でフラグが0
			if (worldTransformEnemy_[e].translation_.z <= -10) {
				enemyFlag_[e] = 0;
			}
		}
	}
}
//エネミージャンプ
void GameScene::EnemyJump() { 
	//敵でループ
		for (int e = 0; e < 10; e++) {
			//消滅演出なら
			if (enemyFlag_[e] == 2) {
				//移動(y座標に速度を加える)
				worldTransformEnemy_[e].translation_.y += enemyJumpSpeed_[e];
				//速度を減らす
				enemyJumpSpeed_[e] -= 0.1f;
				//斜め移動
			    worldTransformEnemy_[e].translation_.x += enemySpeed_[e] * 2;
				//下へ落ちると消滅
				if (worldTransformEnemy_[e].translation_.y < -3) {
					//存在しない
					enemyFlag_[e] = 0;
				}
			}
		}
}

//衝突判定
void GameScene::Collision() { 
	//衝突判定(プレイヤーと敵)
	CollisionPlayerEnemy(); 
}
//衝突判定(プレイヤーと敵)
void GameScene::CollisionPlayerEnemy() {
	for (int e = 0; e < 10; e++) {
		//敵が存在したら
		if (enemyFlag_[e] == 1) {
			float dx =
			  abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_[e].translation_.x);
			float dz =
			  abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_[e].translation_.z);
			//衝突したら
			if (dx < 1 && dz < 1) {
				playerLife_ -= 1;
				enemyFlag_[e] = 0;
				//プレイヤー点滅タイマー
				playerTimer_ = 60;
				//プレイヤーヒットSEを再生(falseまたは指定なしでループしない)
				audio_->PlayWave(soundDataHandlePlayerHitSE_, false,2.0f);
			}
		}
	}
}
//衝突判定(ビームと敵)
void GameScene::CollisionBeamEnemy() {
	for (int e = 0; e < 10; e++) {
		for (int b = 0; b < 10; b++) {
			//敵と弾が存在したら
			if (enemyFlag_[e] == 1 && beamFlag_[b] == 1) {
				float dx = abs(
				  worldTransformBeam_[b].translation_.x - worldTransformEnemy_[e].translation_.x);
				float dz = abs(
				  worldTransformBeam_[b].translation_.z - worldTransformEnemy_[e].translation_.z);
				//衝突したら
				if (dx < 1 && dz < 1) {
					gameScore_ += 100;
					enemyFlag_[e] = 2;
					beamFlag_[b] = 0;
					//ジャンプ速度を1
					enemyJumpSpeed_[e] = 1;
					//エネミーヒットSEを再生(falseまたは指定なしでループしない)
					audio_->PlayWave(soundDataHandleEnemyHitSE_, false,2.0f);
				}
			}
		}
	}
}