#include<iostream>
#include<stdio.h>
#include<string.h>
#include <windows.h>
#include <tlhelp32.h>
#include<stdlib.h>

#include "RemThreadInjector.h"
using namespace std;
void print_help()
{
    printf("usage:\n");
    printf("hook_redirect.exe targe_process_name\n");
}
int main(int argc,char*argv[])
{
    printf("I'm hook_redirect\n");
    if(argc!=2)
    {
        printf("must give the processname which you wnat to hook_redirect\n");
        print_help();
        return -1;
    }

    unsigned int target_pid=0;
    const char*target_name=argv[1];
    {
        //get the target_process id
        PROCESSENTRY32 pe32;
        // 在使用这个结构之前，先设置它的大小
        pe32.dwSize = sizeof(pe32);

        // 给系统内的所有进程拍一个快照
        HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hProcessSnap == INVALID_HANDLE_VALUE)
        {
            printf("CreateToolhelp32Snapshot调用失败!\n");
            return -1;
        }
        // 遍历进程快照，轮流显示每个进程的信息
        BOOL bMore = ::Process32First(hProcessSnap, &pe32);
        while (bMore)
        {
            //printf("进程名称:%s\n", pe32.szExeFile);
            //printf("进程ID:%u\n\n", pe32.th32ProcessID);
            if(strstr(pe32.szExeFile,target_name))
            {
                target_pid=pe32.th32ProcessID;
            }
            bMore = ::Process32Next(hProcessSnap, &pe32);
        }
        // 不要忘记清除掉snapshot 对象
        ::CloseHandle(hProcessSnap);
    }
    if(target_pid==0)
    {
        printf("can't find the target pid for :%s\n",target_name);
        return -1;
    }
    else{
        printf("finded the target_pid {%u}\n",target_pid);
    }

    char abs_path[MAX_PATH];
    if(!_fullpath(abs_path,"./hook_dll.dll",MAX_PATH))
    {
        printf("get full path error\n");
        return -1;
    }
    printf("abspath=%s\n",abs_path);
//    char exeFullPath[MAX_PATH];
//    GetModuleFileName(NULL,exeFullPath,MAX_PATH);   
//    cout<<exeFullPath<<endl;
//    return 0;
    CRemThreadInjector cri(abs_path);
    cri.InjectModuleInto(target_pid);
    cri.EjectModuleFrom(target_pid);
    return 0;
}