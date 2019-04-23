#include "MTboard.h"
#define LONG_MAX 10  //maxima longitud de la serpiente
#define VMAX 650.0 //Velocidad máxima de la serpiente mSg entre pasos
#define VMIN 180.0 //velocidad mínima
#define STEPVEL (VMAX-VMIN)/LONG_MAX
#define LED_ON 1

MTboard mtBoard;
struct {
  char fila;
  char columna;
} serpiente[LONG_MAX], movimiento, fruta;

enum {Left = 1, Right = 2, Up = 3, Down = 4} direccion;
boolean hayFruta = false;
int intervaloFrutaActual;
char longActual = 1;
int velocidad = VMAX;//250

void setup() {

  Serial.begin(57600);
  randomSeed(analogRead(4));
  iniJuego();
  delay(3000);
  mtBoard.textoScroll("Bienvenido a ArduGames SNAKE tienes que comer 15 manzanas");
  mtBoard.drawBitmap(ALEGRE);
  delay(1000);


}

void loop() {
  long tiempo = millis();
  mtBoard.atiendeSerie();
  byte k = 255;
  while (((millis() - tiempo) < velocidad) && (((k = mtBoard.leePulsador()) == 255))); //espera a pulsar o timeout
  if (k != 255) // si es porque se ha pulsado
    switch ( direccion)
    {
      case Up:
        direccion = (k == DERECHA) ? Right : Left;
        break;

      case Down:
        direccion = (k == DERECHA) ? Left : Right;
        break;
      case Left:
        direccion = (k == DERECHA) ? Up : Down;
        break;
      case Right:
        direccion = (k == DERECHA) ? Down : Up ;
        break;
    };
  mueve();
  dibujaSerpiente();
  frutas();
  delay(10);
  tiempo = millis(); //espera a soltar
  while ((((millis() - tiempo) < VMIN) && (mtBoard.leePulsador()) != 255)); //espera dejar pulsar
  delay(10);
}

void iniJuego() //Comienzo del juego
{

  serpiente[0].fila = serpiente[0].columna = 4;
  direccion = Right;
  movimiento.fila = 1;
  movimiento.columna = 0;
  longActual = 1;
  hayFruta = false;
  velocidad = VMAX;
}

inline void mueve() //Mueve la serpiente
{
  for (byte n = longActual; n >= 1; n--) serpiente[n] = serpiente[n - 1];
  serpiente[0].fila += (direccion == Up || direccion == Down) ? ((direccion == Up) ? 1 : -1) : 0;
  serpiente[0].columna += (direccion == Right || direccion == Left) ? ((direccion == Right) ? 1 : -1) : 0;
  if (fueraTablero() || estaEnSerpiente(serpiente[0].fila, serpiente[0].columna, 1 )) //si se sale o choca se acaba
  {
    mtBoard.drawBitmap(TRISTE);
    delay(3000);
    iniJuego();
  }
}
inline void dibujaSerpiente()//pinta la serpiente
{
  //Serial.println("Dibuja");
  mtBoard.borrar();      // clear display
  for (char n = 0; n < longActual; n++)mtBoard.drawPixel(serpiente[n].fila, serpiente[n].columna, LED_ON);
  if (hayFruta)mtBoard.drawPixel(fruta.fila, fruta.columna, LED_ON); //dibuja fruta


}
inline void frutas()//Se encarga de ir sacando las frutas
{

  if (hayFruta)
  {
    if ((serpiente[0].fila == fruta.fila) && (serpiente[0].columna == fruta.columna))
    {
      longActual++; //si se la ha comido crece la serpiente
      velocidad -= STEPVEL;
      if (longActual == LONG_MAX)hasGanado(); //Si es lo suficientemente grande se acabo el juego
      hayFruta = false;
    }

  }
  else
  {
    do {
      fruta.fila = random(8);
      fruta.columna = random(8);
    } while (estaEnSerpiente(fruta.fila, fruta.columna, 0));
    hayFruta = true;
  }

}

inline boolean fueraTablero() //true si el punto esta fuera del tablero
{
  return (serpiente[0].columna > 7) || (serpiente[0].columna < 0) || (serpiente[0].fila > 7) || (serpiente[0].fila < 0);
}

inline boolean estaEnSerpiente(char fila, char columna, char inicio) //true si el punto esta dentro de la serpiente inicio=0 incluye cabeza 1 sin cabeza
{
  for (char n = inicio; n < longActual; n++)if ((serpiente[n].fila == fila) && (serpiente[n].columna == columna))return true;
  return (false);
}

inline void hasGanado()  //Funcion cuando has ganado la  partida sonrie y saca coordenadas
{

  mtBoard.drawBitmap(ALEGRE);
  delay(3000);
  mtBoard.textoScroll("Enhorabuena las coordenadas son:");
  while (1)  mtBoard.coordenadasShow();
}
