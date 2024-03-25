#pragma once

#include "ising.hpp"
#include <stack>
#include <map>
#include <cmath>
#include <iostream>

namespace MC {

struct Parameters
{
    uint epochThreshold;
    uint jumpSize; 
    double dataRecordDuration;
    double relativeVariation;

    void (*mcIterator)(Ising::Lattice&, Parameters&, double&, double&);

    double J;
    double h;
    double T;
    double kB;
};

Parameters parameters(uint epochTreshold, uint jumpSize, double dataRecordDuration, double relativeVariation, void (*mcIterator)(Ising::Lattice&, Parameters&, double&, double&), double T, double J, double h, double kB);

struct Properties {
    double *T;
    double *E;
    double *E_sq;
    double *M;
    double *M_sq;
    double *M_abs;
};

typedef std::pair<int, int> Site;

Site makeSite(Ising::Lattice &lat, int x, int y);

/// @brief Effectue une seule itération de l'algorithme de Metropolis sur le réseau
/// @param lat Réseau de spin
/// @param options Paramètres de simulation
/// @param deltaE Variable où stocker la différence d'énergie du mouvement Monte-Carlo
/// @param deltaM Variable où stocker la différence de magnetisation du mouvement Monte-Carlo
void metropolisIteration(Ising::Lattice &lat, Parameters &options, double &deltaE, double &deltaM);

/// @brief 
/// @param lat 
/// @param options 
/// @param stack 
/// @param rejectedSite 
/// @param neighbor_x 
/// @param neighbor_y 
/// @param spin0 
/// @param clusterSize 
/// @param clusterSpin 
/// @param clusterNeighbor 
void tryNeighbor(Ising::Lattice &lat, Parameters &options, std::stack<Site> &stack, std::map<Site, int> &cluster, int neighbor_x, int neighbor_y, int spin0);

/// @brief Effectue une seule itération de l'algorithme de Wolff sur le réseau.
/// Cet algorithme ne nécessite pas de l'itérer un grand nombre de fois, pour un réseau
/// de taille 256x256, une vingtaine d'itérations suffit à atteindre un équilibre.
/// Stack = pas de fonction récursive sujette à exploser le call stack (8Mb sur Fedora par défaut).
/// Un stack offre autant d'appels que de RAM disponible.
// ! Cet algorithme n'est pas pertinent pour h != 0, metropolis convergera très rapidement vers l'état d'équilibre de toute manière
// ! Tenir compte de h implique de rajouter une seconde hashmap pour suivre les noeuds du cluster sans les retourner, le retournement du cluster
// ! étant conditionné par ΔS = 2h * spin_cluster, une fois le cluster construit ... Complexifie inutilement l'algorithme quand Metropolis fonctionne pour ce cas.
/// @param lat Réseau de spin
/// @param options Paramètres de simulation 
void wolffIteration(Ising::Lattice &lat, Parameters &options, double &deltaE, double &deltaM);

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
/// @param energy Energie du réseau (doit être initialisée à la valeur actuelle préalablement)
/// @param magnetization Magnetisation du réseau (doit être initialisée à la valeur actuelle préalablement)
/// @return Nombre d'itérations nécessaire pour atteindre l'équilibre.
uint reachEquilibrium(Ising::Lattice &lat, Parameters &options, double &energy, double &magnetization);

/// @brief Fait varier la température progressivement pour obtenir l'évolution des grandeurs moyennes selon la température
/// @param lat Réseau de spin
/// @param options Paramètres de simulation
/// @param Ti Température de départ
/// @param Tf Température de fin
/// @param samplingPoints Nombre de points à calculer
Properties thermalizeLattice(Ising::Lattice &lat, Parameters &options, double Ti, double Tf, uint samplingPoints);

Properties magnetizeLattice(Ising::Lattice &lat, Parameters &options, double hi, double hf, uint samplingPoints);
}