#ifndef IEEE_80_HPP
#define IEEE_80_HPP

namespace ieee80 {

enum enumSolo {
    Pantano,
    Lodo,
    Humus,
    AreiaArgilosa,
    AreiaSilicosa,
    Brita,
    Calcareo,
    Granito,
    SoloNaoDefinido
};

enum enumCabo {
    CobreMole,
    CobreComercial,
    CobreAco,
    AluminioAco,
    Aco1020,
    AcoInox,
    Zinco,
    AcoInox304,
    CaboNaoDefinido
};

struct Dados {
    double larguraMetros             = 70.0;
    double comprimentoMetros         = 70.0;
    double rho1Ohm                   = 2500.0;
    double rho2Ohm                   = 400.0;
    double correnteMalhaAmperes      = 1200.0;
    unsigned int hastes              = 10;
    double resistenciaMinMalhaOhms   = 2.78;
    double tempAmbienteCelsius       = 40.0;
    double diametroMalhaD1Metros     = 0.102;
    double N                         = 0.67;
    double tempoDeCurtoSegundos      = 0.5;
    double alturaMalhaMetros         = 0.5;
    double distanciaCondutoresMetros = 7.0;
    double tempMaxMalha              = 850.0;
    unsigned int solo                = Brita;
    unsigned int cabo                = CobreComercial;
    double area                      = larguraMetros * comprimentoMetros;
    double nCondLarg                 = larguraMetros / distanciaCondutoresMetros;
    double nCondComp                 = comprimentoMetros / distanciaCondutoresMetros;
};


void aparentResistivity(const Dados &, double &, double &,
                 double &);  // Resistencia aparente da malha
                                                // From IEEE Std 80-2013
double csCorrectionFactor(const Dados &);          // Page 23 - EQ 27
double stepVoltage50kg(const Dados &);          // Page 28 - EQ 29
double touchVoltage50kg(const Dados &);          // Page 29 - EQ 32
double cableSection(const Dados &);                // Page 42 - EQ 37
double groundResistance(const Dados &);   // Page 67 - EQ 57
double touchVoltageMesh(const Dados &);         // Page 94 - EQ 85
double kmFactor(const Dados &);                  // Page 94 - EQ 86
double kiFactor(const Dados &);                  // Page 95 - EQ 94
double stepVoltageMesh(const Dados &);         // Page 96 - EQ 97
double ksFactor(const Dados &);                  // Page 96 - EQ 99

double cableDiameter(const Dados &);
double meshCalc(const Dados &);
double overallConductorLenght(const Dados &);

double GPR(const Dados &);

}  // namespace ieee80

#endif
