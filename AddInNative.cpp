
#include <Winsock2.h> 
#include <ws2tcpip.h> 

#include <wchar.h>
#include <string>
#include "AddInNative.h"
#include "sky.h"
#include "server.h"
static const wchar_t g_kClassNames[] = L"CAddInNative"; //|OtherClass1|OtherClass2";

uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len = 0);
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len = 0);
uint32_t getLenShortWcharStr(const WCHAR_T* Source);

//---------------------------------------------------------------------------//
long GetClassObject(const wchar_t* wsName, IComponentBase** pInterface)
{
    log("GetClassObject(). begin.");
    if(!*pInterface)
    {
        *pInterface= new CAddInNative();
        log("GetClassObject(). return pInterface.");
        return (long)*pInterface;
    }
    log("GetClassObject(). return error.");
    return 0;
}
//---------------------------------------------------------------------------//
long DestroyObject(IComponentBase** pIntf)
{
    log("DestroyObject().");
   if(!*pIntf)
      return -1;

   delete *pIntf;
   *pIntf = 0;
   return 0;
}
//---------------------------------------------------------------------------//
const WCHAR_T* GetClassNames()
{
    log("GetClassNames().");
    WCHAR_T* lnames = L"CAddInNative";
    return lnames;
}
//---------------------------------------------------------------------------//
//CAddInNative
CAddInNative::CAddInNative()
{
    log("CAddInNative().");

}
//---------------------------------------------------------------------------//
CAddInNative::~CAddInNative()
{
    log("~CAddInNative().");
}
//---------------------------------------------------------------------------//
bool CAddInNative::Init(void* pConnection)
{ 
    log("Init().");
    base = (IAddInDefBase *)(pConnection);
    return true;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetInfo()
{ 
    return 2000; 
}
//---------------------------------------------------------------------------//
void CAddInNative::Done()
{
    log("Done().");
}
//---------------------------------------------------------------------------//
bool CAddInNative::RegisterExtensionAs(WCHAR_T** wsLanguageExt)
{ 
    wchar_t* wsExtension = L"vkChat";
    int iActualSize = ::wcslen(wsExtension) + 1;
    WCHAR_T* dest = 0;
    if (this->memManager)
    {
        auto res = this->memManager->AllocMemory((void**)wsLanguageExt, iActualSize * sizeof(WCHAR_T));
        if (res)
            ::convToShortWchar(wsLanguageExt, wsExtension, iActualSize);
        log("RegisterExtensionAs return 'vkChat'");
        return true;
    }
    error("RegisterExtensionAs(). return false.");
    return false;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNProps()
{ 
    log("GetNProps().");
    return eLastProp;
}
//---------------------------------------------------------------------------//
long CAddInNative::FindProp(const WCHAR_T* wsPropName)
{ 
    std::wstring wstringPropName(wsPropName);
    std::string stdstringPropName(wstringPropName.begin(), wstringPropName.end());
    log("FindProp(). name:'"+ stdstringPropName+"'");

    if ( "port"== stdstringPropName) {
        log("FindProp(). name:'" + stdstringPropName + "'. return: '"+ std::to_string(Props::port) +"'");
        return Props::port;
    }
    log("FindProp(). name:'" + stdstringPropName + "'. return '-1'");
    return -1;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInNative::GetPropName(long lPropNum, long lPropAlias)
{ 
    log("GetPropName(). Nomber: '" +std::to_string(lPropNum)+"'." + " alias:" + std::to_string(lPropAlias));

    switch (lPropNum) {
    case Props::port:
        WCHAR_T * t1 = NULL, *test = L"port";
        int iActualSize = wcslen(test) + 1;
        this->memManager->AllocMemory((void**)&t1, iActualSize * sizeof(WCHAR_T));
        ::convToShortWchar(&t1, test, iActualSize);

        std::wstring wstringLog(t1);
        std::string stdstringLog(wstringLog.begin(), wstringLog.end());
        log("GetPropName(). Nomber: '" + std::to_string(lPropNum)+"'. return: '"+ stdstringLog+"'.");

        return t1;
    }

    log("GetPropName(). Nomber: '" + std::to_string(lPropNum) + "'. return: NULL.");
    return NULL;
}
//---------------------------------------------------------------------------//
bool CAddInNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{ 
    log("GetPropVal(). Nomber: '" + std::to_string(lPropNum)+"'.");

    switch (lPropNum) {
    case Props::port:

        //pvarPropVal->vt = VTYPE_BOOL;
        //pvarPropVal->bVal = true;

        pvarPropVal->vt = VTYPE_I4;
        pvarPropVal->intVal = portToS;
        log("GetPropVal(). Props::port. return true.");
        return true;

    };
    log("GetPropVal(). Nomber: '" + std::to_string(lPropNum) + "'. return false.");
    return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::SetPropVal(const long lPropNum, tVariant* varPropVal)
{ 
    log("SetPropVal(). Nomber: " + std::to_string(lPropNum));
    switch (lPropNum) {
    case Props::port:
        if (TV_VT(varPropVal) != VTYPE_I4){
            error("SetPropVal(). Props::port. not int.");
            return false;
        }
        log("SetPropVal(). Props::port. value:"+ std::to_string(varPropVal->intVal));
        portToS = varPropVal->intVal;
    }
    return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::IsPropReadable(const long lPropNum)
{ 
    log("IsPropReadable(). Nomber: " + std::to_string(lPropNum));
    switch (lPropNum) {
    case Props::port:
        log("IsPropReadable(). Nomber: '" + std::to_string(lPropNum)+"'. Props::port return true.");
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::IsPropWritable(const long lPropNum)
{
    log("IsPropWritable(). Nomber: " + std::to_string(lPropNum));
    switch (lPropNum) {
    case Props::port:
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNMethods()
{ 
    log("GetNMethods()."); 
    return eLastMethod;
}
//---------------------------------------------------------------------------//
long CAddInNative::FindMethod(const WCHAR_T* wsMethodName)
{ 
    std::wstring wstringPropName(wsMethodName);
    std::string stdstringPropName(wstringPropName.begin(), wstringPropName.end());
    log("FindMethod(). name:'" + stdstringPropName + "'");
    if ("runServer" == stdstringPropName) {
        return Methods::runServer;
    }
    return -1;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInNative::GetMethodName(const long lMethodNum, 
                            const long lMethodAlias)
{ 
    log("GetMethodName(). Nomber: " + std::to_string(lMethodNum)+" alias:"+ std::to_string(lMethodAlias));
    WCHAR_T* name = NULL;
    switch (lMethodNum) {
    case Methods::runServer:
        textToReturn(name,L"runServer");
        break;
    }
    return name;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNParams(const long lMethodNum)
{ 
    log("GetNParams(). Nomber: " + std::to_string(lMethodNum));
    return 0;
}
//---------------------------------------------------------------------------//
bool CAddInNative::GetParamDefValue(const long lMethodNum, const long lParamNum,
                          tVariant *pvarParamDefValue)
{ 
    log("GetParamDefValue()."); 
    return false;
} 
//---------------------------------------------------------------------------//
bool CAddInNative::HasRetVal(const long lMethodNum)
{ 
    log("HasRetVal(). Nomber: " + std::to_string(lMethodNum));
    return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsProc(const long lMethodNum,
                    tVariant* paParams, const long lSizeArray)
{ 
    log("CallAsProc(). Nomber: " + std::to_string(lMethodNum));
    switch (lMethodNum) {
    case Methods::runServer:
        run_daemon_server(base);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsFunc(const long lMethodNum,
                tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{ 
    log("CallAsFunc(). Nomber: " + std::to_string(lMethodNum));
    return false;
}
//---------------------------------------------------------------------------//
void CAddInNative::SetLocale(const WCHAR_T* loc)
{
    log("SetLocale(). Not use.");
}
void CAddInNative::textToReturn(WCHAR_T* toReturn, WCHAR_T* usingText)
{
    int iActualSize = wcslen(usingText) + 1;
    this->memManager->AllocMemory((void**)&toReturn, iActualSize * sizeof(WCHAR_T));
    ::convToShortWchar(&toReturn, usingText, iActualSize);
}
//---------------------------------------------------------------------------//
bool CAddInNative::setMemManager(void* mem)
{
    log("setMemManager().");
    memManager = (IMemoryManager*)mem;
    return true;
}
//---------------------------------------------------------------------------//
uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len)
{
    if (!len)
        len = ::wcslen(Source)+1;

    if (!*Dest)
        *Dest = new WCHAR_T[len];

    WCHAR_T* tmpShort = *Dest;
    wchar_t* tmpWChar = (wchar_t*) Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len*sizeof(WCHAR_T));
    do
    {
        *tmpShort++ = (WCHAR_T)*tmpWChar++;
        ++res;
    }
    while (len-- && *tmpWChar);

    return res;
}
//---------------------------------------------------------------------------//
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len)
{
    if (!len)
        len = getLenShortWcharStr(Source)+1;

    if (!*Dest)
        *Dest = new wchar_t[len];

    wchar_t* tmpWChar = *Dest;
    WCHAR_T* tmpShort = (WCHAR_T*)Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len*sizeof(wchar_t));
    do
    {
        *tmpWChar++ = (wchar_t)*tmpShort++;
        ++res;
    }
    while (len-- && *tmpShort);

    return res;
}
//---------------------------------------------------------------------------//
uint32_t getLenShortWcharStr(const WCHAR_T* Source)
{
    uint32_t res = 0;
    WCHAR_T *tmpShort = (WCHAR_T*)Source;

    while (*tmpShort++)
        ++res;

    return res;
}
//---------------------------------------------------------------------------//
