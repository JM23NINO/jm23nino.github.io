// Jose Manuel Niño Ibarra, Andres Fernando Garcia Delgado, OTO 25, Programación II Dra. Roxana G. Herrera Herrera
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

// Estructura auxiliar para la tabla de score
typedef struct {
    char nombre[50];
    int puntos;
    long tiempoSegundos; // Nuevo campo para el tiempo
} JugadorScore;

void ordenarYMostrarScores() {
    FILE *f = fopen("jugadores.txt", "r");
    if (f == NULL) {
        cuadro(30, 10, 40, 5, 12); 
        gotoxy(35, 12); printf("No hay registros aun.");
        getch();
        return;
    }

    JugadorScore lista[100]; 
    int total = 0;
    char linea[100];

    // 1. Cargar datos (Ahora lee 3 columnas)
    while (fgets(linea, sizeof(linea), f) && total < 100) {
        char n[50];
        int p;
        long t; // Variable para tiempo
        
        // Formato: Nombre \t Puntos \t Tiempo
        // Si el archivo es viejo (2 columnas), t se quedara en 0 o basura, 
        // asi que inicializamos t=0 por seguridad.
        t = 0;
        int leidos = sscanf(linea, "%s\t%d\t%ld", n, &p, &t);
        
        if (leidos >= 2) { // Al menos leyo nombre y puntos
            strcpy(lista[total].nombre, n);
            lista[total].puntos = p;
            lista[total].tiempoSegundos = t;
            total++;
        }
    }
    fclose(f);

    // 2. Ordenar por PUNTOS (Mayor a Menor)
    for (int i = 0; i < total - 1; i++) {
        for (int j = 0; j < total - i - 1; j++) {
            if (lista[j].puntos < lista[j + 1].puntos) {
                JugadorScore temp = lista[j];
                lista[j] = lista[j + 1];
                lista[j + 1] = temp;
            }
        }
    }

    // 3. Interfaz Gráfica
    system("cls");
    int x = 25; // Ajustado un poco a la izquierda para que quepa el tiempo
    int y = 3;
    
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    gotoxy(x + 10, y); printf("HALL OF FAME - MEJORES JUGADORES");
    
    y = 5;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11); 
    gotoxy(x, y);      printf("RANK");
    gotoxy(x + 8, y);  printf("JUGADOR");
    gotoxy(x + 30, y); printf("PUNTUACION");
    gotoxy(x + 45, y); printf("TIEMPO TOTAL"); // Nueva columna
    
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8); 
    gotoxy(x, y + 1); printf("----------------------------------------------------------");

    y = 7;
    for (int i = 0; i < total; i++) {
        if (i == 0) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); 
        else if (i == 1) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);  
        else if (i == 2) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);  
        else SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); 

        gotoxy(x, y + i);      printf("#%d", i + 1);
        gotoxy(x + 8, y + i);  printf("%s", lista[i].nombre);
        gotoxy(x + 30, y + i); printf("%d pts", lista[i].puntos);
        
        // Convertir segundos a Minutos
        int minutos = lista[i].tiempoSegundos / 60;
        gotoxy(x + 45, y + i); printf("%d min", minutos);
        
        if (i >= 14) break; 
    }

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
    gotoxy(20, 25); printf("Presiona cualquier tecla para volver...");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    getch();
}

