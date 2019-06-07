#include <iostream>
#include<stdlib.h>
#include "IEEE-80.hpp"

using namespace std;

int main(){

    ieee80::Dados data;

    double alpha, beta, rho;

    ieee80::aparentResistivity(data, alpha, beta, rho);

    data.cabo = ieee80::CobreMole;
    data.solo = ieee80::Brita;
    cout << "Resistencia aparente do Cobre Comercial em Brita" << endl;
    cout << "\t alpha: " << alpha << "\t beta: " << beta << "\t rho_a: " << rho << endl;
    cout <<  "Secao do cabo [mm2]: " << ieee80::cableSection(data) << endl;
    cout << "Diametro do cabo [m]: " << ieee80::cableDiameter(data) << endl;

    cout <<  "Fator de correcao Cs: " << ieee80::csCorrectionFactor(data) << endl;
    
    cout <<  "Tensao de toque [V]: " << ieee80::touchVoltage50kg(data) << endl;
    cout <<  "Tensao de passo [V] : " << ieee80::stepVoltage50kg(data) << endl;
    
    cout <<  "Fator Km: " << ieee80::kmFactor(data) << endl;
    cout << "Fator Ki: " << ieee80::kiFactor(data) << endl;
    cout << "Fator Ks: " << ieee80::ksFactor(data) << endl;

    cout << "Tensão de toque na malha [V] : " << ieee80::touchVoltageMesh(data) << endl;
    cout << "Tensão de passo na malha [V] : " << ieee80::stepVoltageMesh(data) << endl;

    cout << "Comprimento total da malha [m]: " << ieee80::meshCalc(data) << endl;
    cout << "Total de condutores (cabos + hastes) [m]: " << ieee80::overallConductorLenght(data) << endl;
    
    //Calculos a partir do total de condutores calculados
    ieee80::Dados data2 = data;
    data2.comprimentoMetros = ieee80::overallConductorLenght(data);
    cout << "Resistividade total: " << ieee80::groundResistance(data2) << endl;
    cout << "GPR: " << ieee80::GPR(data2) << endl;

    return 0;

}