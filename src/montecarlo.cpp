#include "montecarlo.hpp"

namespace MC {
Parameters parameters(uint epochTreshold, uint jumpSize, double dataRecordDuration, double relativeVariation, void (*mcIterator)(Ising::Lattice&, Parameters&, double&, double&), double T, double J, double h, double kB) {
    Parameters options = Parameters();
    options.epochThreshold = epochTreshold;
    options.J = J;
    options.h = h;
    options.jumpSize = jumpSize;
    options.T = T;     
    options.kB = kB;
    options.relativeVariation = relativeVariation;
    options.dataRecordDuration = dataRecordDuration;
    options.mcIterator = mcIterator;

    return options;
}

Site makeSite(Ising::Lattice &lat, int x, int y) {
    Site out = Site();
    out.first = Ising::pcx(lat, x);
    out.second = Ising::pcy(lat, y);
    return out;
}

void metropolisIteration(Ising::Lattice &lat, Parameters &options, double &deltaE, double &deltaM) {
    int randomX = std::rand() % lat.sizeX;
    int randomY = std::rand() % lat.sizeY;

    deltaE = Ising::swappingEnergy(lat, randomX, randomY, options.J, options.h);
    deltaM = Ising::swappingMagnetization(lat, randomX, randomY);
    
    if (deltaE <= 0 || (double)std::rand() / RAND_MAX < std::exp(-deltaE/(options.kB * options.T))) {
        Ising::flipSpin(lat, randomX, randomY);
    }
    else {
        deltaE = 0;
        deltaM = 0;
    }
}

void tryNeighbor(Ising::Lattice &lat, Parameters &options, std::stack<Site> &stack, std::map<Site, int> &rejectedSite, int neighbor_x, int neighbor_y, int spin0, int &clusterSize, int &clusterSpin, int &clusterNeighbor) {
    Site neighbor = makeSite(lat, neighbor_x, neighbor_y);
    
    if (spin0 != Ising::getSpin(lat, neighbor_x, neighbor_y)) {
        rejectedSite[neighbor] = -spin0;
        return;
    }
    
    int isNeighborAccepted = (double)std::rand() / RAND_MAX < 1 - std::exp(- 2 / (options.kB * options.T));
    
    if (isNeighborAccepted) {
        rejectedSite.erase(neighbor);
        stack.push(neighbor);

        clusterSize += 1;
        clusterSpin += spin0;
    }
    else {
        rejectedSite[neighbor] = spin0;
    }
}

void wolffIteration(Ising::Lattice &lat, Parameters &options, double &deltaE, double &deltaM) {
    assert(options.h == 0);

    // Spin initial
    int randomX = std::rand() % lat.sizeX;
    int randomY = std::rand() % lat.sizeY;
    int spin0 = Ising::getSpin(lat, randomX, randomY);

    // Suivi de la construction du cluster
    Site visitedSite;
    std::stack<Site> stack;
    stack.push(makeSite(lat, randomX, randomY));

    // Suivi du cluster en temps réel (std::map offre une complexité O(log(n)) pour les opérations d'accès, suppression et permet de ne pas compter double)
    std::map<Site, int> rejectedSite;
    int clusterSize = 1;
    int clusterSpin = spin0;
    int clusterNeighbor = 0;

    while(!stack.empty()) {
        visitedSite = stack.top();
        stack.pop();

        tryNeighbor(lat, options, stack, rejectedSite, visitedSite.first + 1, visitedSite.second, spin0, clusterSize, clusterSpin, clusterNeighbor);
        tryNeighbor(lat, options, stack, rejectedSite, visitedSite.first - 1, visitedSite.second, spin0, clusterSize, clusterSpin, clusterNeighbor);
        tryNeighbor(lat, options, stack, rejectedSite, visitedSite.first, visitedSite.second + 1, spin0, clusterSize, clusterSpin, clusterNeighbor);
        tryNeighbor(lat, options, stack, rejectedSite, visitedSite.first, visitedSite.second - 1, spin0, clusterSize, clusterSpin, clusterNeighbor);

        Ising::flipSpin(lat, visitedSite.first, visitedSite.second);
    }

    for (auto it = rejectedSite.begin(); it != rejectedSite.end(); it++) {
        clusterNeighbor += it->second;
    }
    deltaE = 2 * options.J * spin0 * clusterNeighbor + 2 * options.h * clusterSpin;
    deltaM = - 2 * clusterSpin / lat.sizeXY;
}

int atEquilibrium(Ising::Lattice &lat, Parameters &options, int oldEnergy, int newEnergy) {
    return fabs((newEnergy - oldEnergy) / (double)newEnergy) < options.relativeVariation;
}


uint reachEquilibrium(Ising::Lattice &lat, Parameters &options) {
    // Propriétés du réseau.
    double energy = Ising::latticeEnergy(lat, options.J, options.h);
    double oldEnergy = energy;
    double magnetization = Ising::magnetization(lat);
    double deltaE = 0;
    double deltaM = 0;
    
    // Suivi de l'équilibre
    int isEquilibrium = 0;
    int confirmEquilibrium = 0;

    uint i = 0;
    while (i < options.epochThreshold && !isEquilibrium) {
        if (i % options.jumpSize == 0) {
            isEquilibrium = atEquilibrium(lat, options, oldEnergy, energy);
            oldEnergy = energy;

            if (isEquilibrium && !confirmEquilibrium) {
                confirmEquilibrium = 1;
                isEquilibrium = 0;
            }
            else if (isEquilibrium && confirmEquilibrium) {
                std::cout << "[MC] Equilibrium state found.\n";
                break;
            }
            else if (!isEquilibrium && confirmEquilibrium) {
                confirmEquilibrium = 0;
            }
        }
        options.mcIterator(lat, options, deltaE, deltaM);
        energy += deltaE;
        magnetization += deltaM;

        i++;
    }
    return i;
}

// Properties thermalizeLattice(Ising::Lattice &lat, Parameters &options, double Ti, double Tf, uint samplingPoints) {
//     assert(samplingPoints > 1);

//     Properties props = Properties(); 
//     props.E = new double[samplingPoints];
//     props.E_sq = new double[samplingPoints];
//     props.M = new double[samplingPoints];
//     props.M_sq = new double[samplingPoints];
//     props.M_abs = new double[samplingPoints];
//     props.T = new double[samplingPoints];
    
//     options.T = std::min(Ti, Tf);
//     double dT = fabs(Tf - Ti) / (samplingPoints - 1);

//     int equilibriumSteps;
//     int meanSteps;
//     double E;
//     double m;

//     for (uint i = 0; i < samplingPoints; i++)
//     {
//         std::cout << "[Thermalize] T = " << options.T << "\n";
//         equilibriumSteps = reachEquilibrium(lat, options);
//         meanSteps = options.dataRecordDuration * equilibriumSteps;
        
//         for (int j = 0; j < meanSteps; j++)
//         {
//             E = Ising::latticeEnergy(lat, options.J, options.h);
//             m = Ising::magnetization(lat);
//             props.E[i] += E / lat.sizeXY;
//             props.E_sq[i] += E * E / (lat.sizeXY * lat.sizeXY);
//             props.M[i] += m;
//             props.M_sq[i] += m*m;
//             props.M_abs[i] += fabs(m);
//             options.mcIterator(lat, options);
//         }

//         props.T[i] = options.T;
//         props.E[i] /= meanSteps;
//         props.E_sq[i] /= meanSteps;
//         props.M[i] /= meanSteps;
//         props.M_sq[i] /= meanSteps;
//         props.M_abs[i] /= meanSteps;
        
//         options.T += dT;
//     }
//     return props;
// }

}