#include <algorithm>  // find, max
#include <iostream>
#include <vector>

#include <boost/math/constants/constants.hpp> 

#include "IEEE-80.hpp"

using namespace std;


struct structSolo {
    string tipo;
    double resistividade;
};

static const vector<structSolo> Solo{
    {"Pantano", 50.0},	 {"Lodol", 100.0},	   {"Humus", 150.0},
    {"Areia Argilosa", 200.0}, {"Areia Silicosa", 1000.0}, {"Brita", 3000.0},
    {"Calcareo", 5000.0},      {"Granito", 10000.0},       {"NaoDef", 3000.0}};

struct structCabo {
    string tipo;
    double Ar;
    double K0;
    double Pr;
    double TCAP;
};

static const vector<structCabo> Cabo{
    {"Cobre mole", 0.00393, 234, 1.72, 3.4},
    {"Cobre comercial", 0.00381, 242, 1.78, 3.4},
    {"Cobre com alma de aço", 0.00378, 245, 10.1, 3.8},
    {"Alumínio com alma de aço", 0.0036, 258, 8.48, 3.561},
    {"Aço 1020", 0.00377, 245, 15.9, 3.8},
    {"Aço inoxidável", 0.00377, 245, 17.5, 4.4},
    {"Zinco", 0.0032, 293, 20.1, 3.9},
    {"Aço inoxidável 304", 0.0013, 749, 72.0, 4.0}};

// ResAparente(area, p1, d1, p2, N, alpha, beta, rhoa)
void ieee80::resAparente(const Dados &data, double &alpha, double &beta,
                         double &rhoa)
{
    double r;
    r     = sqrt((data.larguraMetros * data.comprimentoMetros) / M_PI);
    alpha = r / data.diametroMalhaD1Metros;
    beta  = data.rho2Ohm / data.rho1Ohm;
    rhoa  = data.N * data.rho1Ohm;
}

// Page 23 - EQ 27 - IEEE Std 80-2013
// Correction factor
double ieee80::fatorCorrecaoCs(const Dados &data)
{
    return (1 - ((0.09 * (1 - (data.rho2Ohm / data.rho1Ohm))) /
                 (2 * data.alturaMalhaMetros + 0.09)));
}

// Page 28 - EQ 29 - IEEE Std 80-2013
// 8.4 Step and touch voltage criteria 
double ieee80::tensaoPasso50kg(const Dados &data)
{
    return ((1000 + 6 * fatorCorrecaoCs(data) * Solo[data.solo].resistividade) *
            (0.116 / sqrt(data.tempoDeCurtoSegundos)));
}

// Page 29 - EQ 32 - IEEE Std 80-2013
// 8.4 Step and touch voltage criteria
double ieee80::tensaoToque50kg(const Dados &data)
{
    return (
        (1000 + 1.5 * fatorCorrecaoCs(data) * Solo[data.solo].resistividade) *
        (0.116 / sqrt(data.tempoDeCurtoSegundos)));
}

//  Pagina 42 - EQ 37 - IEEE Std 80-2013
//  11.3.1 Symmetrical currents
//  TCAP = is the thermal capacity per unit volume from Table 1, in J/(cm3·°C)
//  (further defined in 11.3.1.1) (vem da tabela dos condutores) Ar = is the
//  thermal coefficient of resistivity at reference temperature Tr in 1/°C
//  (vem da tabela dos condutores) Pr = is the resistivity of the ground
//  conductor at reference temperature Tr in µΩ-cm K0 = 1/a0 or (1/ar) – Tr in
//  ºC Tm = is the maximum allowable temperature in °C Ta = is the ambient
//  temperature in °C
double ieee80::secaoCabo(const Dados &data)
{
    double T0, T1;

    T0 = (Cabo[data.cabo].TCAP) /
         (data.tempoDeCurtoSegundos * Cabo[data.cabo].Ar * Cabo[data.cabo].Pr);
    T1 = (Cabo[data.cabo].K0 + data.tempMaxMalha) /
         (Cabo[data.cabo].K0 + data.tempAmbienteCelsius);

    return (data.correnteMalhaAmperes * (197.4 / sqrt(T0 * log(T1)))) *
           0.000506707;
}

// Page 67 - EQ 57 - IEEE Std 80-2013
// Ground resistance
// Rgm = Rg(p2, Lt, area, Hmalha)
double ieee80::resistenciaAterramento(const Dados &data)
{
    double eq1, eq2, compmalha;

    eq1 = (1.0 / sqrt(20.0 * data.area));
    eq2 = 1 / (1 + data.alturaMalhaMetros * sqrt(20.0 / data.area));

    // Ajustes
    compmalha = data.comprimentoMetros;
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
double ieee80::tensaoToqueMalha(const Dados &data)
{
    double temp;
    Dados temp2 = data; 

    temp = (data.rho2Ohm * data.correnteMalhaAmperes * fatorKm(data) *
            fatorKi(data)) /
           compTotalCondutores(temp2);

    return temp;
}

// Page 94 - EQ 86 - IEEE Std 80-2013
// 16.5.1 Mesh voltage
// The geometrical factor Km
// Km(DistCond, Hmalha, 1, DiametroCabo, NDmalha)
double ieee80::fatorKm(const Dados &data)
{
    double Kii, Kh, n;
    double T0, T1, T2, T3, T4, T5;
    double profundidadeMalha = 1.0;  // Referencia da norma
    double dCabo	     = diametroCabo(data);

    n = max(data.nCondComp, data.nCondLarg);

    Kii = pow(2.0 * n, (-2.0 / n));
    Kh  = sqrt(1 + (data.alturaMalhaMetros / profundidadeMalha));

    T0 = 1 / (2 * M_PI);
    T1 = (data.distanciaCondutoresMetros * data.distanciaCondutoresMetros) /
         (16 * data.alturaMalhaMetros * dCabo);

    T2 = ((data.distanciaCondutoresMetros + 2 * data.alturaMalhaMetros) *
          (data.distanciaCondutoresMetros + 2 * data.alturaMalhaMetros)) /
         (8 * data.distanciaCondutoresMetros * dCabo);

    T3 = data.alturaMalhaMetros / (4 * dCabo);
    T4 = Kii / Kh;
    T5 = 8 / (M_PI * (2 * n - 1));

    return (T0 * (log(T1 + T2 - T3) + T4 * log(T5)));
}

// Page 95 - EQ 94 - IEEE Std 80-2013
// 16.5.1 Mesh voltage
// The irregularity factor Ki
double ieee80::fatorKi(const Dados &data)
{
    return 0.644 + 0.148 * sqrt(data.nCondLarg * data.nCondComp);
}

// Page 96 - EQ 97 - IEEE Std 80-2013
// 16.5.2 Step voltage
// Epm(p2, Imalha, Ks1, Ki1, Lt)
double ieee80::tensaoPassoMalha(const Dados &data)
{

    double temp;
    Dados temp2 = data;

    temp = (data.rho2Ohm * data.correnteMalhaAmperes * fatorKs(data) *
            fatorKi(data)) /
           compTotalCondutores(temp2);

    return temp;
}

// Page 96 - EQ 99 - IEEE Std 80-2013
// 16.5.2 Step voltage
// Fator de espacamento
double ieee80::fatorKs(const Dados &data)
{
    double n;
    double p1, p2, p3;
    n = max(data.nCondComp, data.nCondLarg);

    p1 = (1 / M_PI) * (1.0 / (2 * data.alturaMalhaMetros));
    p2 = (1.0 / (data.distanciaCondutoresMetros + data.alturaMalhaMetros));
    p3 = (1 / data.distanciaCondutoresMetros) * pow(1 - 0.5, n - 2);

    return (p1 + p2 + p3);
}

//-------------- Second part ----------------------//

double ieee80::diametroCabo(const Dados &data)
{
    return (2 * sqrt(secaoCabo(data) / M_PI) * 0.001);  // metros
}

// Page 92 - Figure 32 —Design procedure block diagram - IEEE Std 80-2013
// In this one the algorithm was modified to return the total of cables
// used to achieve a particular ground resistance value

// CalculaLTmax(RMinmalha, rho2, areaMalha, Hmalha)
double ieee80::calculaMalha(const Dados &data)
{

    // Calculate the maximum mesh length for the resistance
    // specified
    Dados data2 = data;

    data2.comprimentoMetros = 1.0;
    data2.larguraMetros     = 1.0;

    double resMax = resistenciaAterramento(data2);

    while (resMax > data.resistenciaMinMalhaOhms) {
        data2.comprimentoMetros += 1.0;
        data2.larguraMetros += 1.0;

        if (data2.comprimentoMetros > 1000000.0) {
            cout << "Não é possivel dimensionar a malha" << endl;
            break;
        }
        else {
            resMax = resistenciaAterramento(data2);
        }
    }

    return data2.comprimentoMetros;
}

double ieee80::compTotalCondutores(const Dados &data)
{
    double Lt;
    Lt = calculaMalha(data);  // total cables
    Lt += data.hastes * 3;

    return Lt;
}

double ieee80::GPR(const Dados &data)
{
    return (resistenciaAterramento(data) * data.correnteMalhaAmperes);
}
