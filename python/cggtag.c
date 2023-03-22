#define PY_SSIZE_T_CLEAN
#include <Python.h>

uint8_t* encode(const char *input, int *length);
uint8_t* render(const char *input, int width, int height);

static PyObject *method_encode(PyObject *self, PyObject *args) {
    const char *str;

    if (!PyArg_ParseTuple(args, "s", &str)) {
        return NULL;
    }
    int n;
    uint8_t* buff = encode(str, &n);
    Py_ssize_t buff_size = n;
    PyObject* result = Py_BuildValue("y#", buff, buff_size);
    free(buff);
    return result;
}

static PyObject *method_render(PyObject *self, PyObject *args) {
    const char *str;
    int width, height;

    if (!PyArg_ParseTuple(args, "sii", &str, &width, &height)) {
        return NULL;
    }
    uint8_t *bitmap = render(str, width, height);
    Py_ssize_t buff_size = 0;
    if (width % 8 == 0) {
        buff_size = (width / 8) * height;
    } else {
        buff_size = (width / 8 + 1) * height;
    }
    PyObject* result = Py_BuildValue("y#", bitmap, buff_size);
    free(bitmap);
    return result;
}

static PyMethodDef EncodeMethods[] = {
    {"encode", method_encode, METH_VARARGS, "Encode text commands to ggtag's binary format"},
    {"render", method_render, METH_VARARGS, "Render text commands to bitmap"},
    {NULL, NULL, 0, NULL}
};


static struct PyModuleDef cggtagmodule = {
    PyModuleDef_HEAD_INIT,
    "cggtag",
    "Python bindings for ggtag",
    -1,
    EncodeMethods
};

PyMODINIT_FUNC PyInit_cggtag(void) {
    return PyModule_Create(&cggtagmodule);
}