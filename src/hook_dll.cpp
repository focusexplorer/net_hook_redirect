#include<stdio.h>
#include<windows.h>
#include <ImageHlp.h>
#include"base.h"
#pragma comment(lib, "ImageHlp")
HMODULE g_hModule;
/*extern "C" __declspec(dllexport) */ void show_module_functions()
{
    printf("%s\n", __FUNCTION__);
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
        printf("\n 模块名称:%s\n", pszDllName);
        // 一个IMAGE_THUNK_DATA 就是一个unsigned long long，它指定了一个导入函数
        IMAGE_THUNK_DATA *pThunk = (IMAGE_THUNK_DATA *)((BYTE *)hMod + pImportDesc->OriginalFirstThunk);
        int n = 0;
//        {
//            DWORD dwOldProtect;
//            MEMORY_BASIC_INFORMATION mbi;
//            VirtualQuery(pThunk, &mbi, sizeof(mbi));
//            VirtualProtect(pThunk, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);
//            printf("%u=>%u\n",dwOldProtect, PAGE_READWRITE);
//            int a=dwOldProtect;
//        }
        while (pThunk->u1.Function)
        { 
            printf("sizeof(pThunk->u1.Function=%d\t",(int)sizeof(pThunk->u1.Function));
            if(IS32BIT)
                printf("thunk_data.function=%X\t",pThunk->u1.AddressOfData);
            if(IS64BIT)
                printf("thunk_data.function=%llX\t",pThunk->u1.AddressOfData);
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
                printf("从此模块导入的函数：%-25s,", pszFunName);

            }

                // 取得函数地址。IAT 表就是一个DWORD or ULONGLONG类型的数组，每个成员记录一个函数的地址
               if(IS64BIT) 
               {
                    PULONG64 lpAddr = (PULONG64)((BYTE *)hMod + pImportDesc->FirstThunk) + n;
                    printf("函数地址:%llX\n", *lpAddr);
               }
               else
               {
                    PDWORD lpAddr = (DWORD *)((BYTE *)hMod + pImportDesc->FirstThunk) + n;
                    printf("函数地址:%X\n", *lpAddr);
               }
            n++;
            pThunk++;
        }
        pImportDesc++;
    }
    printf("--------------------------------\n");
}
BOOL WINAPI DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    printf("DLL main run=>");
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    // 应用程序创建了一个新的线程
        g_hModule = (HMODULE)hModule; 
        printf("process_attach\n");
        show_module_functions();
        break;
    case DLL_THREAD_ATTACH:
    // 应用程序某个线程正常终止
        printf("thread_attach\n");
        break;
    case DLL_THREAD_DETACH:
    // 动态键接库将被卸载
        printf("thread_detach\n");
        break;
    case DLL_PROCESS_DETACH:
        printf("process_detach\n");
        break;
    }
    return TRUE;
}