// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 AUTOWINDOWDLL_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// AUTOWINDOWDLL_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef AUTOWINDOWDLL_EXPORTS
#define AUTOWINDOWDLL_API __declspec(dllexport)
#else
#define AUTOWINDOWDLL_API __declspec(dllimport)
#endif

// 此类是从 AutoWindowDll.dll 导出的
class AUTOWINDOWDLL_API CAutoWindowDll {
public:
	CAutoWindowDll(void);

};
HHOOK hhook = nullptr;

extern AUTOWINDOWDLL_API int nAutoWindowDll;

extern "C"
{
	AUTOWINDOWDLL_API void Install(void);
	AUTOWINDOWDLL_API void Unstall(void);
}
AUTOWINDOWDLL_API LRESULT CALLBACK OnWindowCBT(int nCode,WPARAM wParam,LPARAM lParam);
