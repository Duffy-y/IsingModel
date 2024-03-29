#include <Python.h>
#include "pythoncpp.hpp"
#include "colormap.hpp"
#include "ising.hpp"
#include "montecarlo.hpp"
#include <ctime>
#include <fstream>
#include <iostream>
#include <unistd.h>  

void saveProps(Ising::Lattice &lat, MC::Parameters options, MC::Properties props, uint samplingPoints, std::string fileName) {
    std::fstream file;
    file.open("res/" + fileName, std::ios::out);

    double norm, nr2;

    for (uint i = 0; i < samplingPoints; i++)
    {
        norm = 1/(lat.sizeXY * lat.neighborCount * props.mcSteps[i]);
        nr2 = norm / props.mcSteps[i];
        file << props.T[i] << ";";
        file << norm * props.E[i]<< ";";
        file << norm * props.E_sq[i]<< ";";
        file << norm * props.M[i] << ";"; 
        file << norm * props.M_sq[i] << ";"; 
        file << norm * props.M_abs[i] << ";";
        file << (norm * props.E_sq[i] - nr2 * props.E[i] * props.E[i]) / (options.kB * props.T[i]) / (options.kB * props.T[i]) << ";";
        file << (norm * props.M_sq[i] - nr2 * props.M[i] * props.M[i]) / (options.kB * props.T[i]) << "\n";
    }
    file.close();
}

/// @brief Fonction pour montrer visuellement l'évolution du système.
void showAlgorithm(Ising::Lattice &lat, MC::Parameters options, double Ti, double Tf, uint samplingPoints) {
    plt::ion();
    double deltaT = abs(Tf - Ti) / samplingPoints;

    auto spin = np::array(lat.spin, lat.sizeY, lat.sizeX);
    
    double E = Ising::latticeEnergy(lat, options.J, options.h);
    double M = Ising::magnetization(lat);
    double deltaE = 0;
    double deltaM = 0;
    
    for (size_t i = 0; i < samplingPoints; i++)
    {
        for (size_t j = 0; j < options.epochThreshold; j++)
        {
            if (j % options.jumpSize == 0) {
                plt::clf();
                plt::xlabel("X");
                plt::ylabel("Y");
                plt::title("T=" + std::to_string(options.T) + "; X=" + std::to_string(M));
                np::array(spin, lat.spin, lat.sizeY, lat.sizeX);
                plt::imshow(spin, CMAP_seismic);
                plt::colorbar(); 
                plt::pause();
            }
            options.mcIterator(lat, options, deltaE, deltaM);
            E += deltaE;
            M += deltaM;
        }

        options.T += deltaT;
    }
    plt::ioff();
}


int main(int argc, char *argv[]) {
    std::srand(std::time(NULL));
    py::openPython();    

    // * Initialisation du réseau de spin
    Ising::Lattice lat = Ising::lattice(16, 16);
    Ising::randomSpin(lat, 0.5);

    // * Création des paramètres de simulation
    MC::Parameters options = MC::parameters(2.5e6, 150000, 0.5, 0.0002, MC::metropolisIteration, 0.1, 1, 0, 1);
    // MC::Parameters options = MC::parameters(500, 100, 2, 0.0002, MC::wolffIteration, 0.01, 1, 0, 1);

    // * Démonstration de l'algorithme visuellement
    // showAlgorithm(lat, options, 0.1, 5, 20);

    // * Génération des données pour T qui varie
    uint samplingPoints = 100;
    MC::Properties propsTemp = MC::thermalizeLattice(lat, options, 0.1, 5, samplingPoints);
    saveProps(lat, options, propsTemp, samplingPoints, "temp_data.csv");

    // * Génération des données pour h qui varie
    options.T = 4;
    MC::Properties propsMagn = MC::magnetizeLattice(lat, options, -5, 5, samplingPoints);
    saveProps(lat, options, propsMagn, samplingPoints, "magn_data.csv");

    py::closePython();

    // ! Les free sont inutiles dans ce programme, les tableaux sont nécessaires et persistent tout le long
    // ! de la durée de vie du programme. Les tableaux seront libérés automatiquement par le noyau.

    free(lat.spin);

    free(propsTemp.E);
    free(propsTemp.E_sq);
    free(propsTemp.M);
    free(propsTemp.M_sq);
    free(propsTemp.mcSteps);
    free(propsTemp.T);

    free(propsMagn.E);
    free(propsMagn.E_sq);
    free(propsMagn.M);
    free(propsMagn.M_sq);
    free(propsMagn.mcSteps);
    free(propsMagn.T);

    return 0;
}   