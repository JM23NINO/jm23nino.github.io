// Jose Manuel Niño Ibarra, Andres Fernando Garcia Delgado, OTO 25, Programación II Dra. Roxana G. Herrera Herrera
#define COLOR_TABLERO 15
#define ESP 3

void gotoxy(int x, int y);
void cuadro(int posX,int posY,int ancho,int alto, int color);

void cuadro(int posX,int posY,int ancho,int alto,int color)
{
	int i;
	SetConsoleTextAttribute(GetStdHandle (STD_OUTPUT_HANDLE),color);
	for(i=0;i<ancho;i++)
	{
		gotoxy(posX+i,posY);
		printf("%c",196);
		gotoxy(posX+i,posY+alto);
		printf("%c",196);
	}
	for(i=0;i<alto;i++)
	{
		gotoxy(posX,posY+i);
		printf("%c",179);
		gotoxy(posX+ancho,posY+i);
		printf("%c",179);
	}
	gotoxy(posX, posY);
	printf("%c",218);
	gotoxy(posX+ancho, posY);
	printf("%c",191);
	gotoxy(posX, posY+alto);
	printf("%c",192);
	gotoxy(posX+ancho, posY+alto);
	printf("%c",217);
 
}
void gotoxy(int x, int y)
{
	HANDLE hcon;
	hcon=GetStdHandle(STD_OUTPUT_HANDLE);
	COORD dwPos;
	dwPos.X= x;
	dwPos.Y= y;
	SetConsoleCursorPosition(hcon, dwPos);
}
