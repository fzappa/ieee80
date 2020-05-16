//**********************************************
//  Library for calculating grounding parameters based on:
// Guide for Safety in AC Substation Grounding - IEEE-80
//
//  Implemented for Substation discipline in the
//  Electrical Engineering course at CEFET / RJ
//
//
//  License: GPLv3
//  Author: Alan Franco - https://github.com/fzappa/ieee80
//  Rev.: 05/03/2016 (PT_BR)
//*********************************************

#include "IEEE-80.hpp"

#include <boost/math/constants/constants.hpp>

#include <algorithm>  // find, max
#include <iostream>
#include <vector>

struct structSoil {
    std::string type;
    double resistivity;
};

// typical values of resistivity of some types of soil
static const std::vector<structSoil> Soil{
    {"Pantano", 50.0}, {"Sludge", 100.0}, {"Humus", 150.0}, {"Clay Sand", 200.0},
    {"Siliceous Sand", 1000.0}, {"Crushed Stone", 3000.0}, {"Limestone", 5000.0},
    {"Granite", 10000.0}, {"Default", 3000.0}};

// Page 42 - EQ 37 - IEEE Std 80-2013
struct structCable {
    std::string type;
    double Ar;    // Thermal coefficient of resistivity
    double K0;    // Thermal capacity per unit volume
    double Pr;    // Resistivity of the ground
    double TCAP;  // Thermal capacity per unit volume
};

// Types os conductor cables - Page 46 - Table 1 - IEEE Std 80-2013
// Description, alpha_r factor, K0, Resistivity, Thermal Capacity
static const std::vector<structCable> Cable{{"Copper, annealed soft-drawn", 0.00393, 234, 1.72, 3.4},
                                            {"Copper, commercial hard-drawn", 0.00381, 242, 1.78, 3.4},
                                            {"Copper-clad steel wire", 0.00378, 245, 10.1, 3.8},
                                            {"Aluminum-clad steel wire", 0.0036, 258, 8.48, 3.561},
                                            {"Steel, 1020", 0.00377, 245, 15.9, 3.8},
                                            {"Stainless-clad steel rod", 0.00377, 245, 17.5, 4.4},
                                            {"Zinc-coated steel rod", 0.0032, 293, 20.1, 3.9},
                                            {"Stainless steel, 304", 0.0013, 749, 72.0, 4.0}};

// aparentResistivity(area, p1, d1, p2, N, alpha, beta, rhoa)
void ieee80::aparentResistivity(const Data &data, double &alpha, double &beta, double &rhoa) {
    double r;
    r     = sqrt((data.widthMeters * data.lengthMeters) / M_PI);
    alpha = r / data.diameterMeshD1Meter;
    beta  = data.rho2Ohm / data.rho1Ohm;
    rhoa  = data.N * data.rho1Ohm;
}

// Page 23 - EQ 27 - IEEE Std 80-2013
// Correction factor
double ieee80::csCorrectionFactor(const Data &data) {
    return (1 -
            ((0.09 * (1 - (data.rho2Ohm / data.rho1Ohm))) / (2 * data.heightMeshMeters + 0.09)));
}

// Page 28 - EQ 29 - IEEE Std 80-2013
// 8.4 Step and touch voltage criteria
double ieee80::stepVoltage50kg(const Data &data) {
    return ((1000 + 6 * csCorrectionFactor(data) * Soil[data.soil].resistivity) *
            (0.116 / sqrt(data.shortCircuitTimeSeconds)));
}

// Page 29 - EQ 32 - IEEE Std 80-2013
// 8.4 Step and touch voltage criteria
double ieee80::touchVoltage50kg(const Data &data) {
    return ((1000 + 1.5 * csCorrectionFactor(data) * Soil[data.soil].resistivity) *
            (0.116 / sqrt(data.shortCircuitTimeSeconds)));
}

//  Pagina 42 - EQ 37 - IEEE Std 80-2013
//  11.3.1 Symmetrical currents
//  TCAP = is the thermal capacity per unit volume from Table 1, in J/(cm3·°C)
//  (further defined in 11.3.1.1) (comes from the table of conductors) Ar = is the
//  thermal coefficient of resistivity at reference temperature Tr in 1/°C
//  (comes from the table of conductors) Pr = is the resistivity of the ground
//  conductor at reference temperature Tr in µΩ-cm K0 = 1/a0 or (1/ar) – Tr in
//  ºC Tm = is the maximum allowable temperature in °C Ta = is the ambient
//  temperature in °C
double ieee80::cableSection(const Data &data) {
    double T0, T1;

    T0 = (Cable[data.cable].TCAP) /
         (data.shortCircuitTimeSeconds * Cable[data.cable].Ar * Cable[data.cable].Pr);
    T1 = (Cable[data.cable].K0 + data.maximumMeshTemperature) /
         (Cable[data.cable].K0 + data.ambientTempCelsius);

    return (data.chainMeshAmperes * (197.4 / sqrt(T0 * log(T1)))) * 0.000506707;
}

// Page 67 - EQ 57 - IEEE Std 80-2013
// Ground resistance
// Rgm = Rg(p2, Lt, area, Hmalha)
double ieee80::groundResistance(const Data &data) {
    double eq1, eq2, compmalha;

    eq1 = (1.0 / sqrt(20.0 * data.area));
    eq2 = 1 / (1 + data.heightMeshMeters * sqrt(20.0 / data.area));

    // Ajustes
    compmalha = data.lengthMeters;
    if (compmalha < 1.0)
        compmalha = 1.0;

    if (eq1 < 0.001)
        eq1 = 0.001;

    if (eq2 < 0.001)
        eq2 = 0.001;

    return (data.rho2Ohm * ((1 / compmalha) + (eq1) * (1 + (eq2))));
}

// Page 94 - EQ 85 - IEEE Std 80-2013
// 16.5.1 Mesh voltage
double ieee80::touchVoltageMesh(const Data &data) {
    double temp;
    Data temp2 = data;

    temp = (data.rho2Ohm * data.chainMeshAmperes * kmFactor(data) * kiFactor(data)) /
           overallConductorLenght(temp2);

    return temp;
}

// Page 94 - EQ 86 - IEEE Std 80-2013
// 16.5.1 Mesh voltage
// The geometrical factor Km
// Km(DistCond, Hmalha, 1, cableDiameter, NDmalha)
double ieee80::kmFactor(const Data &data) {
    double Kii, Kh, n;
    double T0, T1, T2, T3, T4, T5;
    double profundidadeMalha = 1.0;  // Standard reference
    double dCable            = cableDiameter(data);

    n = std::max(data.numberConductorsLength, data.numberConductorsWidth);

    Kii = pow(2.0 * n, (-2.0 / n));
    Kh  = sqrt(1 + (data.heightMeshMeters / profundidadeMalha));

    T0 = 1 / (2 * M_PI);
    T1 = (data.spacingConductorsMeters * data.spacingConductorsMeters) /
         (16 * data.heightMeshMeters * dCable);

    T2 = ((data.spacingConductorsMeters + 2 * data.heightMeshMeters) *
          (data.spacingConductorsMeters + 2 * data.heightMeshMeters)) /
         (8 * data.spacingConductorsMeters * dCable);

    T3 = data.heightMeshMeters / (4 * dCable);
    T4 = Kii / Kh;
    T5 = 8 / (M_PI * (2 * n - 1));

    return (T0 * (log(T1 + T2 - T3) + T4 * log(T5)));
}

// Page 95 - EQ 94 - IEEE Std 80-2013
// 16.5.1 Mesh voltage
// The irregularity factor Ki
double ieee80::kiFactor(const Data &data) {
    return 0.644 + 0.148 * sqrt(data.numberConductorsWidth * data.numberConductorsLength);
}

// Page 96 - EQ 97 - IEEE Std 80-2013
// 16.5.2 Step voltage
// Epm(p2, Imalha, Ks1, Ki1, Lt)
double ieee80::stepVoltageMesh(const Data &data) {

    double temp;
    Data temp2 = data;

    temp = (data.rho2Ohm * data.chainMeshAmperes * ksFactor(data) * kiFactor(data)) /
           overallConductorLenght(temp2);

    return temp;
}

// Page 96 - EQ 99 - IEEE Std 80-2013
// 16.5.2 Step voltage
// Spacing factor
double ieee80::ksFactor(const Data &data) {
    double n;
    double p1, p2, p3;
    n = std::max(data.numberConductorsLength, data.numberConductorsWidth);

    p1 = (1 / M_PI) * (1.0 / (2 * data.heightMeshMeters));
    p2 = (1.0 / (data.spacingConductorsMeters + data.heightMeshMeters));
    p3 = (1 / data.spacingConductorsMeters) * pow(1 - 0.5, n - 2);

    return (p1 + p2 + p3);
}

//-------------- Second part ----------------------//

double ieee80::cableDiameter(const Data &data) {
    return (2 * sqrt(cableSection(data) / M_PI) * 0.001);  // metros
}

// Page 92 - Figure 32 —Design procedure block diagram - IEEE Std 80-2013
// The algorithm here has been modified to return the total length
// of cables used to achieve a particular ground resistance value
//
double ieee80::meshCalc(const Data &data) {

    // Calculate the maximum mesh length for the resistance
    // specified
    Data data2 = data;

    data2.lengthMeters = 1.0;
    data2.widthMeters  = 1.0;

    double resMax = groundResistance(data2);

    while (resMax > data.resistanceMinMeshOhms) {
        data2.lengthMeters += 1.0;
        data2.widthMeters += 1.0;

        // def a limit max
        if (data2.lengthMeters > 1000000.0) {
            std::cout << "It is not possible to dimension the mesh" << std::endl;
            break;
        } else {
            resMax = groundResistance(data2);
        }
    }

    return data2.lengthMeters;
}

double ieee80::overallConductorLenght(const Data &data) {
    double Lt;
    Lt = meshCalc(data);  // total cables
    Lt += data.rods * 3;

    return Lt;
}

// Ground Potential Rise
// The maximum electrical potential that a ground electrode may attain relative
// to a distant grounding point assumed to be at the potential of remote earth.
double ieee80::GPR(const Data &data) {
    return (groundResistance(data) * data.chainMeshAmperes);
}
