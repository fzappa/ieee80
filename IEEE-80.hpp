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


void resAparente(const Dados &, double &, double &,
                 double &);  // Resistencia aparente da malha
                                                // From IEEE Std 80-2013
double fatorCorrecaoCs(const Dados &);          // Page 23 - EQ 27
double tensaoPasso50kg(const Dados &);          // Page 28 - EQ 29
double tensaoToque50kg(const Dados &);          // Page 29 - EQ 32
double secaoCabo(const Dados &);                // Page 42 - EQ 37
double resistenciaAterramento(const Dados &);   // Page 67 - EQ 57
double tensaoToqueMalha(const Dados &);         // Page 94 - EQ 85
double fatorKm(const Dados &);                  // Page 94 - EQ 86
double fatorKi(const Dados &);                  // Page 95 - EQ 94
double tensaoPassoMalha(const Dados &);         // Page 96 - EQ 97
double fatorKs(const Dados &);                  // Page 96 - EQ 99

double diametroCabo(const Dados &);
double calculaMalha(const Dados &);
double compTotalCondutores(const Dados &);

double GPR(const Dados &);

}  // namespace ieee80

#endif
