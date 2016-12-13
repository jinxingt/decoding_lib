#ifndef __CS_CONFIG__H__
#define __CS_CONFIG__H__

#include "CS_Common.h"

// add class of CS_CodingScheme
enum CSCodingSchemeType : char{
	CSCodingSchemeTypeRDP = 0,
	CSCodingSchemeTypePIT = 1,
	CSCodingSchemeTypeEVENODD = 2,
};

struct CS_CodingSchemeKey
{
	//以下几个变量唯一决定CodingScheme
	CSCodingSchemeType type;
	int row;
	int k;
	int m;
	int shorten_row;
	int shorten_col;

	//额外的参数
	int algorithm;
	bool is_single_fail;
	bool is_default;

	bool operator==(const CS_CodingSchemeKey& rhs) const{
		return type == rhs.type && row == rhs.row && k == rhs.k &&m == rhs.m && shorten_row == rhs.shorten_row && shorten_col == rhs.shorten_col;
	}

	CS_CodingSchemeKey(CSCodingSchemeType type, int row, int k, int m, int shorten_row, int shorten_col, int algorithm, bool is_single_fail = false, bool is_default = false) :
		type(type), row(row), k(k), m(m), shorten_row(shorten_row), shorten_col(shorten_col), algorithm(algorithm), is_single_fail(is_single_fail), is_default(is_default){

	}
	CS_CodingSchemeKey(){}
	void print(){
		LogDebug("type = %d, row=%d, k=%d, m=%d, shorten_row=%d, shorten_col=%d, algorithm=%d, is_single_fail=%d, is_default=%d\n",
			type, row, k, m, shorten_row, shorten_col, algorithm, is_single_fail, is_default);
	}
};
struct CS_CodingSchemeKeyHasher
{
	size_t operator() (const CS_CodingSchemeKey& k) const{
		return hash<int>()(k.type | (k.row << 8) | (k.k << 16) | (k.shorten_row << 24)) ^
			hash<int>()(k.shorten_col | (k.m << 8));
	}
};

struct CS_CodingSchemeRecoveryKey
{
	CS_CodingSchemeRecoveryKey(vector<int>& cols, int algorithm) : cols(cols), algorithm(algorithm){}
	vector<int> cols;
	int algorithm;

	bool operator == (const CS_CodingSchemeRecoveryKey& rhs) const {
		return (cols == rhs.cols) && (algorithm == rhs.algorithm);
	}
};
struct CS_CodingSchemeRecoveryKeyHasher
{
	size_t operator() (const CS_CodingSchemeRecoveryKey& k) const{
		size_t h = k.algorithm;
		for (int i = 0; i < k.cols.size(); ++i) h ^= hash<int>()(k.cols[i] << 8);
		return h;
	}
};


class CS_Config
{
public:
	CS_Config(){
		int res = createNetFromFile(configDir);
		if (res != 0){
			LogError("read coding scheme config failed\n")
		}
	}
public:
	int createNetFromFile(const char* fileName){
		// 解析json用Json::Reader
		Json::Reader reader;
		// Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array...
		Json::Value root;

		std::ifstream is;
		is.open(fileName, std::ios::binary);
		if (!reader.parse(is, root)){
			LogError("Json parser failed")
			is.close();
			return -1;
		}

		if (root["code_schemes"].isNull()){
			LogNotice("json no iterm of (code_schemes)");
			is.close();
			return 0;
		}

		// 遍历数组
		int file_size = root["code_schemes"].size();
		Json::Value layers = root["code_schemes"];
		if (!layers.isArray()){
			LogNotice("json (code_schemes) config is not array");
			return 0;
		}
		for (int i = 0; i < file_size; ++i)
		{
			//Json::Value val_image = root["files"][i]["images"];
			//std::string type = val_image[j]["type"].asString();
			Json::Value layerJson = layers[i];

			CS_CodingSchemeKey csk;
			csk.type = (CSCodingSchemeType)(layerJson["type"].asInt());
			csk.row = layerJson["row"].asInt();
			csk.k = layerJson["k"].asInt();
			csk.m = layerJson["m"].asInt();
			csk.shorten_row = layerJson["shorten_row"].asInt();
			csk.shorten_col = layerJson["shorten_col"].asInt();
			csk.algorithm = layerJson["algorithm"].asInt();
			csk.is_single_fail = layerJson["is_single_fail"].asBool();
			csk.is_default = layerJson["is_default"].asBool();
			
			coding_schemes.push_back(csk);
		}

		is.close();
		return 0;
	}
public:
	vector<CS_CodingSchemeKey> coding_schemes;
};

#endif//__CS_CONFIG__H__