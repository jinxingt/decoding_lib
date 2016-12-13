/*
 * GetPrivateProfileString �÷�
 * ����ԭ�ͣ�int GetPrivateProfileString(section, key, sdefault, buffer, len, filename)
 * ���أ� ���������ļ��ж�ȡ������buffer�ַ�������
 *   1)����sectionΪ NULL ʱ�����������ļ������е�section�����ƣ���һ��0���ȵĴ�����
 *   2)����keyΪ NULL ʱ�����������ļ��и�section�µ�����key�����ƣ���һ��0���ȵĴ�����
 *   3)����section.keyָ�������ò�����ʱ������sdefaultֵ����
 *   4)��buffer���ڷ��ض�ȡ���������ݣ���lenС��ʵ������ʱ����buffer��'\0'������
 *   5)�����ص�valueֵ�����ü�ǰ��հ׷��������õ����Ż�˫����ǿ��ָ��
 * ���������windows.api�����һ��
 *   1)����ָ����ȡsection������keysʱ�����δƥ�䵽section����keyʱ
 *       windows�·���Ĭ��ֵ(sdefault)
 *       ��ʵ�ַ��ؿմ�
 *   2)��ע����
 *       windows��ÿ�е�һ���ǿհ׷�Ϊ";"ʱ��ʾ�ñ�ע��
 *       ��ʵ�ֽ��ڷ���keysʱ���Ŷ���ע���У���֧��"#"��"!"��";"
 *   3)��windows�²�֧����";"Ϊǰ׺��key���塣��ʵ��֧��
 *
 * GetPrivateProfileInt �÷�
 * ����ԭ�ͣ�int GetPrivateProfileInt(section, key, ndefault, filename)
 * ���أ���section.key���ڣ��������ݲ�Ϊ��ʱ������ʵ��ֵ
 *       �����������ndefalutֵ
 *
 * WritePrivateProfileString �÷�
 * ����ԭ��: bool WritePrivateProfileString(section, key, value, filename)
 * ���أ��ɹ�����true����������false
 *   1)����keyΪ NULL ʱ��ɾ�������ļ��е�sectionָ����
 *   2)����valueΪ NULL ʱ��ɾ�������ļ��е�key������
 *   3)�����������д��section.key.valueʵ��ֵ
 *
 * WritePrivateProfileInt �÷�
 * ����ԭ��: bool WritePrivateProfileInt(section, key, value, filename)
 * �÷��� WritePrivateProfileString ��ȫ��ͬ
 */

#ifndef _WIN32

#include <string>
#include <string.h>
#include <list>
#include <assert.h>

/*����ַ����Ƿ�Ϊsection*/
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

/*����ַ����Ƿ�Ϊsection*/
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

//���ص��ַ���(dest)β�����������'\0', ��windowsһ��
static int get_sections(char* dest, int size, const char* filename)
{
	//�쳣����
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

	//ɨ��sections
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

//���ص��ַ���(dest)β�����������'\0', ��windowsһ��
//windows�µ�section��key������ʱ��������Ĭ�ϴ��������ʵ��
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
	
	//ɨ��section.keys
	const char* orig = dest;

	const int const_buffer_length = 1024;
	char buffer[const_buffer_length] = "";

	//////�ȼ��section���ֵ�λ��
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
	
	//��Ĭ��ֵ������Ŀ���ַ
	int ndefaut = strlen(sdefault);
	if (ndefaut >= size)
		ndefaut = size - 1;
	strncpy(dest, sdefault, ndefaut);
	dest[ndefaut] = 0;

	///�ļ�����
	FILE *fp = fopen(filename, "r");
	if (fp == NULL)
		return strlen(dest);

	const int const_buffer_length = 1024;
	char buffer[const_buffer_length] = "";
	
	//////�ȼ��section���ֵ�λ��
	while (fgets(buffer, const_buffer_length, fp))
	{
		if (match_section(buffer, section))
			break;
	}
	
	////�ȼ��key���ֵ�λ��
	while (fgets(buffer, const_buffer_length, fp))
	{
		char *pos = buffer;
		
		while (isspace(*pos) && *pos)
			pos++;

		if (*pos == '[') //new section, ����Ĭ��ֵ
			break;
		
		if (!match_key(buffer, key))
			continue;
		
		//����'='λ��
		pos = strchr(pos, '=');
		assert(pos != NULL);
		
		pos++;
		
		//ȥ��valueֵǰ�Ŀո��
		while (isspace(*pos) && *pos)
			pos++;
		
		//�����valueֵ
		char *end = pos + strlen(pos);
		if (end == pos)
		{
			dest[0] = 0;
			break;
		}
		
		//ȥ��valueֵ��Ŀո����س����з�	
		while (isspace(*(end-1)) && end != pos)
			end--;
		assert(end >= pos);
		//*end = 0;
		
		//����˫���ż�������
		if ( *pos == '\'' || *pos == '\"')
		{
			if ( (*(end-1) == *pos) && (end-1 != pos) )
			{
				pos++;
				end--;
				assert(end >= pos);
			}
		}
		
		//����svalueֵ
		int length = end - pos;
		if (length >= size)
			length = size - 1;
		
		//��value������Ŀ���ַ
		strncpy(dest, pos, length);
		dest[length] = 0;

		break;
	}
	fclose(fp);

	return strlen(dest);
}

/*���ļ��ж�ȡһ�����õ��ַ���*/
int GetPrivateProfileString(const char* section, const char* key, const char* sdefault, char *dest, int size, const char* filename)
{
	//�о�section
	if (dest == NULL)
		return 0;
	else if (section == NULL)
		return get_sections(dest, size, filename);
	else if (key == NULL)
		return get_section_keys(section, dest, size, filename);
	else
		return get_section_key_value(section, key, sdefault, dest, size, filename);
}

/*��������ֵ*/
int GetPrivateProfileInt(const char* section, const char* key, int ndefault, const char* filename)
{
	if (section == NULL || key == NULL)
		return ndefault;

	//����Ĭ�ϵķ��ش�
	char buffer[32] = "";
	
	//����getstring��ȡƥ��Ĵ�
	if (get_section_key_value(section, key, "", buffer, 30, filename) == 0)
		return ndefault;
	else
		return atoi(buffer);
}

/*��������ֵ*/
bool WritePrivateProfileString(const char* section, const char* key, const char* value, const char* filename)
{
	//��Ч�Լ��
	if (section == NULL)
		return false;

	///////////////////
	FILE *fp = NULL;
	std::list<std::string> content;
	
	const int const_buffer_length = 1024;
	char buffer[const_buffer_length] = "";
	
	//ɨ��ԭʼ�ļ�
	fp = fopen(filename, "r");
	if (fp != NULL)
	{
		while (fgets(buffer, const_buffer_length, fp))
			content.push_back(buffer);
		fclose(fp);
		fp = NULL;
	}
	
	//����ʽ���ļ�
	fp = fopen(filename, "w");
	if (fp == NULL)
		return false;

	//�������ݵ��޸�
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

	//��д����ʼ����
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

