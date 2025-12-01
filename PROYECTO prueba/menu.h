/**
 * ============================================================================
 * MENU.H - DOCUMENTACIÓN
 * ============================================================================
 * Este archivo gestiona el menú principal del juego.
 * Funciones clave:
 * 1. Inicializar el juego (crear las listas/tubos).
 * 2. Mostrar las opciones gráficas (Jugar, Ayuda, Score, Salir).
 * 3. Detectar la navegación del usuario.
 */

#include "ayuda.h" // Incluye la pantalla de instrucciones
#include "score.h" // Incluye la pantalla de ranking

// --- PROTOTIPO EXTERNO (LA SOLUCIÓN A TU PROBLEMA) ---
// Como 'dibujaPixelart' está en portada.h, le avisamos al compilador aquí
// que esa función existe para poder usarla en la opción SALIR.
void dibujaPixelart(const char *archivo, int x, int y, int filas, int columnas);

// --- PROTOTIPOS LOCALES ---
void presenta_menu(int posX, int posY);
int scroll_menu(int posX, int posY, int cantOpc);
void mainMenu();
void dibujarMarcoMenu(int x, int y, int w, int h, int color);

/**
 * Función: dibujarMarcoMenu
 * Propósito: Dibuja un borde sólido alrededor del menú usando caracteres bloque (219).
 * Parámetros: Posición (x,y), Ancho (w), Alto (h) y Color.
 */
void dibujarMarcoMenu(int x, int y, int w, int h, int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    int i, j;
    // Dibuja las líneas horizontales superior e inferior
    for(i = 0; i <= w; i++) {
        gotoxy(x + i, y); printf("%c", 219);
        gotoxy(x + i, y + h); printf("%c", 219);
    }
    // Dibuja las líneas verticales izquierda y derecha
    for(j = 0; j <= h; j++) {
        gotoxy(x, y + j); printf("%c", 219);
        gotoxy(x + w, y + j); printf("%c", 219);
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

/**
 * Función: mainMenu
 * Propósito: Es el bucle principal de la aplicación.
 * 1. Inicializa las estructuras de datos (tubos/listas).
 * 2. Muestra el menú y espera una selección.
 * 3. Redirige al usuario a la función correspondiente (Jugar, Ayuda, etc.).
 */
void mainMenu(){
    // Declaración de la matriz de punteros para los tubos (2 filas, 5 columnas)
    nodo *listas[2][5] = {NULL}; 
    
    // Configuración inicial del tablero de juego
    int ren = 2, col = 5, ancho = 8, alto = 16; 
    int op = 0; // Opción seleccionada
    int x = 45, y = 10; // Coordenadas del menú
    int i, j;
    int contador_torres = 0;

    // --- GENERACIÓN DEL NIVEL ---
    // Llenamos solo 8 torres con bolas, dejando 2 vacías para poder mover.
    for(i = 0; i < 2; i++){
        for(j = 0; j < 5; j++){
            if (contador_torres < 8) {
                crea_lista(&listas[i][j], 5); // Crea torre llena (5 bolas)
                contador_torres++;
            } else {
                listas[i][j] = NULL; // Crea torre vacía
            }
        }
    }

    // --- BUCLE DEL MENÚ ---
    do{
        presenta_menu(x, y); // Dibuja la interfaz
        op = scroll_menu(x, y, 4); // Espera a que el usuario elija
        
        system("cls"); // Limpia pantalla antes de entrar a la opción
        
        switch(op) {
            case 1: // OPCIÓN: JUGAR
                // Llamamos a las funciones del juego (listas.h)
                // Se pasan coordenadas (20,10) para centrar el tablero
                tablero(20, 10, ancho, alto, ren, col, listas);
                moverCuadro(20, 10, ancho, alto, ren, col, listas);
                break;
                
            case 2: // OPCIÓN: AYUDA
                mostrarAyuda(); // Función de ayuda.h
                break;
                
            case 3: // OPCIÓN: SCORE
                ordenarYMostrarScores(); // Función de score.h
                break;
                
            case 4: // OPCIÓN: SALIR
                
                
                // AQUÍ USAMOS LA FUNCIÓN DE PORTADA.H
                dibujaPixelart("exit.txt", 0, 0, 35, 121);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
                gotoxy(x-5, y);
                printf("Saliendo del juego..."); 
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
                gotoxy(x-10, y+2);
                printf("Creditos: Jose Manuel Nino || Andres Fernando Garcia"); 
                gotoxy(x-5, y+4);
                printf("Juego Propio - 2025");
                break;
        }
    } while(op != 4); // Repetir mientras no sea Salir
}

/**
 * Función: presenta_menu
 * Propósito: Dibuja la carcasa estética del menú.
 */
void presenta_menu(int posX, int posY){
    system("cls");
    
    // 1. Dibujar Marco Sólido Azul (Color 1)
    dibujarMarcoMenu(posX - 5, posY - 3, 35, 14, 1); 
    
    // 2. Título Amarillo
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); 
    gotoxy(posX + 5, posY - 1);
    printf("MAIN MENU");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}
 
/**
 * Función: scroll_menu
 * Propósito: Maneja la interacción del usuario.
 * - Pinta las opciones.
 * - Ilumina la opción actual.
 * - Detecta flechas y enter.
 */
int scroll_menu(int posX, int posY, int cantOpc)
{
    char tecla = '\0';
    int op = 1;
    
    // Configuración de colores para cada botón: 
    // 11=Cyan, 10=Verde, 13=Magenta, 12=Rojo
    int colores[] = {11, 10, 13, 12}; 
    char *textos[] = {"1. JUGAR    ", "2. AYUDA    ", "3. SCORE    ", "4. SALIR    "};

    do {
        // Ciclo para redibujar las opciones
        for(int i = 0; i < cantOpc; i++) {
            int currentY = posY + (i * 2) + 1; // Calcula la altura de cada línea
            
            if (op == (i + 1)) {
                // SI ESTÁ SELECCIONADA:
                // Usa el color brillante del arreglo y pone la flecha »
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colores[i]);
                gotoxy(posX, currentY);
                printf("%c %s", 175, textos[i]); // 175 = »
            } else {
                // SI NO ESTÁ SELECCIONADA:
                // Se pinta en gris oscuro (8) para dar efecto de "apagado"
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                gotoxy(posX, currentY);
                printf("  %s", textos[i]); // Espacios para limpiar la flecha vieja
            }
        }

        // Espera input del usuario
        tecla = getch();
        
        if(tecla == 80 && op < cantOpc) { // Flecha ABAJO
            op++;
        }
        if(tecla == 72 && op > 1) { // Flecha ARRIBA
            op--;
        }
        
    } while(tecla != 27 && tecla != 13); // Sale con ESC (27) o ENTER (13)
    
    // Antes de retornar, regresamos el color a blanco
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    return (op);
}
