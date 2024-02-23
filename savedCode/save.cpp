#include <iostream>
#include <ctime>
#include <cmath>
#include <fstream>
#include <stack>

// Matplotlib parameters
#define MATPLOTLIB_ENABLED 1
#define INTERACTIVE 1

// Simulation conditions
// J doit être un entier, sinon on introduit une instabilité de type dans les fonctions.
#define UP 1
#define DOWN (-1)
#define J 1
#define kB 1.0

// Simulation parameters
#define L 100
#define TEMP_STEP 0.1
#define EPOCH_TRESHOLD 1000000
#define JUMP_SIZE L*L
#define RELATIVE_VARIATION 0.02

#if MATPLOTLIB_ENABLED
#include <Python.h>
#endif

#if TIME_EXEC
#include <chrono>
#endif

template <typename T>
T min(T a, T b) {
    return a > b ? a : b;
}

struct Lattice {
    int **spin;
    size_t sizeX;
    size_t sizeY;
    int energy;
};

Lattice lattice(size_t sizeX, size_t sizeY) {
    Lattice lat = Lattice();
    lat.spin = (int**)malloc(sizeof(int*) * sizeY);
        
    for (int i = 0; i < sizeY; i++) {
        lat.spin[i] = (int*)malloc(sizeof(int) * sizeX);
    }

    lat.sizeX = sizeX;
    lat.sizeY = sizeY;

    return lat;
}

Lattice lattice(const size_t sizeX) {
    return lattice(sizeX, 1);
}

Lattice* latticeMulti(size_t sizeX, size_t sizeY, size_t latticeCount) {
    Lattice *lattices = (Lattice*)malloc(sizeof(Lattice) * latticeCount);
    for (size_t i = 0; i < latticeCount; i++)
    {
        lattices[i] = lattice(sizeX, sizeY);
    }
    return lattices;
}

Lattice* latticeMulti(size_t sizeX, size_t latticeCount) {
    Lattice *lattices = (Lattice*)malloc(sizeof(Lattice) * latticeCount);
    for (size_t i = 0; i < latticeCount; i++)
    {
        lattices[i] = lattice(sizeX, 1);
    }
    return lattices;
}

int* _getSpinRef(Lattice &lat, const size_t x, const size_t y) {
    return &lat.spin[(y + lat.sizeY) % lat.sizeY][(x + lat.sizeX) % lat.sizeX];
}

int getSpin(Lattice &lat, const size_t x, const size_t y) {
    return *_getSpinRef(lat, x, y);
}

void setSpin(Lattice &lat, const size_t x, const size_t y, const int spinValue) {
    *_getSpinRef(lat, x, y) = spinValue;
}

void flipSpin(Lattice &lat, const size_t x, const size_t y) {
    *_getSpinRef(lat, x, y) *= -1;
}

/**
 * @brief Ecrit le contenu d'un réseau de spin dans un fichier (et affiche sous Matplotlib si disponible)
 * 
 * @param lat Réseau de spin à écrire (et afficher)
 */
void plotLattice(Lattice &lat) {
    std::fstream stream;
    stream.open("lattice.grid", std::ios::out);
    for (size_t y = 0; y < lat.sizeY; y++)
    {
        for (size_t x = 0; x < lat.sizeX; x++)
        {
            stream << getSpin(lat, x, y) << " ";
        }
        stream << "\n";
    }
    stream.close();

    #if MATPLOTLIB_ENABLED
    PyRun_SimpleString("A = np.loadtxt('lattice.grid')");
    PyRun_SimpleString("plt.clf()");
    PyRun_SimpleString("plt.imshow(A)");
    PyRun_SimpleString("plt.show()");

    if (!INTERACTIVE) return;
    PyRun_SimpleString("plt.pause(0.0001)");
    #endif
}

/**
 * @brief Initialise l'ensemble du réseau avec des spins de même sens et norme
 * 
 * @param lat Le réseau de spin à initialiser
 * @param spinValue Valeur du spin à attribuer sur l'ensemble du réseau
 */
void uniformSpin(Lattice &lat, const int spinValue) {
    for (size_t y = 0; y < lat.sizeY; y++) {
        for (size_t x = 0; x < lat.sizeX; x++) {
            setSpin(lat, x, y, spinValue);
        }
    }
}

/// @brief Initialise l'ensemble du réseau sur une configuration aléatoire
/// @param lat Le réseau de spin à initialiser
/// @param p Probabilité qu'un spin soit UP
void randomSpin(Lattice &lat, const double p) {
    assert(p >= 0);
    int spinValue = 0;
    for (int y = 0; y < lat.sizeY; y++) {
        for (int x = 0; x < lat.sizeX; x++) {
            spinValue = (double)std::rand() / RAND_MAX < p ? UP : DOWN;
            setSpin(lat, x, y, spinValue);
        }
    }
}

/// @brief Détermine l'hamiltonien du réseau entier
/// @param lat Réseau de spin
/// @return Energie du réseau
int latticeEnergy(Lattice &lat) {
    int latEnergy = 0;
    for (int y = 0; y < lat.sizeY; y++) {
        for (int x = 0; x < lat.sizeX; x++) {
            latEnergy -= J * getSpin(lat, x, y) * (getSpin(lat, x + 1, y) + getSpin(lat, x, y + 1));
        }
    }
    return latEnergy;
}

/// @brief Calcule le gain d'énergie en cas de spin-flip
/// @param lat Réseau de spin
/// @param x Coordonnée x du spin à retourner 
/// @param y Coordonnée y du spin à retourner
/// @return Gain d'énergie pour le réseau en cas d'énergie (E > 0 <=> Destabilisation, E < 0 <=> Stabilisation)
int swappingEnergy(Lattice &lat, const size_t x, const size_t y) {
    const int neighbor_count = getSpin(lat, x + 1, y) + getSpin(lat, x, y + 1) + getSpin(lat, x - 1, y) + getSpin(lat, x, y - 1);
    return 2 * J * getSpin(lat, x, y) * neighbor_count;
}

/// @brief Effectue une seule itération de l'algorithme de Metropolis sur le réseau
/// @param lat Réseau de spin
/// @param T Température du réseau
void metropolisIteration(Lattice &lat, const double T) {
    size_t randomX = std::rand() % lat.sizeX;
    size_t randomY = std::rand() % lat.sizeY;

    const int deltaE = swappingEnergy(lat, randomX, randomY);
    if (deltaE <= 0 || (double)std::rand() / RAND_MAX < std::exp(-deltaE/(kB * T))) {
        flipSpin(lat, randomX, randomY);
        lat.energy += deltaE;
    }
}

// /// @brief Calcule le taux de magnétisation du milieu
// /// @param lat Réseau de spin
// /// @return Taux de magnétisation du milieu (en %)
// double magnetization(Lattice &lat) {
//     double mag = 0;
//     for (int y = 0; y < lat.sizeY; y++) {
//         for (int x = 0; x < lat.sizeY; x++) {
//             mag += getSpin(lat, x, y);
//         }
//     }
//     return mag / (lat.sizeY * lat.sizeX);
// }

// /// @brief Calcule le taux de magnétisation moyen sur un ensemble de réseau de spin
// /// @param lat Tableau contenant des réseaux de spin
// /// @param latticeCount Taille du tableau
// /// @return Taux de magnétisation moyen
// double meanMagnetization(Lattice *lat, size_t latticeCount) {
//     double mag = 0;
//     for (size_t i = 0; i < latticeCount; i++)
//     {
//         mag += magnetization(lat[i]);
//     }
//     return mag / latticeCount;
// }

// double calorificCoefficient(Lattice &lat) {
//     return 0;
// }

// int atEquilibrium(int energy, int oldEnergy) {
//     // On sait que l'équilibre se trouve à une énergie de -2L², on ajoute donc une condition cohérente avec cette observation pour favoriser Metropolis
//     // à converger vers cet équilibre si possible (L'algorithme de Wolff permettrait d'aller bien plus vite pour ça)
//     return energy < - 2 * JUMP_SIZE && (abs((energy - oldEnergy) / (double)energy)) < RELATIVE_VARIATION ? 1 : 0;
// }

// /// @brief Itere l'algorithme de Metropolis jusqu'à l'atteinte de deux équilibres successifs. La fonction suppose que lat.energy est initialisé (résultat incohérent dans le cas contraire)
// /// @param lat Réseau de spin à amener à l'équilibre (si possible)
// /// @param T Température du réseau de spin
// void reachEquilibrium(Lattice &lat, double T) {
//     size_t i = 0;

//     int oldEnergy = lat.energy;

//     int hasReachEquilibrium = 0;
//     int isEquilbrium = 0;
//     int confirmEquilibrium = 0;

//     while(!hasReachEquilibrium && i < EPOCH_TRESHOLD) {
//         metropolisIteration(lat, T);

//         if (i % JUMP_SIZE == 0 && !isEquilbrium) {
//             isEquilbrium = atEquilibrium(lat.energy, oldEnergy);
//             oldEnergy = lat.energy;

//             if (isEquilbrium && confirmEquilibrium) { // Deuxième équilibre consécutifs.
//                 hasReachEquilibrium = 1;
//             }
//             else if (isEquilbrium) { // Première occurence d'un équilibre
//                 confirmEquilibrium = 1;
//                 isEquilbrium = 0;
//             }
//             else if (!isEquilbrium && confirmEquilibrium) { // Un équilibre puis pas un équilibre
//                 confirmEquilibrium = 0;
//             }
//         }

//         if (i % JUMP_SIZE == 0 && MATPLOTLIB_ENABLED && INTERACTIVE) {
//             plotLattice(lat);
//         }

//         i++;
//     }
//     return;
// }

// void thermalizeLattice(double Ti, double Tf, size_t samplingPoints, size_t latticeCount) {
//     assert(samplingPoints > 1);

//     double *meanMagnetization = (double*)malloc(sizeof(double) * samplingPoints);
//     double *calorificCoefficient = (double*)malloc(sizeof(double) * samplingPoints);
//     Lattice* multiverse = (Lattice*)malloc(sizeof(Lattice) * latticeCount);

//     double dT = abs(Tf - Ti) / (samplingPoints - 1);
//     double T = min<double>(Ti, Tf);

//     for (size_t i = 0; i < samplingPoints; i++)
//     {
//         for (size_t latIndex = 0; latIndex < latticeCount; latIndex++)
//         {
//             reachEquilibrium(multiverse[latIndex], T);
//         }
//         // meanMagnetization[i] = meanMagnetization(multiverse, latticeCount);
//         // Fonction pour coefficient calorifique
//         // Fonction pour énergie du réseau.
//         T += dT;
//     }
    
//     return;
// }

/// @brief Initialise l'API Python-C
void initializePython() {
    #if MATPLOTLIB_ENABLED

    Py_Initialize();
    PyRun_SimpleString("import numpy as np");
    PyRun_SimpleString("import matplotlib.pyplot as plt");

    if (INTERACTIVE)
        PyRun_SimpleString("plt.ion()");

    PyRun_SimpleString("fig=plt.figure()");

    #endif
    return;
}

// void wolffIteration(Lattice &lat, const double T) {
//     size_t randomX = std::rand() % lat.sizeX;
//     size_t randomY = std::rand() % lat.sizeY;

//     std::stack<Site> stack;
//     stack.push(site(randomX, randomY));

//     Site visitedSite;
//     int visitedSpin;

//     while(!stack.empty()) {
//         visitedSite = stack.top();
//         std::cout << "Visiting site : " << visitedSite.x << ";" << visitedSite.y << "\n";
//         stack.pop();
//         visitedSpin = getSpin(lat, visitedSite.x, visitedSite.y);
        
//         if (visitedSpin == getSpin(lat, visitedSite.x + 1, visitedSite.y)) {
//             int isNeighborAccepted = (double)std::rand() / RAND_MAX < 1 - std::exp(- 2 / (kB * T));
//             if (isNeighborAccepted) {
//                 stack.push(site(visitedSite.x + 1, visitedSite.y));
//                 // ! ERREUR ICI: On ajoute visitedSite.x + 1, qui ne tient pas compte de la périodicité du réseau -> dérive 
//                 // ! de la position vers l'infini.
//             }
//         }
//         flipSpin(lat, visitedSite.x, visitedSite.y);
//     }
// }

int main() {    
    std::srand(std::time(nullptr));
    initializePython();

    Lattice lat = lattice(L, L);
    randomSpin(lat, 0.5);

    for (size_t i = 0; i < 100000000; i++)
    {
        if (i % 100000 == 0) {
            plotLattice(lat);
        }
        metropolisIteration(lat, 2.6);
    }
    


    #if MATPLOTLIB_ENABLED
    Py_Finalize();
    #endif

    return 0;
}