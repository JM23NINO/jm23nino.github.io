// Jose Manuel Niño Ibarra, Andres Fernando Garcia Delgado, OTO 25, Programación II Dra. Roxana G. Herrera Herrera
/**
 * ============================================================================
 * LISTAS.H - DOCUMENTACIÓN GENERAL
 * ============================================================================
 * Los tubos funcionan como pilas de platos. Solo puedes poner (PUSH) o 
 * quitar (POP) la bola que está hasta arriba. Usamos "Listas Enlazadas" 
 * donde cada nodo apunta al de abajo.
 * * 2. ÁRBOL BINARIO (ABB):
 * Para guardar los puntajes, usamos un árbol ordenado alfabéticamente.
 * Esto permite buscar el nombre de un jugador muy rápido para actualizar
 * sus puntos sin leer todo el archivo cada vez.
 * * 3. RENDERIZADO:
 * Dibujamos todo usando caracteres ASCII (como el 219 '¦') y coordenadas
 * X,Y en la consola.
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>       // Necesaria para el cronómetro
#include <string.h>

// --- CONFIGURACIÓN VISUAL ---
#define ALTO_N 2        // Altura visual de cada bola (2 renglones)
#define ANCHO_N 3       // Ancho visual de cada bola (3 caracteres)
#define COLOR_N 15      // Color blanco para los bordes
#define MAX_CAPACIDAD 5 // Regla: Máximo 5 bolas por tubo
#define NUM_COLORES 8   // Dificultad: 8 colores distintos

// --- ESTRUCTURAS DE DATOS ---

// Estructura INFO: Guarda qué color es la bola y dónde se dibuja
typedef struct {
    int num;  // ID del Color
    int x, y; // Coordenadas en pantalla
} info;

// Estructura NODO: Es la "caja" que contiene la bola y conecta con la siguiente
struct reg {
    info dato;       
    struct reg *sig; // Puntero a la bola de abajo
};
typedef struct reg nodo;

// Estructura ARBOL: Nodo para el ranking de jugadores
typedef struct nodoT {
    char nombre[50];    // Nombre del jugador
    int puntos;         // Partidas ganadas
    long tiempoTotal;   // Segundos totales jugados
    struct nodoT *izq;  // Rama izquierda (Nombres A-M aprox)
    struct nodoT *der;  // Rama derecha (Nombres N-Z aprox)
} nodoTree;

// --- VARIABLES GLOBALES ---
// pool_colores: Es la "baraja" de todas las bolas disponibles en el nivel
static int pool_colores[MAX_CAPACIDAD * NUM_COLORES]; 
static int pool_idx = 0; 
static int pool_initialized = 0; 

// Variables de sesión
char jugadorActual[50] = "Invitado"; 
nodoTree *raizRanking = NULL; // Raíz del árbol en memoria RAM

// Cronómetro: Guarda el momento exacto en que inició la partida
time_t inicioCronometro; 

// --- PROTOTIPOS ---
// Declaramos las funciones aquí para que el compilador sepa que existen
nodo *crea_nodoLS(info dato);
void inserta_inicioLS(nodo **cab, info dato);
void recorreLS(nodo *cab, int x, int y);
info pop_inicioLS(nodo **cab); 
int obtenerColorTope(nodo *cab);
int cuenta_nodos(nodo *cab);
void imprime_nodoLS(info dato, int x, int y);
void borra_nodoLS(info dato, int x, int y);
void crea_lista(nodo **cab, int nodos);
void tablero(int posX, int posY, int ancho, int alto, int rens, int cols, nodo *listas[2][5]);
void moverCuadro(int posX, int posY, int ancho, int alto, int ren, int col, nodo *listas[2][5]);
int contarTorresCompletas(int rens, int cols, nodo *listas[2][5]);
void inicializar_pool_colores();

// Funciones del Sistema (Ranking, Archivos, interfaz)
void cargarJugadorActual();
void cargarRankingEnArbol();
void guardarRankingDesdeArbol();
void actualizarDatosJugador(char *nombre, int puntosSumar, long segundosSumar); 
void mostrarHUD(int rens, int cols, nodo *listas[2][5]);
void reiniciarNivel(nodo *listas[2][5]);
void dibujarMarcoBloque(int x, int y, int ancho, int alto, int color);

// Funciones Recursivas (Auxiliares para el Árbol)
nodoTree* insertarABB(nodoTree* r, char* n, int p, long t);
void guardarEnArchivoRecursivo(nodoTree* r, FILE* f);
void recorridoMejores(nodoTree* r, nodoTree** t1, nodoTree** t2, nodoTree** t3);


// ============================================================================
//     SECCIÓN 1: PERSISTENCIA Y RANKING (Manejo de Archivos y Árboles)
// ============================================================================

/**
 * Función: cargarJugadorActual
 * Descripción: Lee 'jugadores.txt' y obtiene el último nombre registrado.
 * Uso: Permite saber quién se logueó en la portada para asignarle los puntos.
 */
void cargarJugadorActual() {
    FILE *f = fopen("jugadores.txt", "r");
    if (!f) return;
    char linea[100];
    char ultimoNombre[50] = "";
    while (fgets(linea, sizeof(linea), f)) {
        char tempNombre[50];
        if (sscanf(linea, "%s", tempNombre) >= 1) { 
             strcpy(ultimoNombre, tempNombre);
        }
    }
    fclose(f);
    if (strlen(ultimoNombre) > 0) strcpy(jugadorActual, ultimoNombre);
}

/**
 * Función: insertarABB
 * Descripción: Mete un jugador al Árbol Binario.
 * Lógica: Compara nombres alfabéticamente. Si es menor va a la izquierda, si es mayor a la derecha.
 * Si el nombre ya existe, solo actualiza sus puntos y tiempo.
 */
nodoTree* insertarABB(nodoTree* r, char* n, int p, long t) {
	
    if (r == NULL) {     // Encontró un espacio vacío, crea el nuevo nodo
        nodoTree* nuevo = (nodoTree*)malloc(sizeof(nodoTree));
        strcpy(nuevo->nombre, n);
        nuevo->puntos = p;
        nuevo->tiempoTotal = t;
        nuevo->izq = nuevo->der = NULL;
        return nuevo;
    }
    
    int cmp = strcmp(n, r->nombre);
    if (cmp < 0) r->izq = insertarABB(r->izq, n, p, t);
    else if (cmp > 0) r->der = insertarABB(r->der, n, p, t);
    else { // El jugador ya existe, actualizamos datos
        r->puntos = p; 
        r->tiempoTotal = t;
    }
    return r;
}

/**
 * Función: cargarRankingEnArbol
 * Descripción: Pasa los datos del disco duro (TXT) a la memoria RAM (Árbol).
 * Importante: Lee las 3 columnas: Nombre, Puntos y Tiempo.
 */
void cargarRankingEnArbol() {
    raizRanking = NULL; 
    FILE *f = fopen("jugadores.txt", "r");
    if (!f) return;
    char nombre[50];
    int puntos;
    long tiempo;
    char linea[100];

    while (fgets(linea, sizeof(linea), f)) {
        strcpy(nombre, "");
        puntos = 0;
        tiempo = 0;
        if (sscanf(linea, "%s\t%d\t%ld", nombre, &puntos, &tiempo) >= 1) {
            raizRanking = insertarABB(raizRanking, nombre, puntos, tiempo);
        }
    }
    fclose(f);
}

/**
 * Función: guardarEnArchivoRecursivo / guardarRankingDesdeArbol
 * Descripción: Guarda el estado actual del árbol en el archivo de texto.
 * Lógica: Recorrido "In-Order" (Izquierda -> Raíz -> Derecha).
 */
void guardarEnArchivoRecursivo(nodoTree* r, FILE* f) {
    if (r != NULL) {
        guardarEnArchivoRecursivo(r->izq, f);
        fprintf(f, "%s\t%d\t%ld\n", r->nombre, r->puntos, r->tiempoTotal);
        guardarEnArchivoRecursivo(r->der, f);
    }
}

void guardarRankingDesdeArbol() {
    FILE *f = fopen("jugadores.txt", "w"); 
    if (f) {
        guardarEnArchivoRecursivo(raizRanking, f);
        fclose(f);
    }
}

/**
 * Función: actualizarDatosJugador
 * Descripción: Es la función pública para sumar puntos.
 * Pasos:
 * 1. Busca al jugador en el árbol.
 * 2. Suma los puntos ganados y los segundos jugados.
 * 3. Si no existe, lo crea.
 * 4. Guarda todo en el archivo inmediatamente.
 */
void actualizarDatosJugador(char *nombre, int puntosSumar, long segundosSumar) {
    nodoTree *temp = raizRanking;
    int encontrado = 0;
    while (temp != NULL) {
        int cmp = strcmp(nombre, temp->nombre);
        if (cmp == 0) {
            temp->puntos += puntosSumar;      
            temp->tiempoTotal += segundosSumar; 
            encontrado = 1;
            break;
        } else if (cmp < 0) temp = temp->izq;
        else temp = temp->der;
    }
    if (!encontrado) raizRanking = insertarABB(raizRanking, nombre, puntosSumar, segundosSumar);
    guardarRankingDesdeArbol(); 
}

/**
 * Función: recorridoMejores
 * Descripción: Encuentra el TOP 3 de mejores puntajes recorriendo todo el árbol.
 * Nota: Como el árbol está ordenado por Nombre, tenemos que revisar todos los nodos
 * para saber quién tiene más puntos.
 */
void recorridoMejores(nodoTree* r, nodoTree** t1, nodoTree** t2, nodoTree** t3) {
    if (!r) return;
    // Lógica de podio (Desplazamiento: Si encuentro uno mejor que el 1ro, recorro los demás)
    if (*t1 == NULL || r->puntos > (*t1)->puntos) {
        *t3 = *t2; *t2 = *t1; *t1 = r;
    } else if ((*t2 == NULL || r->puntos > (*t2)->puntos) && r != *t1) {
        *t3 = *t2; *t2 = r;
    } else if ((*t3 == NULL || r->puntos > (*t3)->puntos) && r != *t1 && r != *t2) {
        *t3 = r;
    }
    recorridoMejores(r->izq, t1, t2, t3);
    recorridoMejores(r->der, t1, t2, t3);
}


// ============================================================================
//     SECCIÓN 2: LÓGICA DEL JUEGO (MANEJO DE PILAS)
// ============================================================================

// Crea un nuevo nodo en memoria
nodo *crea_nodoLS(info dato) {
    nodo *nuevo = (nodo*)malloc(sizeof(nodo));
    nuevo->dato = dato;
    nuevo->dato.x = -1; nuevo->dato.y = -1;
    nuevo->sig = NULL;
    return nuevo;
}

// PUSH: Inserta una bola en el tope de la pila (tubo)
void inserta_inicioLS(nodo **cab, info dato) {
    nodo *nuevo = crea_nodoLS(dato);
    nuevo->sig = *cab;
    *cab = nuevo;
}

// POP: Extrae y devuelve la bola del tope de la pila
info pop_inicioLS(nodo **cab) {
    info dato = {0, 0, 0};
    if (*cab != NULL) {
        nodo *temp = *cab;
        dato = temp->dato;
        *cab = temp->sig;
        free(temp);
    }
    return dato;
}

// PEEK: Mira el color de la bola superior sin sacarla (para validar si se puede poner otra encima)
int obtenerColorTope(nodo *cab) {
    if (cab == NULL) return -1; 
    return cab->dato.num;
}

// Cuenta cuántas bolas hay en un tubo
int cuenta_nodos(nodo *cab){
    nodo *aux = cab;
    int cont = 0;
    while(aux){ aux = aux->sig; cont++; }
    return cont;
}


// ============================================================================
//     SECCIÓN 3: RENDERIZADO Y VISUALES
// ============================================================================

/**
 * Función: dibujarMarcoBloque
 * Descripción: Dibuja un rectángulo usando el carácter sólido '¦' (ASCII 219).
 * Uso: Para los marcos estéticos del HUD.
 */
void dibujarMarcoBloque(int x, int y, int ancho, int alto, int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    int i, j;
    // Bordes Superior e Inferior
    for(i = 0; i <= ancho; i++) {
        gotoxy(x + i, y); printf("%c", 219);
        gotoxy(x + i, y + alto); printf("%c", 219);
    }
    // Bordes Laterales
    for(j = 0; j <= alto; j++) {
        gotoxy(x, y + j); printf("%c", 219);
        gotoxy(x + ancho, y + j); printf("%c", 219);
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); 
}

// Dibuja una bola individual
void imprime_nodoLS(info dato, int x, int y) {
    cuadro(x, y, ANCHO_N, ALTO_N, COLOR_N); // Borde
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), dato.num);
    gotoxy(x + 1, y + 1);
    printf("%c%c", 219, 219); // Relleno
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

// Borra una bola de la pantalla (pinta negro)
void borra_nodoLS(info dato, int x, int y) {
    cuadro(x, y, ANCHO_N, ALTO_N, 0); 
    gotoxy(x + 1, y + 1);
    printf("  "); 
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

/**
 * Función: recorreLS
 * Descripción: Recorre la lista enlazada y dibuja todas las bolas de un tubo.
 * Lógica Visual: Calcula la 'gravedad'. Si hay pocas bolas, ajusta la Y para
 * que se dibujen en el fondo del tubo, no flotando arriba.
 */
void recorreLS(nodo *cab, int x, int y) {
    nodo *aux = NULL;
    int cantidad = cuenta_nodos(cab);
    int actualY = y + ((MAX_CAPACIDAD - cantidad) * 3); 

    for (aux = cab; aux != NULL; aux = aux->sig) {
        aux->dato.x = x; aux->dato.y = actualY;
        imprime_nodoLS(aux->dato, x, actualY);
        actualY += 3; 
    }
}

/**
 * Función: inicializar_pool_colores
 * Descripción: Crea la "Baraja" de colores.
 * Importante: Asegura que haya pares exactos (5 de cada color) para que el juego sea ganable.
 * Luego mezcla el arreglo aleatoriamente.
 */
void inicializar_pool_colores() {
    int i, j, k = 0;
    int paleta[] = {2, 3, 4, 5, 6, 7, 8, 9}; 
    for (i = 0; i < NUM_COLORES; i++) {
        for (j = 0; j < MAX_CAPACIDAD; j++) pool_colores[k++] = paleta[i]; 
    }
    srand(time(NULL));
    for (i = (MAX_CAPACIDAD * NUM_COLORES) - 1; i > 0; i--) {
        int r = rand() % (i + 1);
        int temp = pool_colores[i];
        pool_colores[i] = pool_colores[r];
        pool_colores[r] = temp;
    }
    pool_idx = 0;
    pool_initialized = 1;
}

// Saca bolas de la baraja y las mete en un tubo
void crea_lista(nodo **cab, int nodos) {
    if (!pool_initialized) inicializar_pool_colores();
    info dato;
    for (int i = 0; i < nodos; i++) {
        if (pool_idx < (MAX_CAPACIDAD * NUM_COLORES)) {
            dato.num = pool_colores[pool_idx++];
            inserta_inicioLS(cab, dato);
        }
    }
}

// Dibuja el tablero completo (todos los tubos vacíos + las bolas dentro)
void tablero(int posX, int posY, int ancho, int alto, int rens, int cols, nodo *listas[2][5]) {
    int i, j, k;
    int x, y;
    int sep_h = 4, sep_v = 6; 

    for (i = 0, y = posY; i < rens; i++, y = y + alto + sep_v) {
        for (j = 0, x = posX; j < cols; j++, x = x + ancho + sep_h) {
            cuadro(x, y, ancho, alto, COLOR_TABLERO);
            // Limpia residuos gráficos dentro del tubo
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0);
            for(k = 1; k < alto - 1; k++) {
                gotoxy(x + 1, y + k);
                printf("       "); 
            }
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
            recorreLS(listas[i][j], x + 2, y + 1); 
        }
    }
}

/**
 * Función: contarTorresCompletas
 * Descripción: Verifica la condición de victoria.
 * Regla: Un tubo cuenta si está LLENO (5 bolas) y TODAS son del mismo color.
 */
int contarTorresCompletas(int rens, int cols, nodo *listas[2][5]) {
    int completas = 0;
    for (int i = 0; i < rens; i++) {
        for (int j = 0; j < cols; j++) {
            nodo *aux = listas[i][j];
            if (aux == NULL) continue; 
            int color = aux->dato.num;
            int count = 0;
            int uniforme = 1; 
            while (aux != NULL) {
                if (aux->dato.num != color) uniforme = 0; 
                aux = aux->sig;
                count++;
            }
            if (count == MAX_CAPACIDAD && uniforme) completas++;
        }
    }
    return completas;
}

/**
 * Función: mostrarHUD
 * Descripción: Dibuja el panel lateral derecho.
 * Componentes: 
 * 1. Nombre del jugador y barra de progreso.
 * 2. Top 3 líderes (con tiempo).
 * 3. Botón visual de reinicio.
 * Usa 'dibujarMarcoBloque' para un estilo sólido.
 */
void mostrarHUD(int rens, int cols, nodo *listas[2][5]) {
    int completas = contarTorresCompletas(rens, cols, listas);
    int x = 85; 
    int y = 5;
    int ancho_panel = 32;

    // --- PANEL 1: JUGADOR Y PROGRESO ---
    dibujarMarcoBloque(x, y, ancho_panel, 7, 11); // Marco Cyan Sólido
    gotoxy(x + 2, y + 1); 
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    printf("JUGADOR ACTUAL:");
    gotoxy(x + 2, y + 2); 
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo
    printf("%-20s", jugadorActual);
    
    gotoxy(x + 2, y + 4); 
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    printf("TORRES COMPLETADAS:");
    
    gotoxy(x + 2, y + 5);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde
    // Barra de progreso visual
    printf("[");
    for(int k=0; k<NUM_COLORES; k++) {
        if(k < completas) printf("%c", 219); 
        else printf(" ");
    }
    printf("] %d / %d", completas, NUM_COLORES);

    // --- PANEL 2: TOP 3 LÍDERES ---
    y += 9; 
    dibujarMarcoBloque(x, y, ancho_panel, 10, 13); // Marco Magenta Sólido
    gotoxy(x + 9, y + 1);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    printf("TOP 3 - LIDERES");
    gotoxy(x + 1, y + 2);
    printf("------------------------------"); 

    gotoxy(x + 2, y + 3);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8); 
    printf("NOM         PTS   TIEMPO");

    nodoTree *t1 = NULL, *t2 = NULL, *t3 = NULL;
    recorridoMejores(raizRanking, &t1, &t2, &t3);
    
    int m, s;

    // Render Top 1
    gotoxy(x + 2, y + 5); SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Oro
    if (t1) {
        m = t1->tiempoTotal / 60; s = t1->tiempoTotal % 60;
        printf("1.%-10s %3d   %02d:%02d", t1->nombre, t1->puntos, m, s);
    } else printf("1. ---");

    // Render Top 2
    gotoxy(x + 2, y + 6); SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); // Plata
    if (t2) {
        m = t2->tiempoTotal / 60; s = t2->tiempoTotal % 60;
        printf("2.%-10s %3d   %02d:%02d", t2->nombre, t2->puntos, m, s);
    } else printf("2. ---");

    // Render Top 3
    gotoxy(x + 2, y + 7); SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6); // Bronce
    if (t3) {
        m = t3->tiempoTotal / 60; s = t3->tiempoTotal % 60;
        printf("3.%-10s %3d   %02d:%02d", t3->nombre, t3->puntos, m, s);
    } else printf("3. ---");

    // --- PANEL 3: BOTÓN REINICIAR ---
    y += 12;
    dibujarMarcoBloque(x, y, ancho_panel, 4, 12); // Marco Rojo Sólido
    gotoxy(x + 6, y + 2);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); 
    printf("[ R ] REINICIAR NIVEL");
    
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

// Resetea el nivel: vacía listas, genera nueva mezcla, reinicia tiempo
void reiniciarNivel(nodo *listas[2][5]) {
    for(int i=0; i<2; i++) {
        for(int j=0; j<5; j++) {
            while(listas[i][j] != NULL) pop_inicioLS(&listas[i][j]);
        }
    }
    pool_initialized = 0; 
    int contador = 0;
    for(int i=0; i<2; i++){
        for(int j=0; j<5; j++){
            if (contador < 8) {
                crea_lista(&listas[i][j], MAX_CAPACIDAD);
                contador++;
            } else {
                listas[i][j] = NULL;
            }
        }
    }
    // Reiniciamos el cronómetro
    inicioCronometro = time(NULL);
}


// ============================================================================
//     SECCIÓN 4: BUCLE PRINCIPAL (CONTROL Y CRONÓMETRO)
// ============================================================================

/**
 * Función: moverCuadro
 * Descripción: Es el ciclo "Game Loop".
 * Maneja:
 * - Detectar teclas (Flechas, Enter, R, ESC).
 * - Lógica de agarrar (PICK) y soltar (DROP) bolas.
 * - Lógica de movimiento múltiple (Bloques de color).
 * - Detección de victoria y guardado de datos.
 */
void moverCuadro(int posX, int posY, int ancho, int alto, int ren, int col, nodo *listas[2][5]) {
    char tecla = 0;
    int x = posX, y = posY;
    int i = 0, j = 0; 
    int sep_h = 4, sep_v = 6;
    int selI = -1, selJ = -1, haySeleccion = 0; 
    info bolaSeleccionada;

    cargarJugadorActual();      
    cargarRankingEnArbol();     

    inicioCronometro = time(NULL); // Arranca el reloj

    tablero(posX, posY, ancho, alto, ren, col, listas);
    mostrarHUD(ren, col, listas);
    cuadro(x, y, ancho, alto, 10); 

    do {
        if(kbhit()) { // Si se presiona tecla...
            tecla = getch();
            cuadro(x, y, ancho, alto, 15); 

            switch (tecla) {
                // Movimiento del Cursor
                case 77: if (j < col - 1) { x += ancho + sep_h; j++; } break; 
                case 75: if (j > 0) { x -= ancho + sep_h; j--; } break;       
                case 72: if (i > 0) { y -= alto + sep_v; i--; } break;        
                case 80: if (i < ren - 1) { y += alto + sep_v; i++; } break;
                
                // Reinicio (Tecla R)
                case 'r':
                case 'R':
                    if (!haySeleccion) { 
                        reiniciarNivel(listas); 
                        system("cls");
                        tablero(posX, posY, ancho, alto, ren, col, listas);
                        mostrarHUD(ren, col, listas);
                        i = 0; j = 0; x = posX; y = posY;
                    }
                    break;

                // Selección / Acción (ENTER)
                case 13: 
                    if (!haySeleccion) {
                        // ACCIÓN: AGARRAR (PICK)
                        if (listas[i][j] != NULL) {
                            haySeleccion = 1; selI = i; selJ = j;
                            bolaSeleccionada = listas[i][j]->dato; 
                            imprime_nodoLS(bolaSeleccionada, x+2, y - 3); 
                            Beep(400, 50);
                        }
                    } else {
                        // ACCIÓN: SOLTAR (DROP)
                        int nodosDestino = cuenta_nodos(listas[i][j]);
                        int colorTope = obtenerColorTope(listas[i][j]);
                        int exito = 0;
                        
                        if (selI == i && selJ == j) { haySeleccion = 0; } // Cancelar
                        else if (nodosDestino < MAX_CAPACIDAD) {
                            // Validar: Tubo vacío O mismo color
                            if (listas[i][j] == NULL || colorTope == bolaSeleccionada.num) {
                                pop_inicioLS(&listas[selI][selJ]); 
                                inserta_inicioLS(&listas[i][j], bolaSeleccionada); 
                                exito = 1; haySeleccion = 0;
                                Beep(600, 50);
                                
                                // --- MOVIMIENTO MÚLTIPLE (BLOQUE) ---
                                // Si hay más bolas del mismo color en el origen y caben en el destino,
                                // las movemos automáticamente en bucle.
                                while (listas[selI][selJ] != NULL && 
                                       listas[selI][selJ]->dato.num == bolaSeleccionada.num &&
                                       cuenta_nodos(listas[i][j]) < MAX_CAPACIDAD) {
                                    
                                    info bolaExtra = pop_inicioLS(&listas[selI][selJ]);
                                    inserta_inicioLS(&listas[i][j], bolaExtra);
                                }
                                
                            } else Beep(200, 100);
                        } else Beep(200, 100);

                        if (!haySeleccion) {
                            // Redibujar tablero tras movimiento
                            int xOrig = posX + (selJ * (ancho + sep_h));
                            int yOrig = posY + (selI * (alto + sep_v));
                            borra_nodoLS(bolaSeleccionada, xOrig + 2, yOrig - 3); 
                            
                            tablero(posX, posY, ancho, alto, ren, col, listas);
                            mostrarHUD(ren, col, listas);
                            
                            // Verificar Victoria
                            if (exito && contarTorresCompletas(ren, col, listas) == NUM_COLORES) {
                                // Calcular tiempo final
                                time_t fin = time(NULL);
                                long segundosJugados = (long)(fin - inicioCronometro);

                                system("cls");
                                gotoxy(posX + 15, posY + 5);
                                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
                                printf("!!! NIVEL COMPLETADO !!!");
                                gotoxy(posX + 10, posY + 6);
                                printf("Tiempo: %ld segundos", segundosJugados);
                                gotoxy(posX + 10, posY + 8);
                                printf("Sumando punto a: %s...", jugadorActual);
                                Sleep(2500);
                                
                                // Guardar victoria y tiempo
                                actualizarDatosJugador(jugadorActual, 1, segundosJugados);
                                reiniciarNivel(listas);
                                
                                system("cls");
                                tablero(posX, posY, ancho, alto, ren, col, listas);
                                mostrarHUD(ren, col, listas); 
                                i=0; j=0; x=posX; y=posY;
                            }
                        }
                    }
                    break;
            }
            cuadro(x, y, ancho, alto, haySeleccion ? 14 : 10); 
        }
    } while (tecla != 27); 
    
    // Guardar tiempo jugado aunque se salga con ESC (sin puntos)
    time_t finSesion = time(NULL);
    long segundosSesion = (long)(finSesion - inicioCronometro);
    actualizarDatosJugador(jugadorActual, 0, segundosSesion);
    
    pool_initialized = 0;
}
