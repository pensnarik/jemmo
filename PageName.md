# Jemmo #

'''Jemmo''' - программа для быстрого просмотра JPEG файлов.

Для того, чтобы написать её, нужно разобраться в нескольких вещах.

**Создание окна с помощью CreateWindow** Рисование на ClientRec (терминология, функции Win32 API)
**Своболные библиотеки (своя реализация, как в ACDSee?)**

Начнем с оконных функций Win32.

## Создание окна ##

Сначала попробуем почерпнуть необходимую информацию из книги Ганеева "Проектирование пользовательского интерфейса средствами Win32 API".

Будем использовать функцию CreateWindow.

```
HWND CreateWindow(      
    LPCTSTR lpClassName,
    LPCTSTR lpWindowName,
    DWORD dwStyle,
    int x,
    int y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam
);
```

lpClassName - указатель на имя класса (зарегистрированного RegisterClass или '''определенного операционной системой''').

## Работа с изображениями ##

### Загрузка ###

Попробуем, как и в ACDSee, использовать для работы с JPEG файлами библиотеку jpeglib. Вроде подключил и даже что-то рисовалось, как тут как обычно что-то решил поменять и теперь даже не компилится, не то что не рисуется. Будем разбираться с этим завтра.

Есть предположение, что нужно играться с флагами компилятора.