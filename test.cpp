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
#include <iostream>
#include<stdlib.h>
#include "IEEE-80.hpp"

// Compile: clang++ IEEE-80.cpp test.cpp -o test
using namespace std;

int main(){

    ieee80::Data data;

    double alpha, beta, rho;

    ieee80::aparentResistivity(data, alpha, beta, rho);

    data.cable = ieee80::CopperSoft;
    data.soil = ieee80::CrushedStone;
    cout << "Apparent resistance of commercial copper in crushed stone" << endl;
    cout << "\t alpha: " << alpha << "\t beta: " << beta << "\t rho_a: " << rho << endl;
    cout <<  "Cable section [mm2]: " << ieee80::cableSection(data) << endl;
    cout << "Cable diameter [m]: " << ieee80::cableDiameter(data) << endl;

    cout <<  "Correction factor Cs: " << ieee80::csCorrectionFactor(data) << endl;
    
    cout <<  "Touch voltage [V]: " << ieee80::touchVoltage50kg(data) << endl;
    cout <<  "Step voltage [V] : " << ieee80::stepVoltage50kg(data) << endl;
    
    cout <<  "Km factor: " << ieee80::kmFactor(data) << endl;
    cout << "Ki factor: " << ieee80::kiFactor(data) << endl;
    cout << "Ks factor: " << ieee80::ksFactor(data) << endl;

    cout << "Mesh touch voltage [V] : " << ieee80::touchVoltageMesh(data) << endl;
    cout << "Mesh step voltage [V] : " << ieee80::stepVoltageMesh(data) << endl;

    cout << "Total mesh length [m]: " << ieee80::meshCalc(data) << endl;
    cout << "Total conductors (cables + rods) [m]: " << ieee80::overallConductorLenght(data) << endl;
    
    //Calculations from total calculated drivers
    ieee80::Data data2 = data;
    data2.lengthMeters = ieee80::overallConductorLenght(data);
    cout << "Total resistivity [Ohm]: " << ieee80::groundResistance(data2) << endl;
    cout << "Ground Potential Rise (GPR) [V]: " << ieee80::GPR(data2) << endl;

    return 0;

}