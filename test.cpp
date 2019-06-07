#include <iostream>
#include<stdlib.h>
#include "IEEE-80.hpp"

using namespace std;

int main(){

    ieee80::Dados data;

    double alpha, beta, rho;

    ieee80::resAparente(data, alpha, beta, rho);

    data.cabo = ieee80::CobreMole;
    data.solo = ieee80::Brita;
    cout << "Resistencia aparente do Cobre Comercial em Brita" << endl;
    cout << "\t alpha: " << alpha << "\t beta: " << beta << "\t rho_a: " << rho << endl;
    cout <<  "Secao do cabo [mm2]: " << ieee80::secaoCabo(data) << endl;
    cout << "Diametro do cabo [m]: " << ieee80::diametroCabo(data) << endl;

    cout <<  "Fator de correcao Cs: " << ieee80::fatorCorrecaoCs(data) << endl;
    
    cout <<  "Tensao de toque [V]: " << ieee80::tensaoToque50kg(data) << endl;
    cout <<  "Tensao de passo [V] : " << ieee80::tensaoPasso50kg(data) << endl;
    
    cout <<  "Fator Km: " << ieee80::fatorKm(data) << endl;
    cout << "Fator Ki: " << ieee80::fatorKi(data) << endl;
    cout << "Fator Ks: " << ieee80::fatorKs(data) << endl;

    cout << "Tensão de toque na malha [V] : " << ieee80::tensaoToqueMalha(data) << endl;
    cout << "Tensão de passo na malha [V] : " << ieee80::tensaoPassoMalha(data) << endl;

    cout << "Comprimento total da malha [m]: " << ieee80::calculaMalha(data) << endl;
    cout << "Total de condutores (cabos + hastes) [m]: " << ieee80::compTotalCondutores(data) << endl;
    
    //Calculos a partir do total de condutores calculados
    ieee80::Dados data2 = data;
    data2.comprimentoMetros = ieee80::compTotalCondutores(data);
    cout << "Resistividade total: " << ieee80::resistenciaAterramento(data2) << endl;
    cout << "GPR: " << ieee80::GPR(data2) << endl;

    return 0;

}