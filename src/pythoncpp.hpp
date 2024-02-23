#pragma once

#include <Python.h>
#include <string>

// Ce fichier d'en-tête à pour but de transposer à la quasi-identité les fonctions de Matplotlib et Numpy nécessaire pour le projet.
// L'objectif est simplement d'exposer des fonctions C/C++ pour exécuter des fonctions Python en arrière.

namespace plt {

/// @brief Initialise l'API Python-C
void openPython(int realTime);

/// @brief Ferme l'API Python-C
void closePython();

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
std::string loadtxt(int **array, uint rows, uint columns);

/// @brief Transfère un tableau 2D C++ dans l'interpréteur Python
/// @param array Tableau de valeurs à transférer
/// @param rows Nombre de lignes
/// @param columns Nombre de colonnes
/// @return Nom de la variable Python créée
void loadtxt(std::string varName, int **array, uint rows, uint columns);

/// @brief Transfère un tableau 1D C++ dans l'interpréteur Python
/// @param array Tableau de valeurs à transférer
/// @param length Nombre de lignes
/// @return Nom de la variable Python créée
std::string loadtxt(double *array, uint length);

/// @brief Transfère un tableau 1D C++ dans l'interpréteur Python
/// @param array Tableau de valeurs à transférer
/// @param length Nombre de lignes
/// @return Nom de la variable Python créée
void loadtxt(std::string varName, double *array, uint length);

/// @brief Reshape un tableau Numpy dans Python
/// @param varName Nom du tableau Numpy
/// @param rows Nombre de lignes
/// @param columns Nombre de colonnes
void reshape(std::string varName, uint rows, uint columns);

}