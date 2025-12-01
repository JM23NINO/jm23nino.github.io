

#include <stdio.h>
#include <windows.h>
#include <conio.h>

void mostrarAyuda() {
    system("cls");
    
    // --- MARCO DECORATIVO ---
    int x = 15, y = 3;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11); // Cyan
    gotoxy(x, y);     printf("========================================================");
    gotoxy(x, y + 1); printf("|             C O M O   J U G A R                      |");
    gotoxy(x, y + 2); printf("========================================================");

    // --- OBJETIVO ---
    y = 7;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo
    gotoxy(x, y); printf("1. EL OBJETIVO:");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
    gotoxy(x, y + 1); printf("   Agrupa todas las bolas del MISMO COLOR en un solo");
    gotoxy(x, y + 2); printf("   tubo hasta completar todos los colores.");

    // --- CONTROLES ---
    y = 11;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo
    gotoxy(x, y); printf("2. CONTROLES:");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
    gotoxy(x, y + 1); printf("   Usa las ");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde
    printf("FLECHAS");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    printf(" para moverte entre los tubos.");
    
    gotoxy(x, y + 2); printf("   Presiona ");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde
    printf("ENTER");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    printf(" para SELECCIONAR una bola.");
    
    gotoxy(x, y + 3); printf("   Presiona ");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde
    printf("ENTER");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    printf(" de nuevo para SOLTARLA.");

    // --- REGLAS ---
    y = 16;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo
    gotoxy(x, y); printf("3. REGLAS IMPORTANTES:");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
    
    gotoxy(x, y + 1); printf("   A) Solo puedes mover la bola superior.");
    
    gotoxy(x, y + 3); printf("   B) Solo puedes colocar una bola encima de otra");
    gotoxy(x, y + 4); printf("      si tienen el ");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta
    printf("MISMO COLOR");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    printf(".");
    
    gotoxy(x, y + 6); printf("   C) Tambien puedes mover bolas a tubos VACIOS.");
    
    gotoxy(x, y + 8); printf("   D) No puedes mover si el tubo destino esta LLENO.");

    // --- PIE DE PAGINA ---
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8); // Gris oscuro
    gotoxy(20, 28); printf("Presiona cualquier tecla para volver al menu...");
    
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Restaurar blanco
    getch(); // Pausa
}
