#include <Windows.h>
#include <string.h>
#include <stdio.h>
 
FILE *fp;
 
void FindFile(wchar_t* path);
void main()
{
    // 파일리스트를 파일로 저장합니다.
    fp = _wfopen(L"filelist.txt", L"w+");
    // 특정경로지정
    wchar_t path[256] = L"C:\\Patch\\";
 
    FindFile(path);
    fclose(fp);
    
}
void FindFile(wchar_t* path)
{
    WIN32_FIND_DATA fd;
 
    wchar_t path2[256];
    wsprintf(path2, L"%s%s", path, L"*.*" );
 
    HANDLE hFind = ::FindFirstFile(path2, &fd);
 
    if( hFind != INVALID_HANDLE_VALUE )
    {
        do
        {
            if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                // 콘솔창에서 출력
                //wprintf(L"%s\n", fd.cFileName);
                // 파일로 저장
                fwprintf(fp, L"%s\n", fd.cFileName);
            }
            else
            {
                if( fd.cFileName[0] != '.')
                {
 
                wchar_t path3[256];
                wsprintf(path3, L"%s%s\\", path, fd.cFileName );
                //재귀호출합니다.
                FindFile(path3);
                }
            }
        }while(::FindNextFile(hFind,&fd));
        ::FindClose(hFind);
    }
}
