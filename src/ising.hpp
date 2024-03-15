#pragma once

#include <stdlib.h>
#include <cassert>

#define UP 1
#define DOWN -1

namespace Ising {
/// @brief Définit un réseau de spin de taille fixe.
struct Lattice {
    int **spin;
    uint sizeX;
    uint sizeY;
};

/// @brief Alloue un réseau de spin 2D de taille donné.
/// @param sizeX Taille selon axe X
/// @param sizeY Taille selon axe Y
/// @return Réseau de spin alloué
Lattice lattice(const uint sizeX, const uint sizeY);

/// @brief Allou un réseau de spin 1D de taille donné.
/// @param sizeX Taille selon axe X
/// @return Réseau de spin alloué
Lattice lattice(const uint sizeX);

/// @brief Alloue plusieurs réseaux de spin 2D de taille donné. Pertinent pour moyennage des grandeurs physiques.
/// @param sizeX Taille selon axe X
/// @param sizeY Taille selon axe Y
/// @param latticeCount Nombre de réseaux de spin à allouer.
/// @return Tableau contenant les réseaux de spin alloués.
Lattice *latticeMulti(const uint sizeX, const uint sizeY, const uint latticeCount);

/// @brief Alloue plusieurs réseaux de spin 1D de taille donné. Pertinent pour moyennage des grandeurs physiques.
/// @param sizeX Taille selon axe X
/// @param latticeCount Nombre de réseaux de spin à allouer.
/// @return Tableau contenant les réseaux de spin alloués.
Lattice *latticeMulti(const uint sizeX, const uint latticeCount);

/// @brief Retourne l'adresse mémoire d'un site de spin de coordonnées (x, y) en tenant compte de la périodicité.
/// @param lat Réseau de spin à considérer
/// @param x Coordoonée x
/// @param y Coordonnée y
/// @return Adresse mémoire du spin en (x, y)
int *_getSpinRef(Lattice &lat, const int x, const int y);

/// @brief Retourne le spin d'un site de coordonnées (x, y) en tenant compte de la périodicité.
/// @param lat Réseau de spin à considérer
/// @param x Coordonnée x
/// @param y Coordonnée y
/// @return Valeur du spin en (x, y)
int getSpin(Lattice &lat, const int x, const int y);

/// @brief Assigne une valeur à un spin de coordonnées (x, y) en tenant compte de la périodicité.
/// @param lat Réseau de spin à considérer
/// @param x Coordonnée x
/// @param y Coordonnée y
/// @param spinValue Valeur à assigner au spin
void setSpin(Lattice &lat, const int x, const int y, const int spinValue);

/// @brief Retourne un spin de coordonnées (x, y) en tenant compte de la périodicité.
/// @param lat Réseau de spin à considérer
/// @param x Coordonnée x
/// @param y Coordonnée y
void flipSpin(Lattice &lat, const int x, const int y);

/// @brief Initialise l'ensemble du réseau avec des spins de même sens et norme
/// @param lat Le réseau de spin à initialiser
/// @param spinValueValeur du spin à attribuer sur l'ensemble du réseau
void uniformSpin(Lattice &lat, const int spinValue);

/// @brief Initialise l'ensemble du réseau sur une configuration aléatoire
/// @param lat Le réseau de spin à initialiser
/// @param p Probabilité qu'un spin soit UP
void randomSpin(Lattice &lat, const double p);

/// @brief Détermine l'hamiltonien du réseau entier
/// @param lat Réseau de spin
/// @param J Paramètre de couplage entre spin
/// @return Energie du réseau
int latticeEnergy(Lattice &lat, int J, int h);

/// @brief Calcule le gain d'énergie en cas de spin-flip
/// @param lat Réseau de spin
/// @param x Coordonnée x du spin à retourner 
/// @param y Coordonnée y du spin à retourner
/// @param J Paramètre de couplage entre spin
/// @return Gain d'énergie pour le réseau en cas d'énergie (E > 0 <=> Destabilisation, E < 0 <=> Stabilisation)
int swappingEnergy(Lattice &lat, const int x, const int y, int J, int h);

/// @brief Calcule le taux de magnétisation du milieu
/// @param lat Réseau de spin
/// @return Taux de magnétisation du milieu (en %)
double magnetization(Lattice &lat);

/// @brief Calcule le taux de magnétisation moyen sur un ensemble de réseau de spin
/// @param lat Tableau contenant des réseaux de spin
/// @param latticeCount Taille du tableau
/// @return Taux de magnétisation moyen
double meanMagnetization(Lattice *lat, uint latticeCount);


int pcx(Lattice &lat, int i);

int pcy(Lattice &lat, int i);

}