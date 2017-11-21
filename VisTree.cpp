//N頂点の有向ツリー(0-indexed)と、各ノードのメッセージを読み込むと、ツリーを表示してくれる。スケールと平行移動はZ,Xキーと十字キーで調整できる。
//N≦100, 1≦|S_i|≦100
//①まずは、平行移動・スケールについて考えずに設計する。
//②平行移動・スケール後の座標は、直接持たず（変換前座標, 平行移動量, 拡大量を変数で管理して）、toDrawPos()関数などで変換するようにする。
//③テキストの配置範囲を「画面サイズ」以外にできるようにする。（ノードが多くなったときを想定）

#include "DxLib.h"
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <complex>
#include <cmath>
using namespace std;

/*解析*/
template<typename T> void chmax(T &a, T b) {
	a = max(a, b);
}

int n;
vector<int> et[100];
string s[100];
int rootId;

void input() {
	ifstream ifs("treedata.txt");
	int in[100] = { 0 };

	ifs >> n;
	for (int i = 0; i < n - 1; i++) {
		int a, b;
		ifs >> a >> b;
		et[a].push_back(b);
		in[b]++;
	}
	for (int i = 0; i < n; i++) {
		ifs >> s[i];
		if (in[i] == 0) rootId = i;
	}
	ifs.close();
}

int maxDepth;	//深さの最大値
int width[100];	//width[深さ] = 頂点数
int depth[100];	//depth[頂点] = 深さ
int ord[100];	//ord[頂点v]  = 深さdepth[v]の頂点のなかで, vには何番目(0-indexed)に訪れたか？

void dfs(int v, int dep) {
	chmax(maxDepth, dep);
	depth[v] = dep;
	ord[v] = width[dep];
	width[dep]++;

	for (int i = 0; i < et[v].size(); i++) {
		dfs(et[v][i], dep + 1);
	}
}

/*拡大縮小平行移動*/
const int windowSizeX = 1000;
const int windowSizeY = 700;
double scale;					//拡大率
double Tx, Ty;					//平行移動（拡大前に図形を平行移動する）

void init_converter() {
	scale = 1;
	Tx = 0;
	Ty = 0;
}

void update_converter() {
	if (CheckHitKey(KEY_INPUT_Z)) {
		scale *= 0.98;
	}
	if (CheckHitKey(KEY_INPUT_X)) {
		scale /= 0.98;
	}

	if (abs(scale) >= 0.01) {
		if (CheckHitKey(KEY_INPUT_UP)) {
			Ty += 3 / scale;
		}
		if (CheckHitKey(KEY_INPUT_RIGHT)) {
			Tx -= 3 / scale;
		}
		if (CheckHitKey(KEY_INPUT_DOWN)) {
			Ty -= 3 / scale;
		}
		if (CheckHitKey(KEY_INPUT_LEFT)) {
			Tx += 3 / scale;
		}
	}
}

//拡大縮小平行移動はここで反映する
int to_drawX(int x) {
	return (x + Tx - windowSizeX / 2) * scale + windowSizeX / 2;
}

int to_drawY(int y) {
	return (y + Ty - windowSizeY / 2) * scale + windowSizeY / 2;
}


//ここから下のデータは基本的に「等倍平行移動なし」を仮定したものである。拡大や平行移動の反映には「to_drawX(), to_drawY()」関数を用いる。
/*テキスト*/
class Text {
protected:
	int lx, ly;
	string s;
public:
	Text() {

	}
	Text(int lx, int ly, string s) {
		this->lx = lx; this->ly = ly; this->s = s;
	}
	void draw() {
		DrawFormatString(lx, ly, 0, s.c_str());
	}
	int length() { return s.length(); }
};

class TextBox : public Text {
	int rx, ry;
public:
	TextBox() { }
	TextBox(int lx, int ly, int rx, int ry, string s) {
		this->lx = lx; this->ly = ly; this->s = s;
		this->rx = rx; this->ry = ry;
	}

	void draw() {
		int lx = to_drawX(this->lx);
		int ly = to_drawY(this->ly);
		int rx = to_drawX(this->rx);
		int ry = to_drawY(this->ry);
		//マイナス倍率に対応
		if (lx > rx) swap(lx, rx);
		if (ly > ry) swap(ly, ry);

		DrawBox(lx, ly, rx, ry, 0, FALSE);
		DrawFormatString(lx, ly, 0, s.c_str());
	}
};

TextBox text[100];
const int fontSize = 30;

void setLeftPos(int cx, int cy, int text_length, int &lx, int &ly) {
	int tx = -(double)fontSize * text_length / 2 / 1.75;
	int ty = -fontSize / 2;
	lx = cx + tx;
	ly = cy + ty;
}

void setRightPos(int cx, int cy, int text_length, int &rx, int &ry) {
	int tx = (double)fontSize * text_length / 2 / 1.75;
	int ty = fontSize / 2;
	rx = cx + tx;
	ry = cy + ty;
}

//配置範囲
int SizeY = windowSizeY;
int SizeX = windowSizeX;

void createText() {
	for (int v = 0; v < n; v++) {
		int cx = SizeX * (depth[v] + 1) / (maxDepth + 2);
		int cy = SizeY * (ord[v] + 1) / (width[depth[v]] + 1);
		int ly, lx, ry, rx;
		setLeftPos(cx, cy, s[v].length(), lx, ly);
		setRightPos(cx, cy, s[v].length(), rx, ry);
		text[v] = TextBox(lx, ly, rx, ry, s[v]);
	}
}

/*矢印*/
void DrawArrow(int x1, int y1, int x2, int y2, double len, int color, int thickness = 1) {
	typedef complex<double> Point;
	Point v = Point((double)x2 - x1, (double)y2 - y1); v /= abs(v);

	DrawLine(x1, y1, x2, y2, color, thickness);

	const double PAI = 3.14159265358979;
	Point a = len * v * exp(Point(0, PAI * 2 / 3));
	Point b = len * v * exp(Point(0, PAI * 4 / 3));

	double px[3];
	double py[3];
	px[0] = x2;
	px[1] = x2 + a.real();
	px[2] = x2 + b.real();
	py[0] = y2;
	py[1] = y2 + a.imag();
	py[2] = y2 + b.imag();
	DrawTriangle(px[0], py[0], px[1], py[1], px[2], py[2], color, TRUE);
}

class Arrow {
	int x1, y1, x2, y2, len, color, thickness;
public:
	Arrow() { }
	Arrow(int x1, int y1, int x2, int y2, int len, int color, int thickness = 1) {
		this->x1 = x1;
		this->y1 = y1;
		this->x2 = x2;
		this->y2 = y2;
		this->len = len;
		this->color = color;
		this->thickness = thickness;
	}
	void draw() {
		int x1 = to_drawX(this->x1);
		int y1 = to_drawY(this->y1);
		int x2 = to_drawX(this->x2);
		int y2 = to_drawY(this->y2);
		DrawArrow(x1, y1, x2, y2, (double)len, color, 1);
	}
};

vector<Arrow> arrow;

//矢印の生成（createText, Textクラスの実装に依存）
void createArrow() {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < et[i].size(); j++) {
			int src = i;
			int dst = et[i][j];

			//srcの右部
			int x1 = SizeX * (depth[src] + 1) / (maxDepth + 2) + text[src].length() * fontSize / 2 / 1.75;
			int y1 = SizeY * (ord[src] + 1) / (width[depth[src]] + 1);

			//dstの左部
			int x2 = SizeX * (depth[dst] + 1) / (maxDepth + 2) - text[dst].length() * fontSize / 2 / 1.75;
			int y2 = SizeY * (ord[dst] + 1) / (width[depth[dst]] + 1);

			arrow.push_back(Arrow(x1, y1, x2, y2, 10, 0));
		}
	}
}

/*描画など*/
void draw() {
	for (int v = 0; v < n; v++) {
		text[v].draw();
	}
	for (int i = 0; i < arrow.size(); i++) {
		arrow[i].draw();
	}
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR arg, int) {
	SetGraphMode(windowSizeX, windowSizeY, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	input();
	dfs(rootId, 0);
	createText();
	createArrow();
	init_converter();

	double beforeScale = -1;

	while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{
		update_converter();

		if (beforeScale != scale) {
			SetFontSize(abs(fontSize * scale));
			beforeScale = scale;
		}

		draw();
	}

	DxLib_End();
	return 0;
}