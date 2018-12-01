#include<stdio.h>
#include<inttypes.h>
#define WIN32_LEAN_AND_MEAN
#include<windows.h>
#include <ImageHlp.h>
#pragma comment(lib, "ImageHlp")
#include<winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#include"base.h"
#include"api_log_demo.h"
#include"log.h"

typedef int (*CONNTYPE)(SOCKET s,const struct sockaddr*name,int namelen);
typedef int (*RECVTYPE)( SOCKET s, char   *buf, int    len, int    flags);
 int myconnect(SOCKET s,const struct sockaddr*name,int namelen);
HMODULE g_hModule;
CONNTYPE conn0;

bool check_change_addr(short aod,void *addr);

/*extern "C" __declspec(dllexport) */ void show_module_functions()
{
    LOG_DEBUG("%s\n", __FUNCTION__);
    // 这里为了示例，取得主模块的模块句柄
    HMODULE hMod = ::GetModuleHandle(NULL);
    IMAGE_DOS_HEADER *pDosHeader = (IMAGE_DOS_HEADER *)hMod;
   // IMAGE_OPTIONAL_HEADER *pOptHeader =(IMAGE_OPTIONAL_HEADER *)((BYTE *)hMod + pDosHeader->e_lfanew + 24);
    //IMAGE_IMPORT_DESCRIPTOR *pImportDesc = (IMAGE_IMPORT_DESCRIPTOR *)((BYTE *)hMod + pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    unsigned long rsize=0;
    IMAGE_IMPORT_DESCRIPTOR *pImportDesc = (IMAGE_IMPORT_DESCRIPTOR *)ImageDirectoryEntryToData(hMod,TRUE,IMAGE_DIRECTORY_ENTRY_IMPORT,&rsize);
    while (pImportDesc->FirstThunk)
    {
        char *pszDllName = (char *)((BYTE *)hMod + pImportDesc->Name);
        LOG_DEBUG("\n 模块名称:%s\n", pszDllName);
        // 一个IMAGE_THUNK_DATA 就是一个unsigned long long(64bit) or unsigned long(32bit)，它指定了一个导入函数
        IMAGE_THUNK_DATA *pThunk = (IMAGE_THUNK_DATA *)((BYTE *)hMod + pImportDesc->OriginalFirstThunk);
        int n = 0;
//        {
//            DWORD dwOldProtect;
//            MEMORY_BASIC_INFORMATION mbi;
//            VirtualQuery(pThunk, &mbi, sizeof(mbi));
//            VirtualProtect(pThunk, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);
//            LOG_DEBUG("%u=>%u\n",dwOldProtect, PAGE_READWRITE);
//            int a=dwOldProtect;
//        }
        while (pThunk->u1.Function)
        { 
            LOG_DEBUG("sizeof(pThunk->u1.Function=%d\t",(int)sizeof(pThunk->u1.Function));
            if(IS32BIT)
                LOG_DEBUG("thunk_data.function=%X\t",pThunk->u1.AddressOfData);
            if(IS64BIT)
                LOG_DEBUG("thunk_data.function=%llX\t",pThunk->u1.AddressOfData);
            //最高位如果是1，按ordinal方式导入函数，低32位的最高位是0，按函数名导入。***************
            
            ULONGLONG FLAG=(IS32BIT?0x80000000ull:0x8000000000000000ull);
            if(pThunk->u1.AddressOfData & FLAG)
            {//ordinal
                
            }
            else
            {
                // 取得函数名称。hint/name 表项前2 个字节是函数序号，后面才是函数名称字符串
                //char *pszFunName = (char *)((BYTE *)hMod + (DWORD)pThunk->u1.AddressOfData + 2);
                IMAGE_IMPORT_BY_NAME *ibn = (IMAGE_IMPORT_BY_NAME *)((BYTE *)hMod + (DWORD)pThunk->u1.AddressOfData);
                char *pszFunName = (char *)ibn->Name;//这个Name在头文件里面只有一个字节，但其实长度是不固定的，它存储的是一个字符串
                LOG_DEBUG("从此模块导入的函数：%-25s,", pszFunName);
            }

            void *mcaddr = 0; //may be changed addr
                              // 取得函数地址。IAT 表就是一个DWORD or ULONGLONG类型的数组，每个成员记录一个函数的地址
            if (IS64BIT)
            {
                PULONG64 lpAddr = (PULONG64)((BYTE *)hMod + pImportDesc->FirstThunk) + n;
                LOG_DEBUG("函数地址:%llX\n", *lpAddr);
                mcaddr = lpAddr;
            }
            else
            {
                PDWORD lpAddr = (DWORD *)((BYTE *)hMod + pImportDesc->FirstThunk) + n;
                LOG_DEBUG("函数地址:%X\n", *lpAddr);
                mcaddr = lpAddr;
            }
            {
                //try to change the address
                if (FLAG && stricmp("WS2_32.dll", pszDllName) == 0 && !check_change_addr(pThunk->u1.AddressOfData, mcaddr))
                {
                    LOG_ERROR("check change addr error:%llu",(ULONGLONG)pThunk->u1.AddressOfData);
                }
            }
            n++;
            pThunk++;
        }
        pImportDesc++;
    }
    LOG_DEBUG("--------------------------------\n");
}
bool check_change_addr(short aod, void *addr)
{
    if (aod == 4)
    {
        if(IS64BIT)
        {
            conn0 = (CONNTYPE)(*(ULONGLONG*)addr);
            LOG_DEBUG("start change connect addr...., last_addr=%llX",*(PULONGLONG)addr);
        }
        else 
        {
            conn0=(CONNTYPE)(*(ULONG*)addr);
            LOG_DEBUG("start change connect addr...., last_addr=%X",*(PULONG)addr);

        }
        { // 修改页的保护属性
            DWORD dwOldProtect;
            MEMORY_BASIC_INFORMATION mbi;
            ::VirtualQuery(addr, &mbi, sizeof(mbi));
            ::VirtualProtect(addr, sizeof(void*), PAGE_READWRITE, &dwOldProtect);
//the following line will lead to a crash
//            ::WriteProcessMemory(::GetCurrentProcess(), addr, &myconnect, sizeof(void*), NULL);
            unsigned long long t=(ULONG64)&myconnect;
            LOG_DEBUG("t=%llX\n",t);
            ::WriteProcessMemory(::GetCurrentProcess(), addr,&t, sizeof(void*), NULL);
            ::VirtualProtect(addr, sizeof(void*), dwOldProtect, 0);
        }
        //the following code will lead to crash
        //        if (IS64BIT)
        //            *(PULONGLONG)addr = (ULONGLONG)myconnect;
        //        if(IS32BIT)
        //            *(unsigned int*)addr=(unsigned int)myconnect;
        if (IS64BIT)
            LOG_DEBUG("address has changed to=%llX, &myconnct=%p{%llX}", *(PULONGLONG)addr, myconnect, &myconnect);
        else
            LOG_DEBUG("address has changed to=%X, &myconnct=%p{%X}", *(PULONG)addr, myconnect, &myconnect);
    }
//    else if(aod==16)
//    {
//        recv0=(RECVTYPE)addr;
//        *addr=myrecv;
//    }
    return true;
}

BOOL WINAPI DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if(!gp_log)
    {
        gp_log=new ns_log::MLog("D:\\github\\net_hook_redirect\\build64\\Debug\\log.hook_dll");
    }
    LOG_DEBUG("DLL main run=>");

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    // 应用程序创建了一个新的线程
        g_hModule = (HMODULE)hModule; 
        LOG_DEBUG("process_attach\n");
        show_module_functions();
        break;
    case DLL_THREAD_ATTACH:
    // 应用程序某个线程正常终止
        LOG_DEBUG("thread_attach\n");
        break;
    case DLL_THREAD_DETACH:
    // 动态键接库将被卸载
        LOG_DEBUG("thread_detach\n");
        break;
    case DLL_PROCESS_DETACH:
        LOG_DEBUG("process_detach\n");
        break;
    }
    while(true)
    {
        LOG_DEBUG("running");
        Sleep(1000);
    }
    return TRUE;
}
int myconnect(SOCKET s,const struct sockaddr*name,int namelen)
{
    LOG_DEBUG("get hook");
    struct sockaddr_in dst;
    memcpy(&dst,name,namelen);
    unsigned int ip=dst.sin_addr.S_un.S_addr;
    unsigned short port=dst.sin_port;
    dst.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
    inet_pton(AF_INET,"127.0.0.1",&dst.sin_addr);
    dst.sin_port=htons(80);
    {
        unsigned char*b=(unsigned char*)&ip;
        LOG_DEBUG("origin connecting to %hhu.%hhu.%hhu.%hhu:%hu\n",b[0],b[1],b[2],b[3],htons(port));
    }
    return conn0(s,(struct sockaddr*)&dst,sizeof(dst));
}