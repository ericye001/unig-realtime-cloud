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



// --------------------------------------------------------------------------
//  Includes
// --------------------------------------------------------------------------
#include "common/IniParser.h"
#include "common/Utils.h"
const int lineSize = 1024;

// --------------------------------------------------------------------------
//  IniParser_T: Constructors and Destructor
// --------------------------------------------------------------------------
IniParser_T::IniParser_T(const char* const pIniFile)
  : i_iniFile(pIniFile)
{
  // no body
}

void IniParser_T::load(void)
{
  ifstream infile(i_iniFile.c_str());
  if(! infile)
  {
    throw IniParserException_T(
                string( "Fail to open " + i_iniFile + " file\n"
                        "\tat IniParser_T::load")+
                SrcLocation_T(__FILE__, __LINE__));
  }
  char line[lineSize];
  string tmpRemark;
  Section_T pSectionName;
  ParameterPair_T pp;
  bool IsFirstSection = true;
  while(infile.getline(line, lineSize))
  {
        if(line[0] == '#')
        {
          tmpRemark += line;
          tmpRemark += "\n";
          continue;
        }
        if(infile.bad() || infile.fail())
        {
            infile.close();
            throw IniParserException_T(
                    string("Fail to read ")+i_iniFile+" file:"+ strerror(errno) + "\n"
                            "\tat IniParser_T::load"+
                    SrcLocation_T(__FILE__, __LINE__));
        }

        for (int i = strlen(line)-1; i >= 0; --i)
        {
            if (line[i] == '\r' || line[i] == '\n')
            {
                line[i] = '\0';
                continue;
            }
            break;
        }

        if(line[0] == '[' && line[strlen(line)-1] == ']')
        {
          {
            i_allSections.push_back(pSectionName);
            pSectionName.i_ppv.clear();
          }
          pSectionName.i_remark = tmpRemark;
          tmpRemark.erase(0, tmpRemark.length());
          pSectionName.i_name = line + 1;
          pSectionName.i_name.erase(pSectionName.i_name.length() - 1);
          IsFirstSection = false;
          continue;
        }
        if(strstr(line, "=") != NULL)
        {
          pp.i_remark = tmpRemark;
          tmpRemark.erase(0, tmpRemark.length());
          pp.i_key = line;
          pp.i_key.erase(pp.i_key.find("="));
          pp.i_val = strstr(line, "=") + 1;
          pSectionName.i_ppv.push_back(pp);
          if(pp.i_key == "SectionType")
          {
            pSectionName.i_type = atoi(pp.i_val.c_str());
          }
          continue;
        }
    }
    i_allSections.push_back(pSectionName);
    infile.close();
}

void IniParser_T::save(void)
{
  string tmpFile = i_iniFile + ".tmp";
  ofstream outfile(tmpFile.c_str());
  if(! outfile)
  {
    throw IniParserException_T(
                    string( "Fail to open " +tmpFile+" file\n"
                            "\tat IniParser_T::save")+
                    SrcLocation_T(__FILE__, __LINE__));
  }

  vector<Section_T>::iterator crt_section = i_allSections.begin();
  vector<Section_T>::iterator end_section = i_allSections.end();
  vector<ParameterPair_T>::iterator crt_pp;
  vector<ParameterPair_T>::iterator end_pp;

  for(; crt_section != end_section; ++ crt_section)
  {
    outfile << crt_section->i_remark
            << "[" << crt_section->i_name << "]"
            << endl;
    crt_pp = (crt_section->i_ppv).begin();
    end_pp = (crt_section->i_ppv).end();
    for(; crt_pp != end_pp; ++ crt_pp)
    {
      outfile << crt_pp->i_remark
              << crt_pp->i_key
              << "="
              << crt_pp->i_val
              << endl;
      if(outfile.bad() || outfile.fail())
      {
        outfile.close();
        throw IniParserException_T(
                    string( "Fail to write "+tmpFile+" file\n"
                            "\tat IniParser_T::save")+
                    SrcLocation_T(__FILE__, __LINE__));
      }
    }
    outfile << endl;
  }
  outfile.close();

  if (rename(tmpFile.c_str(), i_iniFile.c_str()) != 0)
    {
    throw IniParserException_T(
                    string( "Fail to rename "+tmpFile+" file\n"
                            "\tat IniParser_T::save")+
                    SrcLocation_T(__FILE__, __LINE__));
    }
}

void IniParser_T::unload(void)
{
  vector<Section_T>::iterator begin_it = i_allSections.begin();
  vector<Section_T>::iterator end_it = i_allSections.end();
  vector<Section_T>::iterator crt_it;
  for(crt_it = begin_it; crt_it != end_it; ++crt_it)
  {
    crt_it->i_ppv.clear();
  }

  i_allSections.clear();
}

void
IniParser_T::getSection(const char* pSectionName,
        vector<Section_T>::iterator& aSectionIt
)
{
  vector<Section_T>::iterator crt_section = i_allSections.begin();
  vector<Section_T>::iterator end_section = i_allSections.end();
  for(; crt_section != end_section; ++ crt_section)
  {
    if (strcmp(crt_section->i_name.c_str(), pSectionName) == 0)
    {
      aSectionIt = crt_section;
      return;
    }
  }
    throw IniParserException_T(
                    string( "Section ")+pSectionName+" does not exist\n"
                            "\tat IniParser_T::getSection"+
                    SrcLocation_T(__FILE__, __LINE__));
}
void 
IniParser_T::getSection(const char* pSectionName,
    vector<Section_T*>& sections)
{
  vector<Section_T>::iterator crt_section = i_allSections.begin();
  vector<Section_T>::iterator end_section = i_allSections.end();
  for(; crt_section != end_section; ++ crt_section)
  {
    if (strcmp(crt_section->i_name.c_str(), pSectionName) == 0)
    {
      sections.push_back(&(*crt_section));
    }
  }
}

bool
IniParser_T::getSection(const char* pSectionName,
        vector<Section_T>::iterator& startIt,
        vector<Section_T>::iterator& aSectionIt
)
{
  vector<Section_T>::iterator crt_section = startIt;
  vector<Section_T>::iterator end_section = i_allSections.end();
  for (++crt_section; crt_section != end_section; ++crt_section)
  {
    if (strcmp(crt_section->i_name.c_str(), pSectionName) == 0)
    {
      aSectionIt = crt_section;
      return true;
    }
  }

  return false;
}

void
IniParser_T::getKeyVal(
  Section_T& aSection,
  const char* pKey,
  string& val
)
{
  vector<ParameterPair_T>::iterator crt_pp;
  vector<ParameterPair_T>::iterator end_pp;

  crt_pp = aSection.i_ppv.begin();
  end_pp = aSection.i_ppv.end();

  for(; crt_pp != end_pp; ++ crt_pp)
  {
    if (strcmp(crt_pp->i_key.c_str(), pKey) == 0)
    {
      val = crt_pp->i_val;
      return;
    }
  }
    throw IniParserException_T(
                    string( "Fail to load key(")+pKey+") in section("+aSection.i_name+")\n"
                            "\tat IniParser_T::getKeyVal"+
                    SrcLocation_T(__FILE__, __LINE__));
}

void
IniParser_T::getKeyVal(
  Section_T& aSection,
  const char* pKey,
  string*& val
)
{
  vector<ParameterPair_T>::iterator crt_pp;
  vector<ParameterPair_T>::iterator end_pp;

  crt_pp = aSection.i_ppv.begin();
  end_pp = aSection.i_ppv.end();

  for(; crt_pp != end_pp; ++ crt_pp)
  {
    if (strcmp(crt_pp->i_key.c_str(), pKey) == 0)
    {
      val = &(crt_pp->i_val);
      return;
    }
  }
    throw IniParserException_T(
                    string( "Fail to load key(")+pKey+") in section("+aSection.i_name+")\n"
                            "\tat IniParser_T::getKeyVal(%s:%d)"+
                    SrcLocation_T(__FILE__, __LINE__));
}


void
IniParser_T::getKeyVal(
  Section_T& aSection,
  const char* pKey,
  int& val
)
{
  string tmpVal;
  getKeyVal(aSection, pKey, tmpVal);
  val = atoi(tmpVal.c_str());
}

void
IniParser_T::getKeyVal(
  Section_T& aSection,
  const char* pKey,
  long& val
)
{
  string tmpVal;
  getKeyVal(aSection, pKey, tmpVal);
  val = atol(tmpVal.c_str());
}


void
IniParser_T::getKeyVal(
  Section_T& aSection,
  const char* pKey,
  unsigned long& val
)
{
  string tmpVal;
  char* pTmp;
  getKeyVal(aSection, pKey, tmpVal);
  val = strtoul(tmpVal.c_str(), &pTmp, 0);
}

void
IniParser_T::getKeyVal(Section_T& aSection, const char* pKey, bool& val)
{
  string tmpVal;
  getKeyVal(aSection, pKey, tmpVal);
  if (strcasecmp(tmpVal.c_str(), "true") == 0 ||
      strcasecmp(tmpVal.c_str(), "yes") == 0)
  {
    val = true;
  }
  else
  {
    val = false;
  }
}

void IniParser_T::getKeyVal(Section_T& aSection, const char* pKey,
        vector<string>& valVector
)
{
  vector<ParameterPair_T>::iterator crt_pp;
  vector<ParameterPair_T>::iterator end_pp;

  crt_pp = aSection.i_ppv.begin();
  end_pp = aSection.i_ppv.end();
  for(; crt_pp != end_pp; ++crt_pp)
  {
    if (strcmp(crt_pp->i_key.c_str(), pKey) == 0)
    {
      valVector.push_back(crt_pp->i_val);
    }
  }
}

void
IniParser_T::setKeyVal(
  Section_T& aSection,
  const char* pKey,
  const string& val
)
{
  vector<ParameterPair_T>::iterator crt_pp = aSection.i_ppv.begin();
  vector<ParameterPair_T>::iterator end_pp = aSection.i_ppv.end();

  for(; crt_pp != end_pp; ++ crt_pp)
  {
    if (strcmp(crt_pp->i_key.c_str(), pKey) == 0)
    {
      crt_pp->i_val = val;
      return save();
    }
  }
}

void
IniParser_T::setKeyVal(
  Section_T& aSection,
  const char* pKey,
  const char* pVal
)
{
  vector<ParameterPair_T>::iterator crt_pp = aSection.i_ppv.begin();
  vector<ParameterPair_T>::iterator end_pp = aSection.i_ppv.end();

  for(; crt_pp != end_pp; ++ crt_pp)
  {
    if (strcmp(crt_pp->i_key.c_str(), pKey) == 0)
    {
      crt_pp->i_val = pVal;
      return save();
    }
  }
}

void IniParser_T::setKeyVal(Section_T& aSection, const char* pKey, int val)
{
  char tmpVal[20];
  memset(tmpVal, 0, sizeof(tmpVal));

  sprintf(tmpVal, "%d", val);

  return setKeyVal(aSection, pKey, tmpVal);
}

void IniParser_T::setKeyVal(Section_T& aSection, const char* pKey, long val)
{
  char tmpVal[20];
  memset(tmpVal, 0, sizeof(tmpVal));

  sprintf(tmpVal, "%ld", val);

  return setKeyVal(aSection, pKey, tmpVal);
}

void
IniParser_T::setKeyVal(
  Section_T& aSection,
  const char* pKey,
  unsigned long val
)
{
  char tmpVal[20];
  memset(tmpVal, 0, sizeof(tmpVal));

  sprintf(tmpVal, "%ld", val);

  return setKeyVal(aSection, pKey, tmpVal);
}

void IniParser_T::setKeyVal(Section_T& aSection, const char* pKey, bool val)
{
  if (val)
  {
    return setKeyVal(aSection, pKey, "true");
  }
  else
  {
    return setKeyVal(aSection, pKey, "false");
  }
}

void
IniParser_T::setKeyVal(
    Section_T& aSection, const char* pKey, const vector<string>& valVector
)
{
  vector<ParameterPair_T>::iterator crt_pp;

  //  First, remove all old "key=val"
  for (crt_pp = (aSection.i_ppv).begin();
      crt_pp != (aSection.i_ppv).end();)
  {
    if(crt_pp->i_key != pKey)
    {
      ++crt_pp;
      continue;
    }
    //  Remove the element on crt_pp position and return next position
    crt_pp = (aSection.i_ppv).erase(crt_pp);
  }

  ParameterPair_T aParameterPair;
  aParameterPair.i_key = pKey;
  vector<string>::const_iterator crtVal = valVector.begin();
  vector<string>::const_iterator endVal = valVector.end();

  for (; crtVal != endVal; ++crtVal)
  {
    aParameterPair.i_val = *crtVal;
    aSection.i_ppv.push_back(aParameterPair);
  }

  return save();
}

string& IniParser_T::errMsg(void)
{
  return i_errMsg;
}

ostream& IniParser_T::operator<<(ostream& os)
{
  vector<Section_T>::const_iterator crt_section = i_allSections.begin();
  vector<Section_T>::const_iterator end_section = i_allSections.end();
  vector<ParameterPair_T>::const_iterator crt_pp;
  vector<ParameterPair_T>::const_iterator end_pp;
  for(; crt_section != end_section; ++ crt_section)
  {
    os << "----------------------------------------------------------------\n";

    os << crt_section->i_remark << endl
      << "[" << crt_section->i_name << "]" << endl;

    crt_pp = (crt_section->i_ppv).begin();
    end_pp = (crt_section->i_ppv).end();
    for(; crt_pp != end_pp; ++ crt_pp)
    {
      os << crt_pp->i_remark << endl
          << crt_pp->i_key << "=" << crt_pp->i_val << endl;
    }
  }

  return os ;
}
IniParserException_T::IniParserException_T(const char* what) throw()
    : _what(what)
{
    // No body
}

IniParserException_T::IniParserException_T(const string& what) throw()
    : _what(what)
{
    // No body
}

const char* IniParserException_T::what() const throw()
{
    return _what.c_str();
}


