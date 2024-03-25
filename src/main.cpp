#include <Python.h>
#include "pythoncpp.hpp"
#include "colormap.hpp"
#include "ising.hpp"
#include "montecarlo.hpp"
#include <ctime>
#include <fstream>
#include <iostream>
#include <unistd.h>  

double* calorific_coefficient(MC::Properties props, uint samplingPoints, double kB) {
    double *cv = new double[samplingPoints];

    for (uint i = 0; i < samplingPoints; i++)
    {
        cv[i] = props.E_sq[i] - props.E[i]*props.E[i];
        cv[i] /= (kB * props.T[i])*(kB * props.T[i]);
    }
    return cv;
}

double* susceptibility(MC::Properties props, uint samplingPoints, double kB) {
    double *X = new double[samplingPoints];

    for (uint i = 0; i < samplingPoints; i++)
    {
        X[i] = props.M_sq[i] - props.M[i]*props.M[i];
        X[i] /= (kB * props.T[i]);
    }
    return X;
}

/// @brief Fonction pour balayer le paramètre T et récupérer les résultats.
void sweepTemp(Ising::Lattice &lat, MC::Parameters options, uint samplingPoints) {
    MC::Properties props = MC::thermalizeLattice(lat, options, 0.1, 5, samplingPoints);
    double *X = susceptibility(props, samplingPoints, options.kB);
    double *cv = calorific_coefficient(props, samplingPoints, options.kB);
    
    auto E = np::array(props.E, samplingPoints);
    auto E_sq = np::array(props.E_sq, samplingPoints);
    auto M = np::array(props.M, samplingPoints);
    auto M_sq = np::array(props.M_sq, samplingPoints);
    auto M_abs = np::array(props.M_abs, samplingPoints);
    auto T = np::array(props.T, samplingPoints);
    auto X_py = np::array(X, samplingPoints);
    auto cv_py= np::array(cv, samplingPoints);
    
    np::savetxt(E, "res/E.csv", ";");
    np::savetxt(E_sq, "res/E_sq.csv", ";");
    np::savetxt(M, "res/M.csv", ";");
    np::savetxt(M_sq, "res/M_sq.csv", ";");
    np::savetxt(M_abs, "res/M_abs.csv", ";");
    np::savetxt(T, "res/T.csv", ";");
    np::savetxt(X_py, "res/X.csv", ";");
    np::savetxt(cv_py, "res/Cv.csv", ";");
}

/// @brief Fonction pour balayer le paramètre h et récupérer les résultats.
void sweepMagnetic(Ising::Lattice &lat, MC::Parameters options, uint samplingPoints) {
    MC::Properties props = MC::thermalizeLattice(lat, options, 0.1, 5, samplingPoints);
    double *X = susceptibility(props, samplingPoints, options.kB);
    double *cv = calorific_coefficient(props, samplingPoints, options.kB);
    
    auto E = np::array(props.E, samplingPoints);
    auto E_sq = np::array(props.E_sq, samplingPoints);
    auto M = np::array(props.M, samplingPoints);
    auto M_sq = np::array(props.M_sq, samplingPoints);
    auto M_abs = np::array(props.M_abs, samplingPoints);
    auto T = np::array(props.T, samplingPoints);
    auto X_py = np::array(X, samplingPoints);
    auto cv_py= np::array(cv, samplingPoints);
    
    np::savetxt(E, "res/E.csv", ";");
    np::savetxt(E_sq, "res/E_sq.csv", ";");
    np::savetxt(M, "res/M.csv", ";");
    np::savetxt(M_sq, "res/M_sq.csv", ";");
    np::savetxt(M_abs, "res/M_abs.csv", ";");
    np::savetxt(T, "res/T.csv", ";");
    np::savetxt(X_py, "res/X.csv", ";");
    np::savetxt(cv_py, "res/Cv.csv", ";");
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
    MC::Parameters options = MC::parameters(5e6, 150000, 0.5, 0.0002, MC::metropolisIteration, 0.1, 1, 0, 1);
    // MC::Parameters options = MC::parameters(50, 5, 2, 0.0002, MC::wolffIteration, 0.01, 1, 0, 1);

    // * Démonstration de l'algorithme visuellement
    // showAlgorithm(lat, options, 0.1, 5, 20);

    // * Génération des données pour T qui varie

    // * Génération des données pour h qui varie

    py::closePython();
    return 0;
}   