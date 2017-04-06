#include "stdafx.h"
#include <fstream>
#include "rxmapbase/RXMapExtractor.h"
#include "StormLib.h"
#include <BWlib/BWrepAPI.h>


#ifdef _WIN32

#include <windows.h>
#include <comutil.h>

#endif

RXMapExtractor::RXMapExtractor(void)
{
}

RXMapExtractor::~RXMapExtractor(void)
{
}



#ifdef _WIN32

int RXMapExtractor::extractTo(const std::wstring &from, const std::wstring &to)
{
    return extractTo((LPCSTR)(_bstr_t)from.c_str(),(LPCSTR)(_bstr_t)to.c_str());
}
#endif

int RXMapExtractor::extractTo(const std::string &from, const std::string &to)
{
    BWrepFile rep;
    if (rep.Load(from.c_str())!=0)
        return -1;

    std::string fileName = dumpToTempFileA(rep);
	if(fileName == "")
		return -2;

    bool res = createSCM(to, fileName);

    unlink(fileName.c_str());

	if(res)
		return 0;
	
	return -3;
}




bool RXMapExtractor::createSCM(const std::string &to, std::string &chk)
{
    HANDLE hMPQ = NULL;
    if(SFileCreateArchiveEx(to.c_str(), CREATE_ALWAYS, 2, &hMPQ)==false)
        return false;

#ifdef _WIN32
    BOOL res = SFileAddFile(hMPQ, chk.c_str(), "staredit\\scenario.chk", MPQ_FILE_IMPLODE | MPQ_FILE_REPLACEEXISTING);
#else
	BOOL res = SFileAddFile(hMPQ, chk.c_str(), "staredit\\scenario.chk", MPQ_FILE_IMPLODE | MPQ_FILE_REPLACEEXISTING);
#endif
    SFileCloseArchive(hMPQ);
 
    if (res == false)
    {
        //Some bad happened when adding the file (maybe not enough room on the HD ?)
        //unlink(to.c_str());
        return false;
    }

    return true;
}



std::string RXMapExtractor::dumpToTempFileA(BWrepFile &f)
{
    std::string path = createTempFileA();
	if(path != "")
	{
		std::ofstream outStream(path.c_str(), std::ios::out | std::ios::binary);

		for (int i = 0; i < f.m_oMap.GetNumSection(); i++)
		{
			const BWrepMapSection * section = f.m_oMap.GetSectionNumber(i);
			int size = section->GetSize();

			outStream.write(section->GetTitle(), 4);
			outStream.write((const char *)&size, 4);
			outStream.write((const char *)(section->GetData()), size);
		}

		outStream.close();
	}

    return path;
}



std::string RXMapExtractor::createTempFileA()
{
#ifdef _WIN32
    char temp[1024];
    char res[MAX_PATH  +1 ];

    GetTempPathA(1024, temp);
    GetTempFileNameA(temp,"rexplorerfile",0,res);
    return res;
#else
    char sfn[15] = "/tmp/rx.XXXXXX";
	int sfp = mkstemp(sfn);

	if(sfp != -1)
	{
		close(sfp);
		return sfn;
	}
	return "";
#endif
}
