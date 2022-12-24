// ApacheLogs.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "ApacheLogs.h"
#include <string>
#include <regex>
#include <fstream>
#include <vector>
#include <iterator>
#include <set>
#include <map>
#include <commctrl.h>

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_APACHELOGS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_APACHELOGS));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APACHELOGS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;//MAKEINTRESOURCEW(IDC_APACHELOGS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED |  WS_CAPTION | WS_SYSMENU |  WS_MINIMIZEBOX,
      CW_USEDEFAULT, 0, 1225, 750, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

std::vector<std::vector<std::wstring>> logs;

int NowLine = 0;
void WriteLogs();
void GetStat();
DWORD WINAPI GetLogs(LPVOID)
{
    while (true)
    {
        std::wifstream fin;
        int OldNowLine = NowLine;
        fin.open("E:\\Programming\\spz\\KURSACH\\ApacheLogs\\ApacheLogs\\Logs.log");
        if (fin.is_open())
        {
            std::wstring text;
            std::getline(fin, text);
            int i = logs.size();
            for (int skip = 0; skip < NowLine; skip++)
            {
                std::getline(fin, text);
            }
            while (!fin.eof())
            {
                std::getline(fin, text);
                std::wcmatch result;
                std::wregex regular(L"([\\w]+\\:\\s)"
                    L"(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\s)"
                    L"(\\[.*\\]\\s)"
                    L"(\".*?\"\\s)"
                    L"([0-9]{1,5}\\s)"
                    L"([0-9]{1,5}\\s)"
                    L"(\".*?\"\\s)"
                    L"(\".*?\")");
                // "LB4var3: 127.0.0.1 [22/Dec/2022:12:26:45 +0200] \"GET / HTTP / 1.1\" 200 740 \" - \" \"Mozilla / 5.0 (Windows NT 10.0; Win64; x64) AppleWebKit / 537.36 (KHTML, like Gecko) Chrome / 108.0.0.0 Safari / 537.36\"";
                if (std::regex_match(text.c_str(), result, regular))
                {
                    logs.push_back(std::vector<std::wstring>());
                    for (int j = 1; j < result.size(); j++)
                    {
                        logs[i].push_back(result[j]);
                    }
                    i++;
                }
                if (text != std::wstring() && text != L"\n" && text != L" ")
                {
                    NowLine++;
                }
            }
        }
        fin.close();
        if (OldNowLine != NowLine)
        {
            WriteLogs();
            GetStat();
        }
        Sleep(5000);
    }
}

HWND Log;
HWND Data;

void GetStat() {
    SendMessage(Data, LB_RESETCONTENT, 0, 0);
    std::multiset<std::wstring> Id;
    std::multiset<std::wstring> Prodject;
    for (int i = 0; i < logs.size(); i++)
    {
        Prodject.insert(logs[i][0]);
        Id.insert(logs[i][1]);
    }
    std::wstring text = L"Количество запросов от каждого пользователя:";
    SendMessage(Data, LB_ADDSTRING, 0, (LPARAM)text.c_str());
    std::map<std::wstring, int> IdMap;
    for (auto it = Id.begin(); it != Id.end(); it++)
    {
        IdMap.insert(std::make_pair(*it, Id.count(*it)));
    }
    for (auto it = IdMap.begin(); it != IdMap.end(); it++)
    {
        text = (*it).first;
        text += (L" - " + std::to_wstring((*it).second));
        SendMessage(Data, LB_ADDSTRING, 0, (LPARAM)text.c_str());
    }

    text = L"";
    SendMessage(Data, LB_ADDSTRING, 0, (LPARAM)text.c_str());
    text = L"Количество запросов в каждом проекте:";
    SendMessage(Data, LB_ADDSTRING, 0, (LPARAM)text.c_str());
    std::map<std::wstring, int> ProdjectMap;
    for (auto it = Prodject.begin(); it != Prodject.end(); it++)
    {
        ProdjectMap.insert(std::make_pair(*it, Prodject.count(*it)));
    }
    for (auto it = ProdjectMap.begin(); it != ProdjectMap.end(); it++)
    {
        text = (*it).first;
        text += (L" - " + std::to_wstring((*it).second));
        SendMessage(Data, LB_ADDSTRING, 0, (LPARAM)text.c_str());
    }
}


LPWSTR StdToWinapiString(std::wstring text) {
    WCHAR* buffer = new WCHAR[text.size() + 1];
    wcscpy_s(buffer, text.size() + 1, text.c_str());
    buffer[text.size()] = '\0';
    return buffer;
}

void CreateHeader() {
    std::wstring text = L"Проект";

    LVCOLUMN LVCOLUMN1;
    LVCOLUMN1.mask = LVCF_WIDTH | LVCF_TEXT;
    LVCOLUMN1.cx = 150;
    LVCOLUMN1.pszText = StdToWinapiString(text);
    ListView_InsertColumn(Log, 0, &LVCOLUMN1);

    text = L"Дата";
    LVCOLUMN LVCOLUMN2;
    LVCOLUMN2.mask = LVCF_WIDTH | LVCF_TEXT;
    LVCOLUMN2.cx = 250;
    LVCOLUMN2.pszText = StdToWinapiString(text);
    ListView_InsertColumn(Log, 1, &LVCOLUMN2);

    text = L"IP";
    LVCOLUMN LVCOLUMN3;
    LVCOLUMN3.mask = LVCF_WIDTH | LVCF_TEXT;
    LVCOLUMN3.cx = 150;
    LVCOLUMN3.pszText = StdToWinapiString(text);
    ListView_InsertColumn(Log, 2, &LVCOLUMN3);

    text = L"Запрос";
    LVCOLUMN LVCOLUMN4;
    LVCOLUMN4.mask = LVCF_WIDTH | LVCF_TEXT;
    LVCOLUMN4.cx = 630;
    LVCOLUMN4.pszText = StdToWinapiString(text);
    ListView_InsertColumn(Log, 3, &LVCOLUMN4);
}

int NowRows = 0;
void WriteLogs() {
    for (int i = NowRows; i < logs.size(); i++)
    {
        LV_ITEM LVInsert;
        LVInsert.mask = LVIF_TEXT | LVIF_IMAGE;
        LVInsert.iItem = i;
        LVInsert.iSubItem = 0; 
        LVInsert.pszText = StdToWinapiString(logs[i][0]);
        ListView_InsertItem(Log, &LVInsert);

        LVITEM LVItemColum1;
        LVItemColum1.mask = LVIF_TEXT;
        LVItemColum1.iItem = i;
        LVItemColum1.pszText = StdToWinapiString(logs[i][2]); 
        LVItemColum1.iSubItem = 1;  
        ListView_SetItem(Log, &LVItemColum1); 

        LVITEM LVItemColum2 = LVItemColum1;
        LVItemColum2.pszText = StdToWinapiString(logs[i][1]); 
        LVItemColum2.iSubItem = 2; 
        ListView_SetItem(Log, &LVItemColum2);

        LVITEM LVItemColum3 = LVItemColum1;
        LVItemColum3.pszText = StdToWinapiString(logs[i][3]);
        LVItemColum3.iSubItem = 3;
        ListView_SetItem(Log, &LVItemColum3);
        NowRows++;
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE: 
    {
        Log = CreateWindowW(WC_LISTVIEWW, L"Log", WS_CHILD | WS_VISIBLE | WS_BORDER |
            LVS_REPORT | WS_VSCROLL,
            0, 0, 1200, 400, hWnd, HMENU(0), NULL, NULL);
        CreateHeader();
        Data = CreateWindowA("listbox", "Data", LBS_DISABLENOSCROLL | WS_CHILD
            | WS_VISIBLE | WS_VSCROLL | WS_BORDER | LBS_NOTIFY,
            0, 400, 1200, 300, hWnd, HMENU(1), NULL, NULL);
        CreateThread(NULL, 0, GetLogs, NULL, 0, 0);
        break;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
