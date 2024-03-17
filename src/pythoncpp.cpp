#include "pythoncpp.hpp"
#include "utils.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <format>

namespace py {
PyObject* openPython() {
    Py_Initialize();
    import("numpy", "np");
    import("matplotlib.pyplot", "plt");
    PyRun_SimpleString("from matplotlib.widgets import Slider");

    return PyImport_AddModule("__main__");
}

void import(std::string module, std::string alias) {
    PyRun_SimpleString(("import " + module + " as " + alias).c_str());
}

void import(std::string module) {
    PyRun_SimpleString(("import " + module).c_str());
}

std::string createVariable() {
    std::string varName = randomString(10);
    PyRun_SimpleString((varName + "= 0" ).c_str());
    return varName;
}

void setDouble(std::string varName, double value) {
    PyRun_SimpleString((varName + "=" + std::to_string(value)).c_str());
}

double getDouble(PyObject *m, std::string varName) {
    PyObject *v = PyObject_GetAttrString(m, varName.c_str());

    double result = PyFloat_AsDouble(v);
    Py_DECREF(v);

    return result;
}

void closePython() {
    Py_Finalize();
    return;
}
}

namespace plt {

std::string figure() {
    std::string varName = randomString(10);
    PyRun_SimpleString((varName + " = plt.figure()").c_str());
    return varName;
}

void ion() {
    PyRun_SimpleString("plt.ion()");
}

void ioff() {
    PyRun_SimpleString("plt.ioff()");
}

std::string axes(double left, double bottom, double width, double height) {
    std::string varName = randomString(10);
    PyRun_SimpleString((varName + "= plt.axes([" + std::to_string(left) + "," + std::to_string(bottom) + "," + std::to_string(width) + ","+ std::to_string(height) + "])").c_str());
    return varName;
}

std::string Slider(std::string ax, std::string label, double valmin, double valmax, std::string varToUpdate) {
    std::string varName = randomString(10);
    std::string updateFunctionName = randomString(10);
   
    std::string sliderCreation = std::format("{} = Slider({}, '{}', {}, {}, valinit={})", varName, ax, label, valmin, valmax, varToUpdate);

    PyRun_SimpleString(("def " + updateFunctionName + "(value):\n\tglobal " + varToUpdate + "\n\t" + varToUpdate + "= value").c_str());
    PyRun_SimpleString(sliderCreation.c_str());
    PyRun_SimpleString((varName + ".on_changed(" + updateFunctionName + ")").c_str());

    return varName;
}

void plot(std::string x, std::string y, std::string label) {
    std::ostringstream ss;
    ss << "plt.plot(" << x << "," << y << ",label='" << label << "')";
    PyRun_SimpleString(ss.str().c_str());
}

void imshow(std::string X, std::string cmap) {
    PyRun_SimpleString(("plt.imshow(" + X + ",cmap='" + cmap + "', vmin=-1, vmax=1)").c_str());
}

void show() {
    PyRun_SimpleString("plt.show()");
}

void clf() {
    PyRun_SimpleString("plt.clf()");
}

void pause() {
    PyRun_SimpleString("plt.pause(0.01)");
}

void title(std::string title) {
    PyRun_SimpleString(("plt.title('" + title + "')").c_str());
}

void xlabel(std::string xlabel)
{
    PyRun_SimpleString(("plt.xlabel('" + xlabel + "')").c_str());
}

void ylabel(std::string ylabel)
{
    PyRun_SimpleString(("plt.ylabel('" + ylabel + "')").c_str());
}

void colorbar() {
    PyRun_SimpleString("plt.colorbar()");
}
}

namespace np {
std::string array(int **data, uint rows, uint columns)
{
    std::string varName = randomString(10);
    array(varName, data, rows, columns);
    
    return varName;
}

void array(std::string varName, int **data, uint rows, uint columns)
{
    std::ostringstream ss;
    ss << "np.array([";
    for (size_t y = 0; y < rows; y++)
    {
        for (size_t x = 0; x < columns; x++)
        {
            ss << data[y][x];
            if (y != rows - 1 || x != columns -1) {
                ss << ",";
            }
        }
    }
    ss << "])";

    PyRun_SimpleString((varName + "=" + ss.str()).c_str());
    reshape(varName, rows, columns);
}

std::string array(double *data, uint length)
{
    std::string varName = randomString(10);
    array(varName, data, length);
    
    return varName;
}

void array(std::string varName, double *data, uint length)
{
    std::ostringstream ss;
    ss << "np.array([";
    for (size_t i = 0; i < length; i++)
    {
        ss << data[i];
        if (i != length - 1) {
            ss << ",";
        }
    }
    ss << "])";

    PyRun_SimpleString((varName + "=" + ss.str()).c_str());
}

void reshape(std::string varName, uint rows, uint columns) {
    PyRun_SimpleString((varName + "= np.reshape(" + varName + ",(" + std::to_string(rows) + "," + std::to_string(columns) + "))").c_str());
}

void savetxt(std::string varName, std::string filename, std::string delimiter) {
    std::string command = std::format("np.savetxt('{}', {}, delimiter='{}')", filename, varName, delimiter);
    PyRun_SimpleString(command.c_str());
}
}