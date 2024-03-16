#include "pythoncpp.hpp"
#include "montecarlo.hpp"
#include <stack>
#include <cmath>
#include <iostream>

namespace MC {

Site site(Ising::Lattice &lat, int x, int y) {
    Site out = Site();
    out.x = Ising::pcx(lat, x);
    out.y = Ising::pcy(lat, y);
    return out;
}

void metropolisIteration(Ising::Lattice &lat, Parameters &options) {
    int randomX = std::rand() % lat.sizeX;
    int randomY = std::rand() % lat.sizeY;

    const double deltaE = Ising::swappingEnergy(lat, randomX, randomY, options.J, options.h);
    const double deltaM = Ising::swappingMagnetization(lat, randomX, randomY);
    
    if (deltaE <= 0 || (double)std::rand() / RAND_MAX < std::exp(-deltaE/(options.kB * options.T))) {
        Ising::flipSpin(lat, randomX, randomY);
    }
}

void wolffIteration(Ising::Lattice &lat, Parameters &options) {
    int randomX = std::rand() % lat.sizeX;
    int randomY = std::rand() % lat.sizeY;

    std::stack<Site> stack;
    stack.push(site(lat, randomX, randomY));

    Site visitedSite;
    int spin0 = Ising::getSpin(lat, randomX, randomY);

    while(!stack.empty()) {
        visitedSite = stack.top();
        stack.pop();
        
        if (spin0 == Ising::getSpin(lat, visitedSite.x + 1, visitedSite.y)) {
            int isNeighborAccepted = (double)std::rand() / RAND_MAX < 1 - std::exp(- 2 / (options.kB * options.T));
            if (isNeighborAccepted) {
                stack.push(site(lat, visitedSite.x + 1, visitedSite.y));
            }
        }
        if (spin0 == Ising::getSpin(lat, visitedSite.x - 1, visitedSite.y)) {
            int isNeighborAccepted = (double)std::rand() / RAND_MAX < 1 - std::exp(- 2 / (options.kB * options.T));
            if (isNeighborAccepted) {
                stack.push(site(lat, visitedSite.x - 1, visitedSite.y));
            }
        }
        if (spin0 == Ising::getSpin(lat, visitedSite.x, visitedSite.y + 1)) {
            int isNeighborAccepted = (double)std::rand() / RAND_MAX < 1 - std::exp(- 2 / (options.kB * options.T));
            if (isNeighborAccepted) {
                stack.push(site(lat, visitedSite.x, visitedSite.y + 1));
            }
        }
        if (spin0 == Ising::getSpin(lat, visitedSite.x, visitedSite.y - 1)) {
            int isNeighborAccepted = (double)std::rand() / RAND_MAX < 1 - std::exp(- 2 / (options.kB * options.T));
            if (isNeighborAccepted) {
                stack.push(site(lat, visitedSite.x, visitedSite.y - 1));
            }
        }
        Ising::flipSpin(lat, visitedSite.x, visitedSite.y);
    }
}

int atEquilibrium(Ising::Lattice &lat, Parameters &options, int oldEnergy, int newEnergy) {
    return fabs((newEnergy - oldEnergy) / (double)newEnergy) < options.relativeVariation;
}


int reachEquilibrium(Ising::Lattice &lat, Parameters &options) {
    int oldEnergy = Ising::latticeEnergy(lat, options.J, options.h);
    int newEnergy;
    int isEquilibrium = 0;
    int confirmEquilibrium = 0;

    uint i = 0;
    while (i < options.epochThreshold && !isEquilibrium) {
        if (i % options.jumpSize == 0) {
            newEnergy = Ising::latticeEnergy(lat, options.J, options.h);
            isEquilibrium = atEquilibrium(lat, options, oldEnergy, newEnergy);
            oldEnergy = newEnergy;

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
        options.mcIterator(lat, options);

        i++;
    }
    return i;
}

Properties thermalizeLattice(Ising::Lattice &lat, Parameters &options, double Ti, double Tf, uint samplingPoints) {
    assert(samplingPoints > 1);

    Properties props = Properties(); 
    props.E = new double[samplingPoints];
    props.E_sq = new double[samplingPoints];
    props.M = new double[samplingPoints];
    props.M_sq = new double[samplingPoints];
    props.M_abs = new double[samplingPoints];
    props.T = new double[samplingPoints];
    
    options.T = std::min(Ti, Tf);
    double dT = fabs(Tf - Ti) / (samplingPoints - 1);

    int equilibriumSteps;
    int meanSteps;
    double E;
    double m;

    for (uint i = 0; i < samplingPoints; i++)
    {
        std::cout << "[Thermalize] T = " << options.T << "\n";
        equilibriumSteps = reachEquilibrium(lat, options);
        meanSteps = options.dataRecordDuration * equilibriumSteps;
        
        for (int j = 0; j < meanSteps; j++)
        {
            E = Ising::latticeEnergy(lat, options.J, options.h);
            m = Ising::magnetization(lat);
            props.E[i] += E / lat.sizeXY;
            props.E_sq[i] += E * E / (lat.sizeXY * lat.sizeXY);
            props.M[i] += m;
            props.M_sq[i] += m*m;
            props.M_abs[i] += fabs(m);
            options.mcIterator(lat, options);
        }

        props.T[i] = options.T;
        props.E[i] /= meanSteps;
        props.E_sq[i] /= meanSteps;
        props.M[i] /= meanSteps;
        props.M_sq[i] /= meanSteps;
        props.M_abs[i] /= meanSteps;
        
        options.T += dT;
    }
    return props;
}

}