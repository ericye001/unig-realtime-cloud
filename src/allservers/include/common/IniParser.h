/*
* Copyright (c) 2002-2006, UOneNet, All right reserved
* All rights reserved.
* 
* 文件名称：
* 文件标识：
* 摘    要：
* 
*
* 修订记录：
*       修订日期    修订人    修订内容
*/

//---------------------------------------------------------------------------
//  Copyright (c) 2002-2006, UOneNet, All right reserved
//
//  Class IniParser_T:
//    A base class which parser ini file
//---------------------------------------------------------------------------
#ifndef INI_PARSER_HPP
#define INI_PARSER_HPP

#include <stdlib.h>                         // use atoi, atol, strtoul
#include <pthread.h>                        // use pthread_mutex_lock etc
#include <string>                           // use string
#include <vector>                           // use vector
#include <exception>
#include <fstream>
#include <iostream>
using namespace std;

//---------------------------------------------------------------------------
//  The declaration of parameter pair struct
//---------------------------------------------------------------------------
typedef struct
{
  string i_remark;
  string i_key;
  string i_val;
}ParameterPair_T;

typedef vector<ParameterPair_T> ParamPairVector_T;

//---------------------------------------------------------------------------
//  The declaration of section struct
//---------------------------------------------------------------------------
typedef struct
{
  string i_remark;
  string i_name;
  int i_type;
  ParamPairVector_T i_ppv;
}Section_T;

typedef vector<Section_T> SectionVector_T;

// --------------------------------------------------------------------------
//  这个类实现了读取更新Ini配置文件的接口
// --------------------------------------------------------------------------
class IniParser_T
{
  public:
    IniParser_T(const char* const pIniFile);
		
		ostream& operator<<(ostream& os);
		
    // 读取配置文件，将参数信息加载到i_allSections容器，
    // 包括以#开头的注释信息
    void load(void);
    void unload(void);
    // 保存配置文件，将i_allSections容器内的参数及注释信息写回文件
    void save(void);

    //  Get the first section named pSectionName in Section_T vector
    void getSection(
      const char* pSectionName,
      vector<Section_T>::iterator& aSectionIt
    );
    void getSection(
        const char* pSectionName,
        vector<Section_T*>& sections
    );
    //  Get the next section named pSectionName in Section_T vector from
    //  startIt Section_T iterator
    bool getSection(
      const char* pSectionName,
      vector<Section_T>::iterator& startIt,
      vector<Section_T>::iterator& aSectionIt
    );

    // ------------------------------------------------------------------
    // 查找给定的section及keyword，并返回该key的值，如果找不到key则
    // 退出程序
    //---------------------------------------------------------------------
    void getKeyVal(Section_T& aSection, const char* pKey, string& val);
    void getKeyVal(Section_T& aSection, const char* pKey, string*& val);
		void getKeyVal(const char* pSection, const char* pKey, string& val);
		void getKeyVal(const char* pSection, const char* pKey, string*& val);
		
		void getKeyVal(Section_T& aSection, const char* pKey, char* val);
		void getKeyVal(const char* pSection, const char* pKey, char* val);
		
    //-----------------------------------------------------------------------
    void getKeyVal(Section_T& aSection, const char* pKey, int& val);
		void getKeyVal(const char* pSection, const char* pKey, int& val);
		
    //-----------------------------------------------------------------------
    void getKeyVal(Section_T& aSection, const char* pKey, long& val);
		void getKeyVal(const char* pSection, const char* pKey, long& val);
    //-----------------------------------------------------------------------
    void getKeyVal(Section_T& aSection, const char* pKey, unsigned long& val);
		void getKeyVal(const char* pSection, const char* pKey, unsigned long& val);
		
    //-----------------------------------------------------------------------
    void getKeyVal(Section_T& aSection, const char* pKey, bool& val);
		void getKeyVal(const char* pSection, const char* pKey, bool& val);
		
    //-----------------------------------------------------------------------
    void getKeyVal(Section_T& aSection, const char* pKey, vector<string>& aStrVector);
		void getKeyVal(const char* pSection, const char* pKey, vector<string>& aStrVector);
		
    //-----------------------------------------------------------------------
    //  Update the value of the key pointed by 'pKey' in 'aSection' section
    //-----------------------------------------------------------------------
    void setKeyVal(Section_T& aSection, const char* pKey, const string& val);
    void setKeyVal(Section_T& aSection, const char* pKey, const char* pVal);
    void setKeyVal(Section_T& aSection, const char* pKey, int val);
    void setKeyVal(Section_T& aSection, const char* pKey, long val);
    void setKeyVal(Section_T& aSection, const char* pKey, unsigned long val);
    void setKeyVal(Section_T& aSection, const char* pKey, bool val);
    void setKeyVal(Section_T& aSection, const char* pKey, const vector<string>& aValVector);

		void setKeyVal(const char* pSection, const char* pKey, const string& val);
    void setKeyVal(const char* pSection, const char* pKey, const char* pVal);
    void setKeyVal(const char* pSection, const char* pKey, int val);
    void setKeyVal(const char* pSection, const char* pKey, long val);
    void setKeyVal(const char* pSection, const char* pKey, unsigned long val);
    void setKeyVal(const char* pSection, const char* pKey, bool val);
    void setKeyVal(const char* pSection, const char* pKey, const vector<string>& aValVector);
		
    string& errMsg(void);

  private:
    // Disallow copy and assign constructure
    IniParser_T(const IniParser_T& rhs);
    IniParser_T& operator=(const IniParser_T& rhs);

    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  i_iniFile
    //      Ini配置文件完整文件名
    //  i_allSections
    //    配置文件中所有section及参数对的集合
    // -----------------------------------------------------------------------
    SectionVector_T i_allSections;
    string i_iniFile;
    string i_errMsg;
};

class IniParserException_T : public exception
{
	public:
		IniParserException_T(const char* what) throw();
		IniParserException_T(const string& what) throw();
		~IniParserException_T()throw(){};
		
		virtual const char* what() const throw();
		
	private:
		std::string _what;
};

#endif  // ifndef INI_PARSER_HPP

