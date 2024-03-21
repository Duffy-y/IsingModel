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

void tryNeighbor(Ising::Lattice &lat, Parameters &options, std::stack<Site> &stack, std::map<Site, int> &cluster, int neighbor_x, int neighbor_y, int spin0) {
    Site neighbor = makeSite(lat, neighbor_x, neighbor_y);
    
    if (spin0 != Ising::getSpin(lat, neighbor_x, neighbor_y)) {
        // std::cout << "[Neighbor] Refusing neighbor (opposite spin): (" << neighbor_x << "," << neighbor_y << ")\n";
        return;
    }
    if (cluster.count(neighbor) != 0) {
        // std::cout << "[Neighbor] Refusing neighbor (in cluster): (" << neighbor_x << "," << neighbor_y << ")\n";
        return;
    }
    
    int isNeighborAccepted = (double)std::rand() / RAND_MAX < 1 - std::exp(- 2 / (options.kB * options.T));
    if (isNeighborAccepted) {
        // std::cout << "[Neighbor] Adding to stack site : (" << neighbor_x << "," << neighbor_y << ")\n";        
        stack.push(neighbor);
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
    std::map<Site, int> cluster;
    stack.push(makeSite(lat, randomX, randomY));

    // Suivi du cluster en temps réel
    int clusterSize = 0;
    int clusterNeighbor = 0;

    while(!stack.empty()) {
        visitedSite = stack.top();
        stack.pop();

        if (spin0 == Ising::getSpin(lat, visitedSite.first, visitedSite.second)) {
            // std::cout << "[Wolff] Visited site = " << visitedSite.first << ";" << visitedSite.second << "; spin =" << Ising::getSpin(lat, visitedSite.first, visitedSite.second) << "\n";
            
            cluster[visitedSite] = 1;
            clusterSize++;

            tryNeighbor(lat, options, stack, cluster, visitedSite.first + 1, visitedSite.second, spin0);
            tryNeighbor(lat, options, stack, cluster, visitedSite.first - 1, visitedSite.second, spin0);
            tryNeighbor(lat, options, stack, cluster, visitedSite.first, visitedSite.second + 1, spin0);
            tryNeighbor(lat, options, stack, cluster, visitedSite.first, visitedSite.second - 1, spin0);

            Ising::flipSpin(lat, visitedSite.first, visitedSite.second);
        }
    }

    // Calcul des voisins du cluster
    Site neighbor = makeSite(lat, 0, 0);
    for (const auto& [key, val] : cluster) {
        neighbor.first = Ising::pcx(lat, key.first + 1);
        neighbor.second = Ising::pcy(lat, key.second);
        if (cluster.count(neighbor) == 0) {
            clusterNeighbor += Ising::getSpin(lat, key.first + 1, key.second);
        }

        neighbor.first = Ising::pcx(lat, key.first - 1);
        neighbor.second = Ising::pcy(lat, key.second);
        if (cluster.count(neighbor) == 0) {
            clusterNeighbor += Ising::getSpin(lat, key.first - 1, key.second);
        }

        neighbor.first = Ising::pcx(lat, key.first);
        neighbor.second = Ising::pcy(lat, key.second + 1);
        if (cluster.count(neighbor) == 0) {
            clusterNeighbor += Ising::getSpin(lat, key.first, key.second + 1);
        }

        neighbor.first = Ising::pcx(lat, key.first);
        neighbor.second = Ising::pcy(lat, key.second - 1);
        if (cluster.count(neighbor) == 0) {
            clusterNeighbor += Ising::getSpin(lat, key.first, key.second - 1);
        }
    }

    // std::cout << "[Wolff] Cluster Neighbor = " << clusterNeighbor << "\n";
    // std::cout << "[Wolff] Cluster Spin = " << clusterSize * spin0 << "\n";
    // std::cout << "[Wolff] Cluster Size = " << clusterSize << "\n";

    deltaE = 2 * options.J * spin0 * clusterNeighbor;
    deltaM = - 2 * clusterSize * spin0;
}

int atEquilibrium(Ising::Lattice &lat, Parameters &options, int oldEnergy, int newEnergy) {
    return fabs((newEnergy - oldEnergy) / (double)newEnergy) < options.relativeVariation;
}


uint reachEquilibrium(Ising::Lattice &lat, Parameters &options, double &energy, double &magnetization) {
    // Propriétés du réseau.
    double oldEnergy = energy;
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
        // std::cout << "[MC] ΔE = " << deltaE << ", ΔM = " << deltaM << "\n";

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

    double energy = Ising::latticeEnergy(lat, options.J, options.h);
    double magnetization = Ising::magnetization(lat);
    double deltaE = 0;
    double deltaM = 0;

    for (uint i = 0; i < samplingPoints; i++)
    {
        std::cout << "[Thermalize] T = " << options.T << "\n";
        equilibriumSteps = reachEquilibrium(lat, options, energy, magnetization);
        meanSteps = options.dataRecordDuration * equilibriumSteps;
        
        props.E[i] = 0;
        props.E_sq[i] = 0;
        props.M[i] = 0;
        props.M_sq[i] = 0;
        props.M_abs[i] = 0;

        for (int j = 0; j < meanSteps; j++)
        {
            props.E[i] += energy;
            props.E_sq[i] += energy * energy;
            props.M[i] += magnetization;
            props.M_sq[i] += magnetization*magnetization;
            props.M_abs[i] += fabs(magnetization);
            
            options.mcIterator(lat, options, deltaE, deltaM);
            energy += deltaE;
            magnetization += deltaM;
        }

        props.T[i] = options.T;
        props.E[i] /= meanSteps; // On normalise l'énergie par spin.
        props.E_sq[i] /= meanSteps;
        props.M[i] /= meanSteps;
        props.M_sq[i] /= meanSteps;
        props.M_abs[i] /= meanSteps;
        
        options.T += dT;
    }
    return props;
}

}