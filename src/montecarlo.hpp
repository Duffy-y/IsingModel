#pragma once

#include "ising.hpp"

namespace MC {

struct Parameters
{
    uint epochThreshold;
    uint jumpSize; 
    uint dataRecordDuration;
    double relativeVariation;

    void (*mcIterator)(Ising::Lattice&, Parameters&);

    int J;
    double T;
    double kB;
};

struct Properties {
    double *T;
    double *E;
    double *E_sq;
    double *M;
    double *M_sq;
    double *M_abs;
};

struct Site {
    int x;
    int y;
};

Site site(Ising::Lattice &lat, int x, int y);

/// @brief Effectue une seule itération de l'algorithme de Metropolis sur le réseau
/// @param lat Réseau de spin
/// @param T Température du réseau
void metropolisIteration(Ising::Lattice &lat, Parameters &options);

/// @brief Effectue une seule itération de l'algorithme de Wolff sur le réseau.
/// Cet algorithme ne nécessite pas de l'itérer un grand nombre de fois, pour un réseau
/// de taille 256x256, une vingtaine d'itérations suffit à atteindre un équilibre.
/// Stack = pas de fonction récursive sujette à exploser le call stack (8Mb sur Fedora par défaut).
/// Un stack offre autant d'appels que de RAM disponible.
/// @param lat Réseau de spin
/// @param T Température du réseau
void wolffIteration(Ising::Lattice &lat, Parameters &options);

/// @brief Vérifie si la variation relative d'énergie est sous un seuil donné qu'on considère comme équilibre.
/// @param lat Réseau de spin
/// @param options Paramètres de simulation
/// @param oldEnergy Ancienne énergie
/// @param newEnergy Nouvelle énergie
/// @return 
int atEquilibrium(Ising::Lattice &lat, Parameters &options, int oldEnergy, int newEnergy);

/// @brief Itère un des deux algorithmes Monte-Carlo pour emmener le réseau à l'équilibre dans les conditions données.
/// @param lat Réseau de spin
/// @param options Paramètres de simulation
/// @return Nombre d'itérations nécessaire pour atteindre l'équilibre.
int reachEquilibrium(Ising::Lattice &lat, Parameters &options);

/// @brief Fait varier la température progressivement pour obtenir l'évolution des grandeurs moyennes selon la température
/// @param lat Réseau de spin
/// @param options Paramètres de simulation
/// @param Ti Température de départ
/// @param Tf Température de fin
/// @param samplingPoints Nombre de points à calculer
Properties thermalizeLattice(Ising::Lattice &lat, Parameters &options, double Ti, double Tf, uint samplingPoints);
}