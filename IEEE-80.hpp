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
#ifndef IEEE_80_HPP
#define IEEE_80_HPP

namespace ieee80 {

enum enumSoil {
    Pantano,
    Sludge,
    Humus,
    ClaySand,
    SiliceousSand,
    CrushedStone,
    Limestone,
    Granite,
    Default
};

// Types os conductor cables - Page 46 - Table 1 - IEEE Std 80-2013
enum enumCable {
    CopperSoft,         // Copper, annealed soft-drawn
    CopperComercial,    // Copper, commercial hard-drawn
    CopperSteel,        // Copper-clad steel wire
    AluminumSteel,      // Aluminum-clad steel wire
    Steel1020,          // Steel, 1020
    StainlessSteel,     // Stainless-clad steel rod
    Zinco,              // Zinc-coated steel rod
    StainlessSteel304,  // Stainless steel, 304
    NotDefined
};

// Two-layer stratified soil model
struct Data {
    double widthMeters             = 70.0;
    double lengthMeters            = 70.0;
    double rho1Ohm                 = 2500.0;  // layer 1 resistivity
    double rho2Ohm                 = 400.0;   // layer 2 resistivity
    double currentMeshAmperes      = 1200.0;
    unsigned int rods              = 10;
    double resistanceMinMeshOhms   = 2.78;
    double ambientTempCelsius      = 40.0;
    double diameterMeshD1Meter     = 0.102;
    double N                       = 0.67;
    double shortCircuitTimeSeconds = 0.5;
    double heightMeshMeters        = 0.5;
    double spacingConductorsMeters = 7.0;
    double maximumMeshTemperature  = 850.0;
    unsigned int soil              = CrushedStone;
    unsigned int cable             = CopperComercial;
    double area                    = widthMeters * lengthMeters;
    double numberConductorsWidth   = widthMeters / spacingConductorsMeters;
    double numberConductorsLength  = lengthMeters / spacingConductorsMeters;
};

void aparentResistivity(const Data &, double &, double &,
                        double &);        // Apparent mesh resistivity
                                          // From IEEE Std 80-2013
double csCorrectionFactor(const Data &);  // Page 23 - EQ 27
double stepVoltage50kg(const Data &);     // Page 28 - EQ 29
double touchVoltage50kg(const Data &);    // Page 29 - EQ 32
double cableSection(const Data &);        // Page 42 - EQ 37
double groundResistance(const Data &);    // Page 67 - EQ 57
double touchVoltageMesh(const Data &);    // Page 94 - EQ 85
double kmFactor(const Data &);            // Page 94 - EQ 86
double kiFactor(const Data &);            // Page 95 - EQ 94
double stepVoltageMesh(const Data &);     // Page 96 - EQ 97
double ksFactor(const Data &);            // Page 96 - EQ 99

double cableDiameter(const Data &);
double meshCalc(const Data &);
double overallConductorLenght(const Data &);

double GPR(const Data &);

}  // namespace ieee80

#endif
