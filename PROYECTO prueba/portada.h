// Librerías base del sistema
#include <windows.h> // Para colores y posiciones (gotoxy)
#include <stdio.h>   // Para leer/escribir archivos y textos
#include <conio.h>   // Para detectar teclas (getch)
#include <stdlib.h>  // Para funciones estándar
#include <string.h>  // Para manejar cadenas de texto (nombres)

// --- PROTOTIPOS DE FUNCIONES ---
// Declaramos las funciones aquí para que el código sepa que existen antes de usarlas abajo
void presenta_menu_centrado(int *posX, int *posY);
int scroll_menu_centrado(int posX, int posY, int cantOpc);
void mainPortada();
void dibujaPixelart(const char *archivo, int x, int y, int filas, int columnas);
void dibujarCuadroDetalle(int x, int y, int ancho, int alto, int color); 

/**
 * Función: dibujarCuadroDetalle
 * Propósito: Dibuja un rectángulo elegante en la pantalla usando caracteres ASCII dobles.
 * Parámetros:
 * - x, y: Coordenadas de la esquina superior izquierda.
 * - ancho, alto: Tamaño del cuadro.
 * - color: Código de color para el borde.
 */
void dibujarCuadroDetalle(int x, int y, int ancho, int alto, int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    int i;
    // Dibujamos las esquinas especiales (+, +, +, +)
    gotoxy(x, y); printf("%c", 201); 
    gotoxy(x + ancho, y); printf("%c", 187); 
    gotoxy(x, y + alto); printf("%c", 200); 
    gotoxy(x + ancho, y + alto); printf("%c", 188); 
    
    // Dibujamos líneas horizontales (-)
    for(i = 1; i < ancho; i++) {
        gotoxy(x + i, y); printf("%c", 205); 
        gotoxy(x + i, y + alto); printf("%c", 205); 
    }
    
    // Dibujamos líneas verticales (¦)
    for(i = 1; i < alto; i++) {
        gotoxy(x, y + i); printf("%c", 186); 
        gotoxy(x + ancho, y + i); printf("%c", 186); 
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Reset color a blanco
}

/**
 * Función: mainPortada
 * Propósito: Es la pantalla principal de bienvenida.
 * 1. Muestra el título y el logo (pixelart).
 * 2. Pide el nombre al usuario.
 * 3. Guarda el nombre en 'jugadores.txt' inicializando sus puntos y tiempo en 0.
 */
void mainPortada() {
    char nombreJugador[50]; // Variable para guardar el texto que escriba el usuario
    
    system("cls"); // Limpia la pantalla
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    // Encabezado
    gotoxy(45, 2);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11); // Cyan brillante
    printf("--- SORT BALL || ACOMODAR BOLAS ---");
    
    // Llamada para pintar el dibujo desde el archivo de texto
    dibujaPixelart("unshowmas2.txt", 2, 2, 100, 111);
    
    // Configuración de la posición del formulario de registro
    int input_x = 70;
    int input_y = 10;

    // Etiquetas de texto (Labels)
    gotoxy(input_x, input_y);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo
    printf("REGISTRO DE JUGADOR :D");
    
    // Créditos en pantalla
    gotoxy(input_x, input_y + 8);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    printf("Nino Jose Manuel ");
    
    gotoxy(input_x, input_y + 10);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    printf("Andres Fernando Garcia Delgado");
    
    gotoxy(input_x, input_y + 12);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    printf("Tercer Parcial || Proyecto Final ");
    
    // Prompt para el usuario
    gotoxy(input_x, input_y + 2);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); 
    printf("Ingrese su Nombre: ");
    
    // Lectura de datos
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11); // Texto Cyan al escribir
    scanf("%49s", nombreJugador); // Captura lo que escribe el usuario
    
    // Sistema de Archivos: Guardar el nuevo jugador
    FILE *archivo = fopen("jugadores.txt", "a"); // "a" = append (agregar al final sin borrar lo anterior)
    if (archivo != NULL) {
        // Escribe: NOMBRE [TAB] 0 Puntos [TAB] 0 Tiempo
        fprintf(archivo, "%s\t0\t0\n", nombreJugador); 
        fclose(archivo); // Importante cerrar el archivo para guardar cambios
        
        // Mensaje de éxito
        gotoxy(input_x, input_y + 4);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11); 
        printf("Vamos a jugar :P"); 
    } else {
        // Manejo de errores si no se puede abrir el archivo
        gotoxy(input_x, input_y + 4);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
        printf("Error al guardar datos.");
    }

    Sleep(1000); // Espera 1 segundo antes de cambiar de pantalla
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    system("cls");
}
 
/**
 * Función: dibujaPixelart
 * Propósito: Lee un archivo de texto lleno de números (códigos de color) y pinta píxeles en pantalla.
 * Parámetros:
 * - archivo: Nombre del archivo .txt a leer.
 * - x, y: Posición inicial.
 * - filas, columnas: Dimensiones del dibujo.
 */
void dibujaPixelart(const char *archivo, int x, int y, int filas, int columnas) {
    FILE *f = fopen(archivo, "r"); // Abre en modo lectura ("r")
    if (f == NULL) return; // Si no existe el dibujo, no hace nada y evita crasheos
   
    int color;
    // Doble bucle para recorrer la matriz de píxeles
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            // Lee el siguiente número del archivo
            if (fscanf(f, "%d", &color) == 1) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color); // Cambia el color del lápiz
                gotoxy(x + j, y + i); // Se mueve a la posición
                printf("%c", 219); // Pinta un bloque sólido (¦)
            }
        }
    }
    fclose(f);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Restaura color
}
 
// --- FUNCIONES AUXILIARES DE MENÚ (Estilo alternativo) ---

/**
 * Función: presenta_menu_centrado
 * Propósito: Dibuja un menú estético en el centro de la pantalla.
 * Nota: Esta función es auxiliar para portadas o submenús centrados.
 */
void presenta_menu_centrado(int *posX, int *posY) {
    system("cls");
    int centroX = 60;
    int centroY = 12;
    
    int ancho_menu = 30;
    int alto_menu = 8;
    int marcoX = centroX - (ancho_menu / 2);
    int marcoY = centroY - 2;

    // Dibuja el marco decorativo
    dibujarCuadroDetalle(marcoX, marcoY, ancho_menu, alto_menu, 3); // 3 = Cyan Oscuro

    // Título
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo
    gotoxy(centroX - 4, marcoY - 1); 
    printf("[ M E N U ]");
 
    // Opciones
    int opcionesX = marcoX + 5; 
    int opcionesY = marcoY + 2;
 
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); 
    gotoxy(opcionesX, opcionesY);
    printf("1. JUGAR");
    gotoxy(opcionesX, opcionesY + 1);
    printf("2. COMO JUGAR");
    gotoxy(opcionesX, opcionesY + 2);
    printf("3. SCORE");
    gotoxy(opcionesX, opcionesY + 3);
    printf("4. SALIR");
    
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8); // Gris
    gotoxy(marcoX + 2, marcoY + alto_menu + 1);
    printf("Use flechas y ENTER");
 
    // Devuelve las coordenadas calculadas
    *posX = opcionesX;
    *posY = opcionesY;
}
 
/**
 * Función: scroll_menu_centrado
 * Propósito: Controla la navegación del menú (Flechas Arriba/Abajo).
 * Retorna: El número de la opción seleccionada.
 */
int scroll_menu_centrado(int posX, int posY, int cantOpc) {
    char tecla = '\0';
    int op = 1, y = posY;
    posX = posX - 2; // Ajuste para dibujar la flecha a la izquierda
    
    // Dibuja la flecha inicial
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11); 
    gotoxy(posX, posY);
    printf("%c", 175); // Carácter de flecha »
 
    do {
        tecla = getch(); // Espera una tecla
        
        // Borra la flecha de la posición anterior
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0); // Negro
        gotoxy(posX, y);
        printf("%c", 175);
 
        // Calcula nueva posición
        if (tecla == 80 && op < cantOpc) { // Tecla ABAJO (código 80)
            y++;
            op++;
        }
        if (tecla == 72 && op > 1) { // Tecla ARRIBA (código 72)
            y--;
            op--;
        }
 
        // Dibuja la flecha en la nueva posición
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
        gotoxy(posX, y);
        printf("%c", 175);
        
    } while (tecla != 27 && tecla != 13); // Repite hasta ENTER (13) o ESC (27)
 
    return op;
}
