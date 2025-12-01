// Jose Manuel Niño Ibarra, Andres Fernando Garcia Delgado, OTO 25, Programación II Dra. Roxana G. Herrera Herrera
#include<stdio.h>
#include<conio.h>
#include<windows.h>
#include<stdlib.h>
#include<time.h>

#define RIGHT 77
#define LEFT 75
#define UP 72
#define DOWN 80
#define ESC 27
#define ESPY 6
#define ENTER 13
 
#include "funciones.h"
#include "listaS.h"
#include "menu.h"
#include "portada.h"
 
int main() {
	system("MOD CON COLS=100 FILES=100");
	mainPortada();   
    srand(time(NULL));
    mainMenu();      
 
    getch();
    return 0;
}
