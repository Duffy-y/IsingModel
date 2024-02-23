#include "pythoncpp.hpp"
#include "utils.hpp"

#include <fstream>
#include <sstream>

namespace plt {

void openPython(int realTime) {
    Py_Initialize();
    PyRun_SimpleString("import numpy as np");
    PyRun_SimpleString("import matplotlib.pyplot as plt");

    if (realTime)
        PyRun_SimpleString("plt.ion()");

    PyRun_SimpleString("fig = plt.figure()");

    return;
}

void closePython() {
    Py_Finalize();
    return;
}

void plot(std::string x, std::string y, std::string label) {
    std::ostringstream ss;
    ss << "plt.plot(" << x << "," << y << ",label='" << label << "')";
    PyRun_SimpleString(ss.str().c_str());
}

void imshow(std::string X, std::string cmap) {
    PyRun_SimpleString(("plt.imshow(" + X + ",cmap='" + cmap + "')").c_str());
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
}

namespace np {
std::string loadtxt(int **array, uint rows, uint columns)
{
    std::string varName = randomString(10);
    loadtxt(varName, array, rows, columns);
    
    return varName;
}

void loadtxt(std::string varName, int **array, uint rows, uint columns)
{
    std::ostringstream ss;
    ss << "np.array([";
    for (size_t y = 0; y < rows; y++)
    {
        for (size_t x = 0; x < columns; x++)
        {
            ss << array[y][x];
            if (y != rows - 1 || x != columns -1) {
                ss << ",";
            }
        }
    }
    ss << "])";

    PyRun_SimpleString((varName + "=" + ss.str()).c_str());
    reshape(varName, rows, columns);
}

std::string loadtxt(double *array, uint length)
{
    std::string varName = randomString(10);
    loadtxt(varName, array, length);
    
    return varName;
}

void loadtxt(std::string varName, double *array, uint length)
{
    std::ostringstream ss;
    ss << "np.array([";
    for (size_t i = 0; i < length; i++)
    {
        ss << array[i];
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
}