#ifndef __RTCSCSTRING_H__
#define __RTCSCSTRING_H__

#include "rtcs.h"

class RTCS_BASEOBJECT_EXT_CLASS RTCS_CTstring
{
public:
// Constructors

	// constructs empty RTCS_CTstring
	RTCS_CTstring();
	// copy constructor
	RTCS_CTstring(const RTCS_CTstring& stringSrc);
	// from a single character
	RTCS_CTstring(LPCSTR lpsz);
	// from unsigned characters
	RTCS_CTstring(const unsigned char* psz);
	//
#ifndef WIN32
	RTCS_CTstring(LPCTSTR lpsz);
#endif
// Attributes & Operations

	// get data length
	int GetLength() const;
	// TRUE if zero length
	RTCS_CTbool IsEmpty() const;
	// clear contents to empty
	void Empty();

	int Find(const char* aBuf, int aPos, int aLen) const;
	int Find(const char* aCstr, int aPos=0) const;
	int Find(char aChar, int aPos=0) const;
	RTCS_CTstring Substr(int aPos,int aLen);

	char operator[](int nIndex) const;
	// return pointer to const string
	operator LPCTSTR() const;
	operator LPCSTR();


	// overloaded assignment

	// ref-counted copy from another RTCS_CTstring
	const RTCS_CTstring& operator=(const RTCS_CTstring& stringSrc);
	// set string content to single character
	const RTCS_CTstring& operator=(char ch);
	// copy string content from ANSI string (converts to char)
	const RTCS_CTstring& operator=(LPCSTR lpsz);
	// copy string content from unsigned chars
	const RTCS_CTstring& operator=(const unsigned char* psz);
	//
#ifndef WIN32
	const RTCS_CTstring& operator=(LPCTSTR lpsz);
#endif
	//

	// string concatenation

	// concatenate from another RTCS_CTstring
	const RTCS_CTstring& operator+=(const RTCS_CTstring& string);

	// concatenate a single character
	const RTCS_CTstring& operator+=(char ch);
	// concatenate a UNICODE character after converting it to char
	const RTCS_CTstring& operator+=(LPCTSTR lpsz);

	RTCS_BASEOBJECT_EXT_API friend RTCS_CTstring operator+(const RTCS_CTstring& string1,
			const RTCS_CTstring& string2);
	RTCS_BASEOBJECT_EXT_API friend RTCS_CTstring operator+(const RTCS_CTstring& string, char ch);
	RTCS_BASEOBJECT_EXT_API friend RTCS_CTstring operator+(char ch, const RTCS_CTstring& string);
	RTCS_BASEOBJECT_EXT_API friend RTCS_CTstring operator+(const RTCS_CTstring& string, LPCTSTR lpsz);
	RTCS_BASEOBJECT_EXT_API friend RTCS_CTstring operator+(LPCTSTR lpsz, const RTCS_CTstring& string);

	// string comparison

	// straight character comparison
	int Compare(LPCTSTR lpsz) const;

	// simple sub-string extraction

	RTCS_CTstring Left(int nCount) const;
	RTCS_CTstring Right(int nCount) const;
	void TrimRight(LPCTSTR lpszTargets);
	void TrimLeft(LPCTSTR lpszTargets);

	// upper/lower/reverse conversion

/*	// printf-like formatting using passed string
	void Format(LPCTSTR lpszFormat, ...);
	// printf-like formatting using referenced string resource
	void Format(UINT nFormatID, ...);
	// printf-like formatting using variable arguments parameter
	void FormatV(LPCTSTR lpszFormat, va_list argList);
*/

// Implementation
public:
	virtual ~RTCS_CTstring();

protected:
	char* m_pchData;   // pointer to ref counted string data

	// implementation helpers
	char* GetData() const;
	void Init();
	void ConcatCopy(int nSrc1Len, LPCTSTR lpszSrc1Data, int nSrc2Len, LPCTSTR lpszSrc2Data);
	void ConcatInPlace(int nSrcLen, LPCTSTR lpszSrcData);
	static int  SafeStrlen(LPCTSTR lpsz);
};

// Compare helpers
RTCS_BASEOBJECT_EXT_API
RTCS_CTbool operator==(const RTCS_CTstring& s1, const RTCS_CTstring& s2);
RTCS_BASEOBJECT_EXT_API
RTCS_CTbool operator==(const RTCS_CTstring& s1, LPCTSTR s2);
RTCS_BASEOBJECT_EXT_API
RTCS_CTbool operator==(LPCTSTR s1, const RTCS_CTstring& s2);
RTCS_BASEOBJECT_EXT_API
RTCS_CTbool operator!=(const RTCS_CTstring& s1, const RTCS_CTstring& s2);
RTCS_BASEOBJECT_EXT_API
RTCS_CTbool operator!=(const RTCS_CTstring& s1, LPCTSTR s2);
RTCS_BASEOBJECT_EXT_API
RTCS_CTbool operator!=(LPCTSTR s1, const RTCS_CTstring& s2);
RTCS_BASEOBJECT_EXT_API
RTCS_CTbool operator<(const RTCS_CTstring& s1, const RTCS_CTstring& s2);
RTCS_BASEOBJECT_EXT_API
RTCS_CTbool operator<(const RTCS_CTstring& s1, LPCTSTR s2);
RTCS_BASEOBJECT_EXT_API
RTCS_CTbool operator<(LPCTSTR s1, const RTCS_CTstring& s2);
RTCS_BASEOBJECT_EXT_API
RTCS_CTbool operator>(const RTCS_CTstring& s1, const RTCS_CTstring& s2);
RTCS_BASEOBJECT_EXT_API
RTCS_CTbool operator>(const RTCS_CTstring& s1, LPCTSTR s2);
RTCS_BASEOBJECT_EXT_API
RTCS_CTbool operator>(LPCTSTR s1, const RTCS_CTstring& s2);
RTCS_BASEOBJECT_EXT_API
RTCS_CTbool operator<=(const RTCS_CTstring& s1, const RTCS_CTstring& s2);
RTCS_BASEOBJECT_EXT_API
RTCS_CTbool operator<=(const RTCS_CTstring& s1, LPCTSTR s2);
RTCS_BASEOBJECT_EXT_API
RTCS_CTbool operator<=(LPCTSTR s1, const RTCS_CTstring& s2);
RTCS_BASEOBJECT_EXT_API
RTCS_CTbool operator>=(const RTCS_CTstring& s1, const RTCS_CTstring& s2);
RTCS_BASEOBJECT_EXT_API
RTCS_CTbool operator>=(const RTCS_CTstring& s1, LPCTSTR s2);
RTCS_BASEOBJECT_EXT_API
RTCS_CTbool operator>=(LPCTSTR s1, const RTCS_CTstring& s2);


template<class T> RTCS_CTuint RTCS_HashKey(T key);
template<> RTCS_BASEOBJECT_EXT_API RTCS_CTuint RTCS_HashKey<LPCTSTR>(LPCTSTR key);


#endif
