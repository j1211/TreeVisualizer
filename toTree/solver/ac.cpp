#include <iostream>
#include <string>
#include <vector>
using namespace std;

int n;
string s[100];
int depth[100];
vector<int> et[100];

int getDepth(string s) {
	for (int i = 0; i < s.length(); i++) if (s[i] != ' ') return i;
	return s.length();
}

vector<int> getId(int l, int r, int dep) {
	vector<int> ret;
	for (int i = l; i < r; i++) if (depth[i] == dep) ret.push_back(i);
	return ret;
}

int main() {
	cin >> n;

	getline(cin, s[0]);	//trush
	for (int i = 0; i < n; i++) {
		getline(cin, s[i]);
	}
	
	for (int i = 0; i < n; i++) depth[i] = getDepth(s[i]);
	
	vector<int> ids[100];
	ids[0].push_back(0);
	
	for (int dep = 0; ; dep++) {
		if (ids[dep].size() == 0) break;
		for (int i = 0; i < ids[dep].size(); i++) {
			int l = ids[dep][i], r;
			if (i + 1 == ids[dep].size()) { r = n; }
			else { r = ids[dep][i + 1]; }
			et[l] = getId(l, r, dep + 1);
			for (int j = 0; j < et[l].size(); j++) ids[dep + 1].push_back(et[l][j]);
		}
	}
	
	cout << n << endl;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < et[i].size(); j++) {
			cout << i << " " << et[i][j] << endl;
		}
	}
	for (int i = 0; i < n; i++) {
		cout << s[i] << endl;
	}
	return 0;
}