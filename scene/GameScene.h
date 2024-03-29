﻿#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "DebugText.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <DirectXMath.h>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

  public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//ゲーム初期化
	void GamePlayStart();

	//タイトル
	void TitleUpdate();
	void TitleDraw2DNear();
	//ゲームオーバー
	void GameOverUpdate();
	void GameOverDraw2DNear();

	//スコア表示
	void DrawScore();

	//ライフ(残機)
	void DrawLife();

	//ステージ更新
	void StageUpdate();

	//プレイヤー更新
	void PlayerUpdate();

	//ビーム更新
	void BeamUpdate();
	//ビーム移動
	void BeamMove();
	//ビーム発生
	void BeamBorn();
	
	//エネミー更新
	void EnemyUpdate();
	//エネミー発生
	void EnemyBorn();
	//エネミー移動
	void EnemyMove();
	//エネミージャンプ
	void EnemyJump();

	//衝突判定
	void Collision();
	//衝突判定(プレイヤーと敵)
	void CollisionPlayerEnemy();
	//衝突判定(ビームと敵)
	void CollisionBeamEnemy();

	//ゲームプレイ更新
	void GamePlayUpdate();
	//ゲームプレイ3D表示
	void GamePlayDraw3D();
	//ゲームプレイ背景2D表示
	void GamePlayDraw2DBack();
	//ゲームプレイ近景2D表示
	void GamePlayDraw2DNear();


	//ゲームスコア
	int gameScore_ = 0;
	//ゲームタイマー
	int gameTimer_ = 0;

	//プレイヤーライフ
	int playerLife_ = 3;
	//プレイヤータイマー(衝突時点滅)
	int playerTimer_ = 0;

	//シーン( 0 : ゲームプレイ / 1 : タイトル / 2 : ゲームオーバー)
	int sceneMode_ = 1;
	//エンターキー点滅
	int enterTimer_ = 0;

	//ビーム発射タイマー
	int beamTimer_ = 0;

	//敵のスピード
	float enemySpeed_[10] = {};
	//敵のジャンプ
	float enemyJumpSpeed_[10] = {};

  private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	
	//BG(スプライト)
	uint32_t textureHandleBG_ = 0;
	Sprite* spriteBG_ = nullptr;

	//ビュープロジェクション(共通)
	ViewProjection viewProjection_;

	//スコア数値(スプライト)
	uint32_t textureHandleNumber_ = 0;
	Sprite* spriteNumber_[5] = {};
	//スコアテキスト
	uint32_t textureHandleScore_ = 0;
	Sprite* spriteScore_ = nullptr;

	//ライフ(残機)
	uint32_t textureHandleLife_ = 0;
	Sprite* spriteLife_[3] = {};

	//タイトル
	uint32_t textureHandleTitle_ = 0;
	Sprite* spriteTitle_ = nullptr;

	//ゲームオーバー
	uint32_t textureHandleGameOver_ = 0;
	Sprite* spriteGameOver_ = nullptr;

	//エンターキー
	uint32_t textureHandleEnter_ = 0;
	Sprite* spriteEnter_ = nullptr;

	//ステージ
	uint32_t textureHandleStage_ = 0;
	Model* modelStage_ = nullptr;
	WorldTransform worldTransformStage_[20];

	//プレイヤー
	uint32_t textureHandlePlayer_ = 0;
	Model* modelPlayer_ = nullptr;
	WorldTransform worldTransformPlayer_;

	//ビーム
	uint32_t textureHandleBeam_ = 0;
	Model* modelBeam_ = nullptr;
	WorldTransform worldTransformBeam_[10];
	//ビーム存在フラグ
	int32_t beamFlag_[10] = {};

	//エネミー
	uint32_t textureHandleEnemy_ = 0;
	Model* modelEnemy_ = nullptr;
	WorldTransform worldTransformEnemy_[10];
	//エネミー存在フラグ
	uint32_t enemyFlag_[10] = {};

	//サウンド
	uint32_t soundDataHandleTitleBGM_ = 0;
	uint32_t soundDataHandleGamePlay_ = 0;
	uint32_t soundDataHandleGameOver_ = 0;
	uint32_t soundDataHandleShotSE_ = 0;
	uint32_t soundDataHandleEnemyHitSE_ = 0;
	uint32_t soundDataHandlePlayerHitSE_ = 0;
	uint32_t voiceHandleBGM_ = 0;
};
