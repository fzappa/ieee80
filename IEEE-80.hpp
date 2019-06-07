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

double fatorCorrecaoCs(const Dados &);         // Pagina 23 - EQ 27 - IEEE Std 80-2013
double tensaoPasso50kg(const Dados &);         // Pagina 28 - EQ 29 - IEEE Std 80-2013
double tensaoToque50kg(const Dados &);         // Pagina 29 - EQ 32 - IEEE Std 80-2013
double secaoCabo(const Dados &);               // Pagina 42 - EQ 37 - IEEE Std 80-2013
double resistenciaAterramento(const Dados &);  // Pagina 67 - EQ 57 - IEEE Std 80-2013
double tensaoToqueMalha(const Dados &);        // Pagina 94 - EQ 85 - IEEE Std 80-2013
double fatorKm(const Dados &);                 // Pagina 94 - EQ 86 - IEEE Std 80-2013
double fatorKi(const Dados &);                 // Pagina 95 - EQ 94 - IEEE Std 80-2013
double tensaoPassoMalha(const Dados &);        // Pagina 96 - EQ 97 - IEEE Std 80-2013
double fatorKs(const Dados &);                 // Pagina 96 - EQ 99 - IEEE Std 80-2013

double diametroCabo(const Dados &);
double calculaMalha(const Dados &);
double compTotalCondutores(const Dados &);

double GPR(const Dados &);

}  // namespace ieee80

#endif
