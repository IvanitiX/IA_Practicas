#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include "player.h"
#include "environment.h"

using namespace std;

const double masinf=9999999999.0, menosinf=-9999999999.0;


// Constructor
Player::Player(int jug){
    jugador_=jug;
}


// No modificar estos 3 métodos

// Actualiza el estado del juego para el jugador
void Player::Perceive(const Environment & env){
    actual_=env;
}

double Puntuacion(int jugador, const Environment &estado){
    double suma=0;

    for (int i=0; i<7; i++)
      for (int j=0; j<7; j++){
         if (estado.See_Casilla(i,j)==jugador){
            if (j<3)
               suma += j;
            else
               suma += (6-j);
         }
      }

    return suma;
}


// Funcion de valoracion para testear Poda Alfabeta
double ValoracionTest(const Environment &estado, int jugador){
    int ganador = estado.RevisarTablero();

    if (ganador==jugador)
       return 99999999.0; // Gana el jugador que pide la valoracion
    else if (ganador!=0)
            return -99999999.0; // Pierde el jugador que pide la valoracion
    else if (estado.Get_Casillas_Libres()==0)
            return 0;  // Hay un empate global y se ha rellenado completamente el tablero
    else
          return Puntuacion(jugador,estado);
}

// ------------------- Los tres metodos anteriores no se pueden modificar


int fichasAlrededor(int jugador, const Environment &state, int fila, int col, int &Verticales, int &Horizontales, int &Diagonales){

	Verticales=0;
    Horizontales=0;
    Diagonales=0;
    int UnidasT = 0 ;

//Verticales
	for(int hor=1; fila-hor>=0; hor++){//Columna anterior
		if(state.See_Casilla(fila-hor, col)==jugador){
			Verticales++;
            if (Verticales == 2) UnidasT += 2*Verticales ;
            else if (Verticales == 3) UnidasT += 3*Verticales ;
            else if (Verticales > 3) {UnidasT += Verticales ; Verticales = 0 ;} ;
        }
        else Verticales = 0 ;
	}

	for(int hor=1; fila+hor<7; hor++){//Columna posterior
		if(state.See_Casilla(fila+hor, col)==jugador){
			Verticales++;
            if (Verticales == 2) UnidasT += 2*Verticales ;
            else if (Verticales == 3) UnidasT += 3*Verticales ;
            else if (Verticales > 3) {UnidasT += Verticales ; Verticales = 0 ;} ;
        }
        else Verticales = 0 ;
	}

//Horizontales
	for(int vert=1; col-vert>=0; vert++){//Fila anterior
		if(state.See_Casilla(fila, col-vert)==jugador){
			Horizontales++;
            if (Horizontales == 2) UnidasT += 2*Horizontales ;
            else if (Horizontales == 3) UnidasT += 3*Horizontales ;
            else if (Horizontales > 3) {UnidasT += Horizontales ; Horizontales = 0 ;} ;
        }
        else Horizontales = 0 ;
	}

	for(int vert=1; col+vert<7; vert++){//Fila posterior
		if(state.See_Casilla(fila, col+vert)==jugador){
			Horizontales++;
            if (Horizontales == 2) UnidasT += 2*Horizontales ;
            else if (Horizontales == 3) UnidasT += 3*Horizontales ;
            else if (Horizontales > 3) {UnidasT += Horizontales ; Horizontales = 0 ;} ;
        }
        else Horizontales = 0 ;
	}

//Diagonales
	for(int dia=1; dia<4; dia++){//Diagonal Der-Arr
		if (fila+dia>=7 || col+dia>=7) break;
		if(state.See_Casilla(fila+dia, col+dia)==jugador){
			Diagonales++;
            if (Diagonales == 2) UnidasT += 2*Diagonales ;
            else if (Diagonales == 3) UnidasT += 3*Diagonales ;
            else if (Diagonales > 3) {UnidasT += Diagonales ; Diagonales = 0 ;} ;
        }
        else Diagonales = 0 ;
	}


	for(int dia=1; dia<4; ++dia){//Diagonal Der-Aba
		if(fila-dia<=-1 || col+dia>=7) break;
		if(state.See_Casilla(fila-dia, col+dia)==jugador){
			Diagonales++;
            if (Diagonales == 2) UnidasT += 2*Diagonales ;
            else if (Diagonales == 3) UnidasT += 3*Diagonales ;
            else if (Diagonales > 3) {UnidasT += Diagonales ; Diagonales = 0 ;} ;
        }
        else Diagonales = 0 ;
	}


	for(int dia=1; dia<4; ++dia){//Diagonal Izq-Arr
		if(fila+dia>=7 || col-dia<=-1) break;
		if(state.See_Casilla(fila+dia, col-dia)==jugador){
			Diagonales++;
            if (Diagonales == 2) UnidasT += 2*Diagonales ;
            else if (Diagonales == 3) UnidasT += 3*Diagonales ;
            else if (Diagonales > 3) {UnidasT += Diagonales ; Diagonales = 0 ;} ;
        }
        else Diagonales = 0 ;
	}


	for(int dia=1; dia<4; ++dia){//Diagonal Izq-Aba
		 if(fila-dia<=-1 || col-dia<=-1) break;
		 if(state.See_Casilla(fila-dia, col-dia)==jugador){
    		Diagonales++;
            if (Diagonales == 2) UnidasT += 2*Diagonales ;
            else if (Diagonales == 3) UnidasT += 3*Diagonales ;
            else if (Diagonales > 3) {UnidasT += Diagonales ; Diagonales = 0 ;} ;
        }
        else Diagonales = 0 ;
	}

    return UnidasT ;
}

/*Dadas las fichas cercanas en el estado del Juego, se hará una puntuación en base a las posibilidades de conectar 4, cosa
que queremos saber para evitar ponerlas ahí*/
double analisisEntorno(int jugador, const Environment &estado){
   double valor = 0 ;
   int verticales = 0,horizontales = 0,diagonales = 0 ;

   for(int fil = 0 ; fil+1 < 7 ; fil+=2){
      for(int col = 0 ; col+1 < 7 ; col+=2){
         valor = valor + fichasAlrededor(jugador, estado, fil, col, verticales, horizontales, diagonales)/*
                       + fichasAlrededor(jugador, estado, fil, col++, verticales, horizontales, diagonales)
                       + fichasAlrededor(jugador, estado, fil++, col, verticales, horizontales, diagonales)
                       + fichasAlrededor(jugador, estado, fil, col++, verticales, horizontales, diagonales)*/ ;
      }
   }
   return valor ;
}

// Funcion heuristica (ESTA ES LA QUE TENEIS QUE MODIFICAR)
/*Valorando la puntuación del análisis del entorno del juego,
determinar la victoria o derrota de quien lo solicite*/

double Valoracion(const Environment &estado, int jugador){
   int victoria = estado.RevisarTablero();
   double puntuacion = 0 ;
   int contrincante ;

   if(jugador == 1)
      contrincante = 2 ;
   else
      contrincante = 1 ;
   
   if(victoria == jugador)
      return masinf;
   else if (victoria == contrincante)
      return menosinf;
   else if (estado.Get_Casillas_Libres() == 0)
      return 0 ;
   else{
        puntuacion = puntuacion - analisisEntorno(jugador, estado) + analisisEntorno(contrincante, estado);
    	return puntuacion;
   }
   
}





// Esta funcion no se puede usar en la version entregable
// Aparece aqui solo para ILUSTRAR el comportamiento del juego
// ESTO NO IMPLEMENTA NI MINIMAX, NI PODA ALFABETA
void JuegoAleatorio(bool aplicables[], int opciones[], int &j){
    j=0;
    for (int i=0; i<8; i++){
        if (aplicables[i]){
           opciones[j]=i;
           j++;
        }
    }
}

//Implementación de la poda alfa-beta.

double Poda_AlfaBeta(Environment estado, int jugador, int profundidad, Environment::ActionType &accion, double alfa, double beta, int nodos){
   bool a_aplicar[8];
   double puntos = 0 ;
   int acciones = estado.possible_actions(a_aplicar) ;
   Environment::ActionType lastAct;
   if(profundidad == 0 || estado.JuegoTerminado() == true){
      return Valoracion(estado,jugador);
      nodos++;
    }
   
   int accionActual = -1 ;
   Environment siguiente = estado.GenerateNextMove(accionActual) ;
   //Las ramas pares funcionan para los máximos
   //Las impares, para los mínimos
   //Se irá iterando mientras se pueda actuar.

   if(profundidad % 2 == 0){//Alfa
   int i = 0 ; //Iterador
   while(i<estado.possible_actions(a_aplicar)){
      puntos = Poda_AlfaBeta(siguiente, jugador, profundidad-1, lastAct, alfa, beta,nodos);
      if(puntos > alfa){
         alfa = puntos ;
         accion = static_cast<Environment::ActionType> (accionActual) ;
      }
      if(alfa >= beta) break ;
      //Si no, se corta. Podaríamos así beta
      siguiente = estado.GenerateNextMove(accionActual);
      i++ ;
   }
   return alfa ;
   }

   else{//Beta
   int i = 0 ; //Iterador
   while(i<estado.possible_actions(a_aplicar)){
      puntos = Poda_AlfaBeta(siguiente, jugador, profundidad-1, lastAct, alfa, beta,nodos);
      if(puntos < beta){
         beta = puntos ;
         accion = static_cast<Environment::ActionType> (accionActual) ;
      }
      if(beta <= alfa) break ;
      //Si no, se corta. Podaríamos así alfa
      siguiente = estado.GenerateNextMove(accionActual);
      i++ ;
   }
      return beta ;
   } 
}



// Invoca el siguiente movimiento del jugador
Environment::ActionType Player::Think(){
    const int PROFUNDIDAD_MINIMAX = 6;  // Umbral maximo de profundidad para el metodo MiniMax
    const int PROFUNDIDAD_ALFABETA = 8; // Umbral maximo de profundidad para la poda Alfa_Beta

    Environment::ActionType accion; // acci�n que se va a devolver
    bool aplicables[8]; // Vector bool usado para obtener las acciones que son aplicables en el estado actual. La interpretacion es
                        // aplicables[0]==true si PUT1 es aplicable
                        // aplicables[1]==true si PUT2 es aplicable
                        // aplicables[2]==true si PUT3 es aplicable
                        // aplicables[3]==true si PUT4 es aplicable
                        // aplicables[4]==true si PUT5 es aplicable
                        // aplicables[5]==true si PUT6 es aplicable
                        // aplicables[6]==true si PUT7 es aplicable
                        // aplicables[7]==true si BOOM es aplicable



    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha, beta; // Cotas de la poda AlfaBeta

    int n_act; //Acciones posibles en el estado actual


    n_act = actual_.possible_actions(aplicables); // Obtengo las acciones aplicables al estado actual en "aplicables"
    int opciones[10];

    // Muestra por la consola las acciones aplicable para el jugador activo
    //actual_.PintaTablero();
    cout << " Acciones aplicables ";
    (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
    for (int t=0; t<8; t++)
      if (aplicables[t])
         cout << " " << actual_.ActionStr( static_cast< Environment::ActionType > (t)  );
    cout << endl;


    /*//--------------------- COMENTAR Desde aqui
    cout << "\n\t";
    int n_opciones=0;
    JuegoAleatorio(aplicables, opciones, n_opciones);

    if (n_act==0){
      (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
      cout << " No puede realizar ninguna accion!!!\n";
      //accion = Environment::actIDLE;
    }
    else if (n_act==1){
           (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
            cout << " Solo se puede realizar la accion "
                 << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[0])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[0]);

         }
         else { // Hay que elegir entre varias posibles acciones
            int aleatorio = rand()%n_opciones;
            cout << " -> " << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[aleatorio])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[aleatorio]);
         }

    //--------------------- COMENTAR Hasta aqui*/


    //--------------------- AQUI EMPIEZA LA PARTE A REALIZAR POR EL ALUMNO ------------------------------------------------


    // Opcion: Poda AlfaBeta
    // NOTA: La parametrizacion es solo orientativa
    if (valor >= 99999.0 && aplicables[7])
      accion= static_cast<Environment::ActionType> (7) ;
   else
    valor = Poda_AlfaBeta(actual_, jugador_, PROFUNDIDAD_ALFABETA, accion, menosinf, masinf, 0);
    cout << "Valor MiniMax: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;

    return accion;
}

