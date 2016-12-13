/*
 * GetPrivateProfileString 用法
 * 函数原型：int GetPrivateProfileString(section, key, sdefault, buffer, len, filename)
 * 返回： 返回配置文件中读取出来的buffer字符串长度
 *   1)、当section为 NULL 时，返回配置文件中所有的section的名称，以一个0长度的串结束
 *   2)、当key为 NULL 时，返回配置文件中该section下的所有key的名称，以一个0长度的串结束
 *   3)、当section.key指定的配置不存在时，返回sdefault值内容
 *   4)、buffer用于返回读取的配置内容，当len小于实际内容时，该buffer含'\0'结束符
 *   5)、返回的value值，将裁剪前后空白符，除非用单引号或双引号强制指定
 * 以下情况与windows.api结果不一致
 *   1)、当指定获取section下所有keys时，如果未匹配到section或无key时
 *       windows下返回默认值(sdefault)
 *       本实现返回空串
 *   2)、注释行
 *       windows在每行第一个非空白符为";"时表示该被注释
 *       本实现仅在返回keys时，才定义注释行，并支持"#"、"!"、";"
 *   3)、windows下不支持以";"为前缀的key定义。本实现支持
 *
 * GetPrivateProfileInt 用法
 * 函数原型：int GetPrivateProfileInt(section, key, ndefault, filename)
 * 返回：当section.key存在，并且内容不为空时，返回实际值
 *       其它情况返回ndefalut值
 *
 * WritePrivateProfileString 用法
 * 函数原型: bool WritePrivateProfileString(section, key, value, filename)
 * 返回：成功返回true，其它返回false
 *   1)、当key为 NULL 时，删除配置文件中的section指定段
 *   2)、当value为 NULL 时，删除配置文件中的key配置行
 *   3)、其它情况，写入section.key.value实际值
 *
 * WritePrivateProfileInt 用法
 * 函数原型: bool WritePrivateProfileInt(section, key, value, filename)
 * 用法与 WritePrivateProfileString 完全相同
 */

#ifndef _WIN32

#include <string>
#include <string.h>
#include <list>
#include <assert.h>

/*检查字符串是否为section*/
static bool match_section(const char *str, const char *section)
{
	while (isspace(*str) && *str)
		str++;

	if (*str++ != '[')
		return false;
	
	while (isspace(*str) && *str)
		str++;

	if (strncasecmp(str, section, strlen(section)) != 0)
		return false;
	
	str += strlen(section);
	
	while (isspace(*str) && *str)
		str++;

	return (*str == ']');
}

/*检查字符串是否为section*/
static bool match_key(const char *str, const char *key)
{
	while (isspace(*str) && *str)
		str++;
	
	if (strncasecmp(str, key, strlen(key)) != 0)
		return false;
	
	str += strlen(key);

	while (isspace(*str) && *str)
		str++;
	
	return (*str == '=');
}

//返回的字符串(dest)尾部填充有两个'\0', 与windows一致
static int get_sections(char* dest, int size, const char* filename)
{
	//异常处理
	assert(dest);
	if (size < 2)
	{
		*dest++ = 0;
		return 0;
	}

	FILE *fp = fopen(filename, "r");
	if (fp == NULL)
	{
		*dest++ = 0;
		*dest++ = 0;
		return 0;
	}

	//扫描sections
	const char* orig = dest;

	const int const_buffer_length = 1024;
	char buffer[const_buffer_length] = "";
	
	while (size >= 2)
	{
		if (fgets(buffer, const_buffer_length, fp) == NULL)
		{
			*dest++ = 0;
			size--;
			break;
		}

		char *pos = buffer;
		while (isspace(*pos) && *pos)
			pos++;
		if (*pos != '[')
			continue;
		pos++;
		
		char *end = strchr(pos, ']');
		if (end == NULL)
			continue;
		
		while (isspace(*pos) && pos != end)
			pos++;
		while (isspace(*(end-1)) && end != pos)
			end--;
		
		assert(end >= pos);
		//*end = 0;
		
		int length = end - pos;
		if (length > size - 2)
			length = size - 2;
		strncpy(dest, pos, length);
		size -= length;
		dest += length;
		
		*dest++ = 0;
		size--;
	}
	
	fclose(fp);
	*dest++ = 0;
	return (dest - orig - 2);
}

//返回的字符串(dest)尾部填充有两个'\0', 与windows一致
//windows下当section或key不存在时，将返回默认串，这儿不实现
static int get_section_keys(const char* section, char* dest, int size, const char* filename)
{
	assert(section && dest);
	if (size < 2)
	{
		*dest++ = 0;
		return 0;
	}
	
	FILE *fp = fopen(filename, "r");
	if (fp == NULL)
	{
		*dest++ = 0;
		*dest++ = 0;
		return 0;
	}
	
	//扫描section.keys
	const char* orig = dest;

	const int const_buffer_length = 1024;
	char buffer[const_buffer_length] = "";

	//////先检查section出现的位置
	while (fgets(buffer, const_buffer_length, fp))
	{
		if (match_section(buffer, section))
			break;
	}
	
	while (size >= 2)
	{
		if (fgets(buffer, const_buffer_length, fp) == NULL)
		{
			*dest++ = 0;
			size--;
			break;
		}

		char *pos = buffer;
		while (isspace(*pos) && *pos)
			pos++;
		if (*pos == '[')
			break;

		if (*pos == ';' || *pos == '#' || *pos == '!')
			continue;

		char *end = strchr(pos, '=');
		if (end == NULL)
			continue;
		
		while (isspace(*(end-1)) && end != pos)
			end--;
		
		assert(end >= pos);
		//*end = 0;
		
		int length = end - pos;
		if (length > size - 2)
			length = size - 2;
		strncpy(dest, pos, length);
		size -= length;
		dest += length;
		
		*dest++ = 0;
		size--;
	}
	
	fclose(fp);
	*dest++ = 0;
	return (dest - orig - 2);
}

static int get_section_key_value(const char* section, const char* key, const char* sdefault, char *dest, int size, const char* filename)
{
	assert(section && key && dest);
	if (sdefault == NULL)
		sdefault = "";
	
	//将默认值拷贝到目标地址
	int ndefaut = strlen(sdefault);
	if (ndefaut >= size)
		ndefaut = size - 1;
	strncpy(dest, sdefault, ndefaut);
	dest[ndefaut] = 0;

	///文件操作
	FILE *fp = fopen(filename, "r");
	if (fp == NULL)
		return strlen(dest);

	const int const_buffer_length = 1024;
	char buffer[const_buffer_length] = "";
	
	//////先检查section出现的位置
	while (fgets(buffer, const_buffer_length, fp))
	{
		if (match_section(buffer, section))
			break;
	}
	
	////先检查key出现的位置
	while (fgets(buffer, const_buffer_length, fp))
	{
		char *pos = buffer;
		
		while (isspace(*pos) && *pos)
			pos++;

		if (*pos == '[') //new section, 返回默认值
			break;
		
		if (!match_key(buffer, key))
			continue;
		
		//查找'='位置
		pos = strchr(pos, '=');
		assert(pos != NULL);
		
		pos++;
		
		//去掉value值前的空格符
		while (isspace(*pos) && *pos)
			pos++;
		
		//无填充value值
		char *end = pos + strlen(pos);
		if (end == pos)
		{
			dest[0] = 0;
			break;
		}
		
		//去掉value值后的空格符与回车换行符	
		while (isspace(*(end-1)) && end != pos)
			end--;
		assert(end >= pos);
		//*end = 0;
		
		//处理双引号及单引号
		if ( *pos == '\'' || *pos == '\"')
		{
			if ( (*(end-1) == *pos) && (end-1 != pos) )
			{
				pos++;
				end--;
				assert(end >= pos);
			}
		}
		
		//返回svalue值
		int length = end - pos;
		if (length >= size)
			length = size - 1;
		
		//将value拷贝到目标地址
		strncpy(dest, pos, length);
		dest[length] = 0;

		break;
	}
	fclose(fp);

	return strlen(dest);
}

/*从文件中读取一个配置的字符串*/
int GetPrivateProfileString(const char* section, const char* key, const char* sdefault, char *dest, int size, const char* filename)
{
	//列举section
	if (dest == NULL)
		return 0;
	else if (section == NULL)
		return get_sections(dest, size, filename);
	else if (key == NULL)
		return get_section_keys(section, dest, size, filename);
	else
		return get_section_key_value(section, key, sdefault, dest, size, filename);
}

/*返回整型值*/
int GetPrivateProfileInt(const char* section, const char* key, int ndefault, const char* filename)
{
	if (section == NULL || key == NULL)
		return ndefault;

	//设置默认的返回串
	char buffer[32] = "";
	
	//调用getstring获取匹配的串
	if (get_section_key_value(section, key, "", buffer, 30, filename) == 0)
		return ndefault;
	else
		return atoi(buffer);
}

/*返回整型值*/
bool WritePrivateProfileString(const char* section, const char* key, const char* value, const char* filename)
{
	//有效性检查
	if (section == NULL)
		return false;

	///////////////////
	FILE *fp = NULL;
	std::list<std::string> content;
	
	const int const_buffer_length = 1024;
	char buffer[const_buffer_length] = "";
	
	//扫描原始文件
	fp = fopen(filename, "r");
	if (fp != NULL)
	{
		while (fgets(buffer, const_buffer_length, fp))
			content.push_back(buffer);
		fclose(fp);
		fp = NULL;
	}
	
	//读方式打开文件
	fp = fopen(filename, "w");
	if (fp == NULL)
		return false;

	//配置内容的修改
	std::list<std::string>::iterator it_begin = content.end();
	std::list<std::string>::iterator it_key = content.end();
	std::list<std::string>::iterator it_end = content.end();

	std::list<std::string>::iterator it;
	for (it = content.begin(); it != content.end(); it++)
	{
		if (!match_section(it->c_str(), section))
			continue;

		it_begin = it;
		break;
	}
	for (it++; it != content.end(); it++)
	{
		const char* pos = it->c_str();
		while (isspace(*pos) && *pos)
			pos++;

		if (key && match_key(pos, key))
			it_key = it;			
			
		if (*pos != '[')
			continue;

		it_end = it;
		break;
	}

	if (key == NULL)
		content.erase(it_begin, it_end);
	else if (value == NULL)
		content.erase(it_key);
	else if (it_begin == content.end())
		content.push_back(std::string("\n[") + section + "]\n" + key + " = " + value + "\n");
	else if (it_key == content.end())
		content.insert(it_end, std::string(key) + " = " + value + "\n");
	else
		*it_key = std::string(key) + " = " + value + "\n";

	//先写入起始部分
	while (!content.empty())
	{
		fputs(content.front().c_str(), fp);
		content.pop_front();
	}

	fclose(fp);
	return true;
}

bool WritePrivateProfileInt(const char* section, const char* key, int value, const char* filename)
{
	char buffer[32] = "";
	sprintf(buffer, "%d", value);
	return WritePrivateProfileString(section, key, buffer, filename);
}

#endif

