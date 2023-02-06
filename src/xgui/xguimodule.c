#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <windows.h>
#include <structmember.h>


typedef struct {
    PyObject_HEAD
    PyObject *window_name;
    PyObject *window_class;
    int x;
    int y;
    int width;
    int height;

} GuiObject;

static void Gui_dealloc(GuiObject *self) {
    Py_XDECREF(self->window_name);
    Py_XDECREF(self->window_class);
    Py_TYPE(self)->tp_free((PyObject *) self);
}


static PyObject *Gui_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    GuiObject *self;
    self = (GuiObject *) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->window_name = PyUnicode_FromString("Xgui Window");
        if (self->window_name == NULL) {
            Py_DECREF(self);
            return NULL;
        }


        self->window_class = PyUnicode_FromString("Xgui Window");
        if (self->window_class == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->x = 100;
        self->y = 100;
        self->width = 400;
        self->height = 400;
    }
    return (PyObject *) self;
}

static int Gui_init(GuiObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"window_name", "window_class", "x", "y", "width", "height", NULL};
    PyObject *window_name = NULL, *window_class = NULL, *tmp;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|Oiiii", kwlist, &window_name, &window_class,
                                     &self->x, &self->y, &self->width, &self->height)) {
        PyErr_BadArgument();
        return -1;
    }
    if (window_name) {
        tmp = self->window_name;
        self->window_name = window_name;
        Py_XDECREF(tmp);
    }
    if (window_class) {
        tmp = self->window_class;
        self->window_class = window_class;
        Py_XDECREF(tmp);
    }
    return 0;
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    GuiObject *self = GetPropA(hwnd, "GuiObject"); // is null for the first couple of loops
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
        default:
            NULL;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);

}

static PyObject *Gui_create_window(GuiObject *self, PyObject *Py_UNUSED) {

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
    wc.lpszClassName = PyUnicode_AsUTF8(self->window_class);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        PyErr_ProgramText("guimodule.c", 45);
        PyErr_SetString(PyExc_TypeError, "Window Registration Failed!");
        return 0;
    }
    HWND hwnd = CreateWindowEx(0, (LPCSTR) PyUnicode_AsUTF8(self->window_class), PyUnicode_AsUTF8(self->window_name),
                               WS_OVERLAPPEDWINDOW, self->x, self->y, self->width, self->height, NULL,
                               NULL, hInstance, NULL);
    if (hwnd == NULL) {
        PyErr_ProgramText("guimodule.c", 48);
        PyErr_SetString(PyExc_TypeError, "Window Creation Failed!");
        return 0;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    printf("l128");
    SetPropA(hwnd, "GuiObject", self);
    printf("l129");
    MSG msg;
    while (GetMessage(&msg, hwnd, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return PyLong_FromUnsignedLongLong(msg.wParam);
}

static PyMethodDef GuiMethods[] = {
        {"create_window", (PyCFunction) (void (*)(void)) Gui_create_window, METH_NOARGS, PyDoc_STR(
                                                                                                 "Creates a window.")},
        {NULL, NULL, 0,                                                                  NULL}        /* Sentinel */
};
static PyMemberDef GuiMembers[] = {
        {"window_name",  T_OBJECT_EX, offsetof(GuiObject, window_name),  0, "Name of the window"},
        {"window_class", T_OBJECT_EX, offsetof(GuiObject, window_class), 0, "Window classname"},
        {"x",            T_INT,       offsetof(GuiObject, x),            0, "X coordinate of the window"},
        {"y",            T_INT,       offsetof(GuiObject, y),            0, "Y coordinate of the window"},
        {"width",        T_INT,       offsetof(GuiObject, width),        0, "Width of the window"},
        {"height",       T_INT,       offsetof(GuiObject, height),       0, "Height of the window"},
        {NULL}
};

static PyTypeObject GuiType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "gui.Gui",
        .tp_doc = PyDoc_STR("Main class for gui module"),
        .tp_basicsize = sizeof(GuiObject),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new = Gui_new,
        .tp_init = (initproc) Gui_init,
        .tp_dealloc = (destructor) Gui_dealloc,
        .tp_methods = GuiMethods,
        .tp_members = GuiMembers,
};


static PyMethodDef XguiMethods[] = {
        {NULL, NULL, 0, NULL}        /* Sentinel */
};
static struct PyModuleDef guimodule = {
        PyModuleDef_HEAD_INIT,
        "gui",   /* name of module */
        NULL, /* module documentation, may be NULL */
        -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
        XguiMethods
};

PyMODINIT_FUNC PyInit_gui(void) {
    PyObject *m;
    if (PyType_Ready(&GuiType) < 0)
        return NULL;
    m = PyModule_Create(&guimodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&GuiType);
    if (PyModule_AddObject(m, "Gui", (PyObject *) &GuiType) < 0) {
        Py_DECREF(&GuiType);
        Py_DECREF(m);
        return NULL;
    }
    return m;
}


int main(int argc, char *argv[]) {
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }

    /* Add a built-in module, before Py_Initialize */
    if (PyImport_AppendInittab("gui", PyInit_gui) == -1) {
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
    PyObject *pmodule = PyImport_ImportModule("gui");
    if (!pmodule) {
        PyErr_Print();
        fprintf(stderr, "Error: could not import module 'gui'\n");
    }


    PyMem_RawFree(program);
    return 0;
}