# include <Siv3D.hpp> // Siv3D v0.6.12

class MapChip
{
public:

	static constexpr int32 MapChipSize = 32;	// マップチップのサイズ

	MapChip()
		: m_base{U"../Assets/Tex/Tiles/WH_32x32.png"}{}

	// 指定したインデックスのマップチップを返す
	TextureRegion get(int32 chipIndex) const
	{
		//const int32 x = ((chipIndex % 8) * MapChipSize);
		//const int32 y = ((chipIndex / 8) * MapChipSize);

		//return m_base(x, y, MapChipSize);
		return m_base;
	}

private:

	Texture m_base;

};

// CSV ファイルから int32 の二次元配列を読み込む
Grid<int32> LoadCSV(FilePathView path) {

	CSV csv{ path };	// csv ファイルの読み込み

	// 読み込めなかった場合エラー
	if (not csv) {
		throw Error{ U"Failed to load `{}`"_fmt(path) };
	}

	const size_t yCount = csv.rows(); // 行数

	const size_t xCount = csv.columns(0);	// 列数

	Grid<int32> map(xCount, yCount);	// 二次元配列

	for (size_t y = 0; y < yCount; ++y) {
		for (size_t x = 0; x < xCount; ++x) {
			map[y][x] = csv.get<int32>(y, x);	// 指定した行・列の値を読み込む
		}
	}

	return map;
}

void Main()
{


	//constexpr int TILESIZE = 32;
	//constexpr int TILENUM = 10;
	//constexpr int WINDOWSIZE = TILESIZE*TILENUM;


	//Window::Resize(WINDOWSIZE, WINDOWSIZE);
	Window::Resize(1280, 960);

	MapChip mapChip;

	constexpr Size MapSize{ 20, 15 };

	Grid<int32> mapLayer0 = LoadCSV(U"../Assets/Maps/layer0.csv");		// 最下層
	Grid<int32> mapLayer1 = LoadCSV(U"../Assets/Maps/layer1.csv");		// 一段上
	Grid<int32> mapPassable = LoadCSV(U"../Assets/Maps/passable.csv");	// 通行可能

	// MapSize とロードしたデータのサイズが一致しない場合エラー
	if ((mapLayer0.size() != MapSize)
			|| (mapLayer1.size() != MapSize)
			|| (mapPassable.size() != MapSize)) {
		throw Error{ U"mapLayer0: {}, mapLayer1: {}, mapPassable: {}"_fmt(mapLayer0.size(), mapLayer1.size(), mapPassable.size())};
	}

	// プレイヤーのアニメーションテクスチャ
	const Texture playerTex{ U"../Assets/Tex/Tiles/BK_32x32.png" };

	// 今の座標
	Vec2 currentPos{320, 240};

	// 次の移動先座標
	Vec2 nextPos = currentPos;

	Vec2 moveDirection = { 0.0, 0.0 };

	constexpr double walkSpeed = 100;

	double walkProgress = 1.0;

	int32 direction = 0;

	bool canMove = true;
	bool isMove = false;

	RenderTexture renderTexture{ 640, 480 };

	InputGroup inputLeft = (KeyLeft | KeyA);
	InputGroup inputRight = (KeyRight | KeyD);
	InputGroup inputUp = (KeyUp | KeyW);
	InputGroup inputDown = (KeyDown | KeyS);

	//Scene::SetResizeMode(ResizeMode::Keep);


	// ウィンドウの閉じるボタンを押すか、System::Exit() を呼ぶのを終了操作に設定, エスケープキーを押しても終了しないようになる
	//System::SetTerminationTriggers(UserAction::CloseButtonClicked);

	//Scene::SetBackground(ColorF{ 0.0, 0.0, 0,0 });	// 背景色

	//Window::SetTitle(U"My Game");

	//double scond = 0;

	//Array<ColorF> color;

	/*
	color << HSV{  81.0, 0.47, 0.70 };
	color << HSV{  82.0, 0.40, 0.61 };
	color << HSV{ 155.0, 0.40, 0.43 };
	*/

	//for (int x = 0; x < TILENUM; x++) {
	//	for (int y = 0; y < TILENUM; y++) {
	//		color << HSV{ Random(80.0, 90.0), Random(0.40, 0.50), Random(0.50, 0.70) };
	//	}
	//}

	while (System::Update())
	{
		// ウィンドウタイトルに直近のフレームレートを表示
		//Window::SetTitle(Profiler::FPS());

		//ClearPrint();

		//scond += (Scene::DeltaTime());

		//int32 count = 0;
		//for (int dx = TILESIZE/2; dx < Scene::Size().x - TILESIZE/2; dx += TILESIZE) {
		//	for (int dy = TILESIZE/2; dy < Scene::Size().y - TILESIZE/2; dy += TILESIZE) {
		//		Rect{ dx, dy, TILESIZE }.draw(color[count]);
		//		Rect{ dx, dy, TILESIZE }.drawFrame(1.0, ColorF(0.5, 1.0));
		//		count++;
		//	}
		//}
		//DrawGrid(Scene::Size().x, Scene::Size().y, TILESIZE);

		renderTexture.clear(ColorF{ 0.0 });

		Vec2 moveDirection = { 0.0, 0.0 };

		if (canMove) {
			if (inputDown.pressed()) {
				++moveDirection.y;
				direction = 0;
				isMove = true;
			}
			else if (inputUp.pressed()) {
				--moveDirection.y;
				direction = 3;
				isMove = true;
			}
			if (inputLeft.pressed()) {
				--moveDirection.x;
				direction = 1;
				isMove = true;
			}
			else if (inputRight.pressed()) {
				++moveDirection.x;
				direction = 2;
				isMove = true;
			}


			//Print << (moveDirection.x / sqrt(moveDirection.x * moveDirection.x + moveDirection.y * moveDirection.y));
			if (isMove) {
				nextPos += moveDirection.normalized() * Scene::DeltaTime() * walkSpeed;
				Print << moveDirection.normalized() * Scene::DeltaTime() * walkSpeed;
				isMove = false;
			}


			nextPos.x = Clamp(nextPos.x, 0.0, (double)(renderTexture.size().x - MapChip::MapChipSize));
			nextPos.y = Clamp(nextPos.y, 0.0, (double)(renderTexture.size().y - MapChip::MapChipSize));

			//if (mapPassable[nextPos] == 1) {
			//	nextPos = currentPos;
			//}

			//if (currentPos != nextPos) {
			//	walkProgress -= 1.0;
			//}
			//else {
			//	walkProgress = 1.0;
			//}
		}

		//if (currentPos != nextPos) {
		//	walkProgress += (Scene::DeltaTime() * walkSpeed);

		//	if (1.0 <= walkProgress) {
		//		currentPos = nextPos;
		//	}
		//}

		//
		// 描画
		//
		{
			// renderTexture を描画先として設定
			const ScopedRenderTarget2D rt{ renderTexture };

			// スペースキーを押すと通行可能判定を可視化する
			const bool showPassable = KeySpace.pressed();

			// マップ
			for (int32 y = 0; y < MapSize.y; ++y) {
				for (int32 x = 0; x < MapSize.x; ++x) {
					// マップチップの描画座標
					const Point pos{ (x * MapChip::MapChipSize), (y * MapChip::MapChipSize) };

					// 最下層のマップチップ 0の場合描写しない
					if (const int32 chipIndex = mapLayer0[y][x];
							chipIndex != 0) {
						mapChip.get(chipIndex).draw(pos);
					}

					// 一段上のマップチップ
					if (const int32 chipIndex = mapLayer0[y][x];
							chipIndex != 0) {
						mapChip.get(chipIndex).draw(pos);
					}

					// 通行可能判定
					if ((mapPassable[y][x] != 0)) { // 通行不能の場合
						Rect{ pos, MapChip::MapChipSize }.draw(ColorF{ 0, 0, 1.0, 0.4 });
						// xマーク
						Shape2D::Cross((MapChip::MapChipSize * 0.3), 3, Vec2{ (MapChip::MapChipSize * (x + 0.5)), (MapChip::MapChipSize * (y + 0.5)) }).draw(Palette::Red);
					}
				}
			}

			// Player
			{
				//const Vec2 cellPos = currentPos.lerp(nextPos, walkProgress);
				//const Vec2 pos = (cellPos * MapChip::MapChipSize);
				const Vec2 pos = nextPos;
				playerTex.draw(pos);
				//Print << pos;
			}
		}

		{
			// テクスチャ拡大描画時にフィルタリング （滑らかな拡大処理）をしないサンプラーステートを適用
			const ScopedRenderStates2D sampler{ SamplerState::ClampNearest };

			// renderTexture を 2倍のサイズでシーンに描画
			renderTexture.scaled(2).draw();
		}
	}
}

