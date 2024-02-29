#pragma once

#include <Python.h>
#include <string>

// Ce fichier d'en-tête à pour but de transposer à la quasi-identité les fonctions de Matplotlib et Numpy nécessaire pour le projet.
// L'objectif est simplement d'exposer des fonctions C/C++ pour exécuter des fonctions Python en arrière.

namespace py {
/// @brief Initialise l'API Python-C
PyObject* openPython();

void import(std::string module, std::string alias);

void import(std::string module);

std::string createVariable();

void setDouble(std::string varName, double value);

double getDouble(PyObject *m, std::string varName);

/// @brief Ferme l'API Python-C
void closePython();
}

namespace plt {

std::string figure();

void ion();

void ioff();

std::string axes(double left, double bottom, double width, double height);

std::string Slider(std::string ax, std::string label, double valmin, double valmax, std::string varToUpdate);

void plot(std::string x, std::string y, std::string label);

void imshow(std::string X, std::string cmap);

void show();

void clf();

void pause();

void title(std::string title);

void xlabel(std::string xlabel);

void ylabel(std::string ylabel);
}

namespace np {

/// @brief Transfère un tableau 2D C++ dans l'interpréteur Python
/// @param array Tableau de valeurs à transférer
/// @param rows Nombre de lignes
/// @param columns Nombre de colonnes
/// @return Nom de la variable Python créée
std::string array(int **data, uint rows, uint columns);

/// @brief Transfère un tableau 2D C++ dans l'interpréteur Python
/// @param array Tableau de valeurs à transférer
/// @param rows Nombre de lignes
/// @param columns Nombre de colonnes
/// @return Nom de la variable Python créée
void array(std::string varName, int **data, uint rows, uint columns);

/// @brief Transfère un tableau 1D C++ dans l'interpréteur Python
/// @param array Tableau de valeurs à transférer
/// @param length Nombre de lignes
/// @return Nom de la variable Python créée
std::string array(double *data, uint length);

/// @brief Transfère un tableau 1D C++ dans l'interpréteur Python
/// @param array Tableau de valeurs à transférer
/// @param length Nombre de lignes
/// @return Nom de la variable Python créée
void array(std::string varName, double *data, uint length);

/// @brief Reshape un tableau Numpy dans Python
/// @param varName Nom du tableau Numpy
/// @param rows Nombre de lignes
/// @param columns Nombre de colonnes
void reshape(std::string varName, uint rows, uint columns);

}