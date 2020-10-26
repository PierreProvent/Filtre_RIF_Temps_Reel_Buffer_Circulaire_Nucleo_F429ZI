#include "mbed.h"

#define fech 40000 // Fréquence d'échantillonnage souhaitée
#define NB_COEFF 21 // Nombre de coefficients du filtre RIF

Serial pc(USBTX, USBRX);

AnalogIn entree_analogique(PC_3); // A2 connecteur Arduino
AnalogOut sortie_analogique(PA_5); // D13 Arduino
PwmOut sortie(PE_9); // D6 Arduino
DigitalOut visu(PG_9) ; // D0 Arduino
Ticker ADC_DAC ;

// Filtre RIF passe_bas fc = 2000 Hz généré avec Octave fe réelle = 40 kHz
// Fe = 2 dans Octave --> Fc Octavve = 2000/40000 = 0.075
// b = fir1(20,0.075) ; 21 = nb coeffients dans b
float coeff_filtre[NB_COEFF] = {0.0026603,0.0045069,0.0091798,0.017556,
                                0.029809,0.045263,0.062429,0.079237,0.093403,
                                0.10286,0.10619,0.10286,0.093403,0.079237,
                                0.062429,0.045263,0.029809,0.017556,0.0091798,
                                0.0045069,0.0026603
                               } ;

// Temps de traitement d'un échantillon 2.3 us
float firc(float xn)
{
    static float in[NB_COEFF] ;
    int j, indice  ;
    static int i = 0 ;
    float yn = 0 ;
    in[i] = xn ;        // (1)
    for (j = 0 ; j < NB_COEFF ; j++) {  // (2)
        indice = (i - j + NB_COEFF) % NB_COEFF ;
        yn += coeff_filtre[j]* in[indice] ;
    }
    i++;
    if (i == NB_COEFF) i = 0; // (3)
    return(yn);
}

void filtrage()
{
    float x,y ;
    x = entree_analogique.read();
    visu = 1 ;
    y = firc(x) ;
    visu = 0 ;
    sortie_analogique.write(y) ;
}

int main()
{
    ADC_DAC.attach_us(&filtrage,1000000/fech);
    sortie.period_us(500);
    sortie.write(0.5f) ; // rapport cyclique 1/2
    pc.printf("\nEssai filtrage numerique temps reel fech = %d Hz\n",fech);
    pc.printf("Connectez la sortie D6 (signal a filtrer) sur l'entree A2 \n");
    pc.printf("Visualisez signal filtre sur la broche D13 \n");
    pc.printf("Traitement en cours : ");
    while(1) {
    }
}
