#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <windows.h>

static PyObject *xgui_system(PyObject *self, PyObject *args);

static PyObject *xgui_init(PyObject *self, PyObject *args, PyObject *keywds);

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static PyObject *xgui_system(PyObject *self, PyObject *args) {
    const char *command;
    int sts;

    if (!PyArg_ParseTuple(args, "s", &command))
        return NULL;
    sts = system(command);
    return PyLong_FromLong(sts);
}


static PyObject *xgui_init(PyObject *self, PyObject *args, PyObject *keywds) {

    const char *window_name;
    const char *window_class = "XGui Window";
    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;
    int width = CW_USEDEFAULT;
    int height = CW_USEDEFAULT;
    static char *kwlist[] = {"window_name", "window_class", "x", "y", "width", "height", NULL};
    int ok = PyArg_ParseTupleAndKeywords(args, keywds, "s|siiii", kwlist, &window_name, &window_class, &x, &y, &width,
                                         &height);
    if (!ok) {
        PyErr_BadArgument();
        return 0;
    }
    system("echo l35");
    WNDCLASSEX wc;
    HMODULE hInstance = 0;

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
        PyErr_ProgramText("xguimodule.c", 45);
        PyErr_SetString(PyExc_TypeError, "Window Registration Failed!");
        return 0;
    }
    HWND hwnd = CreateWindowEx(0, (LPCSTR) window_class, window_name, WS_OVERLAPPEDWINDOW, x, y, width, height, NULL,
                               NULL, hInstance, NULL);

    if (hwnd == NULL) {
        PyErr_ProgramText("xguimodule.c", 48);
        PyErr_SetString(PyExc_TypeError, "Window Creation Failed!");
        return 0;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, hwnd, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return PyLong_FromUnsignedLongLong(msg.wParam);
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));
            EndPaint(hwnd, &ps);
        }
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);

}


static PyMethodDef XguiMethods[] = {
        {
                "system", xgui_system,                              METH_VARARGS,
                "Execute a shell command."
        },
        {       "init",   (PyCFunction) (void (*)(void)) xgui_init, METH_VARARGS | METH_KEYWORDS, "dadad"},
        {NULL, NULL, 0,                                                                           NULL}        /* Sentinel */
};
static struct PyModuleDef xguimodule = {
        PyModuleDef_HEAD_INIT,
        "xgui",   /* name of module */
        NULL, /* module documentation, may be NULL */
        -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
        XguiMethods
};

PyMODINIT_FUNC PyInit_xgui(void) {
    return PyModule_Create(&xguimodule);
}


int main(int argc, char *argv[]) {
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }

    /* Add a built-in module, before Py_Initialize */
    if (PyImport_AppendInittab("xgui", PyInit_xgui) == -1) {
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
    PyObject *pmodule = PyImport_ImportModule("xgui");
    if (!pmodule) {
        PyErr_Print();
        fprintf(stderr, "Error: could not import module 'xgui'\n");
    }


    PyMem_RawFree(program);
    return 0;
}