// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� AUTOWINDOWDLL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// AUTOWINDOWDLL_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef AUTOWINDOWDLL_EXPORTS
#define AUTOWINDOWDLL_API __declspec(dllexport)
#else
#define AUTOWINDOWDLL_API __declspec(dllimport)
#endif

// �����Ǵ� AutoWindowDll.dll ������
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
