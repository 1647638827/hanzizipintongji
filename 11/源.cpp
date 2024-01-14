#include<iostream>
#include<iomanip>
#include<string>
#include<fstream>
#include<sstream>
#include<vector>
#include<cstring>

using namespace std;
template<class T>
class chinese_dict //汉字-->T 字典
{
protected:
	T* hashmap;
	const int hash(string key);
public:
	chinese_dict();
	chinese_dict(const int& size);
	T& operator[](const string& key);
};


template<class T>
const int chinese_dict<T>::hash(string key)
{
	unsigned char QWM[2] = { key[1] - 0xa0 ,key[0] - 0xa0 };
	int index = (QWM[1] - 16) * 94 + QWM[0] - 1;
	return index;
}

template<class T>
chinese_dict<T>::chinese_dict()
{
	hashmap = new T[6769];//[72]*[94]
	memset(hashmap, 0, sizeof(T) * 6769);
}

template<class T>
chinese_dict<T>::chinese_dict(const int& size)
{
	hashmap = new T[size];//[72]*[94]
	memset(hashmap, 0, sizeof(T) * size);
}

template<class T>
T& chinese_dict<T>::operator[](const string& key)
{
	return hashmap[hash(key)];
}



inline string load_file()
{
	string strin;
	ifstream fin;
	fin.open("./data.txt");
	if (!fin) {
		cout << "文件不存在！";
		exit(1);
	}
	stringstream f;
	f << fin.rdbuf();
	fin.close();

	strin = f.str();
	return strin;
}

#define coc character_of_chinese
class character_of_chinese {
public:

	string chara;
	int qwm;//区位码
	int counts;

	character_of_chinese(string c) :chara(c)
	{
		unsigned char QWM[2];
		QWM[0] = chara[1] - 0xa0;
		QWM[1] = chara[0] - 0xa0;
		qwm = QWM[1] * 100 + QWM[0];
		counts = 0;
	}
	character_of_chinese(const character_of_chinese& c2)
	{
		chara = c2.chara;
		qwm = c2.qwm;
		counts = c2.counts;
	}
	character_of_chinese operator=(const character_of_chinese& c2)
	{
		chara = c2.chara;
		qwm = c2.qwm;
		counts = c2.counts;
		return *this;
	}


	friend bool operator<(coc c1, coc c2) {
		return c1.qwm < c2.qwm;
	}

	friend bool operator>(coc c1, coc c2) {
		return c1.qwm > c2.qwm;
	}

};

inline bool save_file(const string& file_path, const vector<coc>& vc)
{
	ofstream fout;
	fout.open(file_path);
	if (!fout) 
		return false;
	for (int i = 0; i < vc.size(); i++) {
		//fout << vc[i].chara << " " << vc[i].counts << "     " << vc[i].qwm << endl;
		//cout << vc[i].chara << " " << vc[i].counts << "     " << vc[i].qwm << endl;
		fout << vc[i].chara << " " << vc[i].counts << endl;
		cout << vc[i].chara << " " << vc[i].counts << endl;
	}
	fout.close();
	return true;
}

inline bool save_file(const string& file_path, const vector<coc>& vc, chinese_dict<int>& mp)
{
	ofstream fout;
	fout.open(file_path);
	if (!fout) 
		return false;
	for (int i = 0; i < vc.size(); i++) {
		//fout << vc[i].chara << " " << mp[vc[i].chara] << "     " << vc[i].qwm << endl;
		//cout << vc[i].chara << " " << mp[vc[i].chara] << "     " << vc[i].qwm << endl;
		fout << vc[i].chara << " " << mp[vc[i].chara] << endl;
		cout << vc[i].chara << " " << mp[vc[i].chara] << endl;
	}
	fout.close();
	return true;

}

bool Counts(character_of_chinese c1, character_of_chinese c2)
{
	if (c1.counts == c2.counts) return c1.qwm > c2.qwm; // 同频时按照汉字区位码逆序

	return c1.counts >= c2.counts; //逆序 
}

bool Pinyin(character_of_chinese c1, character_of_chinese c2)
{
	return c1 < c2;
}

bool (*Maincmp)(coc, coc) = NULL;

void merge(vector<coc>& arr, int low, int mid, int high)
{
	int i = low, j = mid + 1;
	vector<coc> temp;
	while (i <= mid && j <= high)
	{
		if (Maincmp(arr[i], arr[j]))
			temp.push_back(arr[i++]);
		else
			temp.push_back(arr[j++]);
	}
	while (i <= mid)
	{
		temp.push_back(arr[i++]);
	}
	while (j <= high)
	{
		temp.push_back(arr[j++]);
	}

	//memcpy(&arr[low], &temp[0], temp.size() * sizeof(coc));
	copy(temp.begin(), temp.end(), arr.begin() + low);

	temp.clear();
}

void merge_sort_do(vector<coc>& arr, int low, int high)
{
	if (low < high)
	{
		int mid = (high + low) / 2;
		merge_sort_do(arr, low, mid);// 递归拆左边的序列
		merge_sort_do(arr, mid + 1, high);// 递归拆右边的序列
		merge(arr, low, mid, high);// 将两个有序的子序列排序合并成有序列
	}

}

void merge_sort(vector<coc>& arr, bool (*cmp)(coc, coc)) //归并排序
{
	Maincmp = cmp; //设置比较函数
	merge_sort_do(arr, 0, arr.size() - 1); //进入递归
}


int main()
{
	string s;
	chinese_dict<int> chara_dict;

	vector<coc> chara_lst_by_ori;
	vector<coc> chara_lst_by_count;
	vector<coc> chara_lst_by_Pinyin;

	s = load_file();

	cout << s;
	for (int i = 0; i < s.size() - 1; i += 2)
	{
		if (s[i] > 0)
		{
			i--;
			continue;
		}
		string tmp = s.substr(i, 2);
		coc coctmp(tmp);
		if (coctmp.qwm < 1601 || coctmp.qwm > 8794)
		{
			continue;
		}

		if (chara_dict[tmp] == 0)
		{
			chara_dict[tmp] = 1;
			chara_lst_by_ori.push_back(coctmp);
			chara_lst_by_Pinyin.push_back(coctmp);
			chara_lst_by_count.push_back(coctmp);
		}
		else
		{
			chara_dict[tmp]++;
		}
	}


	for (int i = 0; i < chara_lst_by_count.size(); i++)
	{
		chara_lst_by_count[i].counts = chara_dict[chara_lst_by_count[i].chara];
	}
	merge_sort(chara_lst_by_count, Counts);
	merge_sort(chara_lst_by_Pinyin, Pinyin);

	cout << "\n按汉字输入顺序排序：\n";
	save_file("./output_chara_lst_by_ori.txt", chara_lst_by_ori, chara_dict);
	cout << endl;
	cout << "按汉字拼音(区位码)顺序排序：\n";
	save_file("./output_chara_lst_by_Pinyin.txt", chara_lst_by_Pinyin, chara_dict);
	cout << endl;
	cout << "按汉字字频逆序排序(同频时按区位码顺序排序)：\n";
	save_file("./output_chara_lst_by_count.txt", chara_lst_by_count);
	cout << endl;

	
	while (true)
	{
		cout << "输入汉字以查询字频（输入n退出）：";
		cin >> s;
		if (s == "n") {
			break;
		}
		if (s[0] > 0) {
			cout << "请输入一个汉字。\n\n";
			continue;
		}
		if (s.length() != 2) {
			cout << "请输入一个汉字。\n\n";
			continue;
		}
		coc coctmp(s);
		if (coctmp.qwm < 1601 || coctmp.qwm > 8794) {
			cout << "请输入一个汉字。\n\n";
			continue;
		}
		cout << s << "   " << chara_dict[s] << "   区位码：" << coctmp.qwm << endl << endl;
	}



	return 0;
}