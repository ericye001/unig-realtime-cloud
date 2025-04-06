#include "Global.h"
#include "Util.h"

void Util::StringToken(string str, string seps, vector<string>& vecResult)
{
	char* token;
	token = strtok((char*)str.c_str(), (char*)seps.c_str());
	while(token != NULL)
	{
		vecResult.push_back(string(token));
		token = strtok( NULL, seps.c_str() ); 
	}
}

bool Util::StringFind(string str, string sub)
{
	int npos = 0;
	npos = str.find(sub);
	
	if(npos == -1)
	{
		return false;
	}
	else
	{
		return true;
	}
}
