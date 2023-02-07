#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <windows.h>
#include <structmember.h>

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            printf("Close triggered by WM_DESTROY");
            PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;

            HBRUSH hbrush = CreateSolidBrush(RGB(255, 255, 255));

            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, hbrush);
            EndPaint(hwnd, &ps);
        }
            return 0;
        default:
            NULL;
    }

    PyObject *callback = GetPropA(hwnd, "callback"); // is null for the first couple of loops
    PyObject *self = GetPropA(hwnd, "self");
    if (callback != NULL && self != NULL) {

        PyObject *tuple = Py_BuildValue("(I)", uMsg);
        PyObject_CallObject(callback, tuple);
        Py_DECREF(tuple);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);

}

static PyObject *destroy_window(PyObject *dummy, PyObject *args) {
    char *window_class, *window_name;
    if (!PyArg_ParseTuple(args, "ss", &window_class, &window_name)) {
        PyErr_SetString(PyExc_TypeError, "Bad Argument!");
        return NULL;
    }
    printf("Attempting to destroy window %s\n", window_class);
    HWND hwnd = FindWindowW((LPCWSTR) window_class, (LPCWSTR) window_name);
    if (!DestroyWindow(hwnd)) {
        PyErr_SetString(PyExc_Exception, "Failed to destroy window: %s");
        return NULL;
    }
    printf("Destroyed window with %s\n", window_class);
    return Py_None;
}

static PyObject *create_window(PyObject *dummy, PyObject *args) {
    char *window_class, *window_name;
    int x, y, width, height;
    PyObject *callback, *self;

    if (!PyArg_ParseTuple(args, "ssiiiiOO", &window_class, &window_name, &x, &y, &width, &height, &callback, &self)) {
        PyErr_BadArgument();
        return NULL;
    }

    WNDCLASSEX wc;
    HMODULE hInstance = 0;
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return 0;
    }

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = window_class;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        PyErr_ProgramText("guimodule.c", 45);
        PyErr_SetString(PyExc_TypeError, "Window Registration Failed!");
        return 0;
    }
    HWND hwnd = CreateWindowEx(0, (LPCSTR) window_class, window_name,
                               WS_OVERLAPPEDWINDOW, x, y, width, height, NULL,
                               NULL, hInstance, NULL);
    if (hwnd == NULL) {
        PyErr_ProgramText("guimodule.c", 48);
        PyErr_SetString(PyExc_TypeError, "Window Creation Failed!");
        return 0;
    }
    printf("Creating window with class %s\n", window_class);
    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);
    SetPropA(hwnd, "callback", callback);
    SetPropA(hwnd, "self", self);

    MSG msg;
    while (GetMessage(&msg, hwnd, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return Py_None;
}

static PyMethodDef CExtMethods[] = {
        {"create_window",  (PyCFunction) (void (*)(void)) create_window,  METH_VARARGS},
        {"destroy_window", (PyCFunction) (void (*)(void)) destroy_window, METH_VARARGS},
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef CExtModule = {
        PyModuleDef_HEAD_INIT,
        "cext",
        NULL,
        -1,
        CExtMethods

};

PyMODINIT_FUNC PyInit_cext(void) {
    return PyModule_Create(&CExtModule);
}

int
main(int argc, char *argv[]) {
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }

    /* Add a built-in module, before Py_Initialize */
    if (PyImport_AppendInittab("cext", PyInit_cext) == -1) {
        fprintf(stderr, "Error: could not extend in-built modules table\n");
        exit(1);
    }

    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(program);

    /* Initialize the Python interpreter.  Required.
       If this step fails, it will be a fatal error. */
    Py_Initialize();

    /* Optionally import the module; alternatively,
       import can be deferred until the embedded script
       imports it. */
    PyObject *pmodule = PyImport_ImportModule("cext");
    if (!pmodule) {
        PyErr_Print();
        fprintf(stderr, "Error: could not import module 'cext'\n");
    }

    PyMem_RawFree(program);
    return 0;
}