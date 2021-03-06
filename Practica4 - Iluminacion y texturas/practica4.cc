//**************************************************************************
// Práctica 4
//
// Jose Antonio Ruiz Millan 2017-2018
//
// Iluminacion
//**************************************************************************


//**************************************************************************
// Objetos:
//
// '1' --> Tetraedro
// '2' --> Cubo
// '3' --> Piramide
// '4' --> Rombo
// '5' --> Ply
// '6' --> Revolucion
// '7' --> Barrido
// '8' --> Cubo de Rubik
// '9' --> Textura
//
// Formas de dibujo:
//
// 'f' --> Solido
// 'm' --> Multiples colores aleatorios
// 'n' --> Lineas
// 'p' --> Puntos
// 'c' --> Ajedrez
// 'a' --> Rubik
//
//**************************************************************************


//**************************************************************************
// Modos de Uso para el Usuario:
//
// Para mover el cubo podemos usas las siguientes teclas:
// '<' --> Cambia el sentido del movimiento
// 'k' --> Mueve la cara 1 (azul)
// 'l' --> Mueve cara 2 (naranja)
// 'i' --> Mueve cara 3 (verde)
// 'j' --> Mueve cara 4 (roja)
// 'o' --> Mueve cara 5 (amarilla)
// 'u' --> Mueve cara 6 (blanca)
// 'd' --> Realiza x movimientos aleatorios para descolocar el cubo
// 'r' --> Resuelve el cubo automaticamente
//
//**************************************************************************

//"**************************************************************************
// Iluminacion:"															
// 'e' --> Activar/desactivar las luces."									
// 's' --> Pintar en modo suavizado"											
// 'x' --> Pintar en modo NO suavizado"											
// 'g' --> Rotar luz a izquierdas"											
// 'h' --> Rotar luz a derechas"										 
// 'b' --> Rotar luz hacia abajo"											
// 'y' --> Rotar luz hacia arriba"											
// 'F1' --> Apagar/encender luz 2"											
// 'F2' --> Material oro"													
// 'F3' --> Material turquesa"												
// 'F4' --> Material ruby"													
//**************************************************************************

//"**************************************************************************
// Textura:"															
//'F7' --> Traslada la textura hacia la izquierda
//'F8' --> Traslada la textura hacia la derecha																					
//**************************************************************************

#include "stdlib.h"
#include "stdio.h"
#include <GL/glut.h>
#include <ctype.h>
#include "objetos.h"
#include <thread>
#include <chrono>
#include "CImg.h"

using namespace cimg_library;

// tamaño de los ejes
//const int AXIS_SIZE=5000;
int dibujo = -1;
int z = 3, w = 3, w2 = 3, a = 6;
char *archivo;
bool LUCES = false,luz2 = true, material1 = false, material2 = false, material3 = false;
_modo modo = LINEAS;
vector<_triangulos3D> objetos;
_cuborubik l(1.5);
_luz luz;
GLuint textura_id;
float k = 0.0;


// variables que definen la posicion de la camara en coordenadas polares
GLfloat Observer_distance;
GLfloat Observer_angle_x;
GLfloat Observer_angle_y;

// variables que controlan la ventana y la transformacion de perspectiva
GLfloat Window_width,Window_height,Front_plane,Back_plane;

// variables que determninan la posicion y tamaño de la ventana X
int UI_window_pos_x=50,UI_window_pos_y=50,UI_window_width=500,UI_window_height=500;

//**************************************************************************
//
//***************************************************************************

void clear_window()
{

glClearDepth(1.0);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}


//**************************************************************************
// Funcion para definir la transformación de proyeccion
//***************************************************************************

void change_projection()
{

glMatrixMode(GL_PROJECTION);
glLoadIdentity();

// formato(x_minimo,x_maximo, y_minimo, y_maximo,Front_plane, plano_traser)
//  Front_plane>0  Back_plane>PlanoDelantero)
glFrustum(-Window_width,Window_width,-Window_height,Window_height,Front_plane,Back_plane);
}

//**************************************************************************
// Funcion para definir la transformación de vista (posicionar la camara)
//***************************************************************************

void change_observer()
{

// posicion del observador
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
glTranslatef(0,0,-Observer_distance);
glRotatef(Observer_angle_x,1,0,0);
glRotatef(Observer_angle_y,0,1,0);
}

//**************************************************************************
// Funcion que dibuja los ejes utilizando la primitiva grafica de lineas
//***************************************************************************

void draw_axis()
{
glBegin(GL_LINES);
// eje X, color rojo
glColor3f(1,0,0);
glVertex3f(-AXIS_SIZE,0,0);
glVertex3f(AXIS_SIZE,0,0);
// eje Y, color verde
glColor3f(0,1,0);
glVertex3f(0,-AXIS_SIZE,0);
glVertex3f(0,AXIS_SIZE,0);
// eje Z, color azul
glColor3f(0,0,1);
glVertex3f(0,0,-AXIS_SIZE);
glVertex3f(0,0,AXIS_SIZE);
glEnd();
}


//**************************************************************************
// Funcion que dibuja los objetos
//***************************************************************************

void draw_objects()
{
	if(dibujo != -1){
		if(dibujo == 7)
			l.draw(modo,1.0,0.0,0.0,3,material1,material2,material3);
		else if(dibujo == 9){
			float l = 8.0;
			int n = 50;

			std::vector<unsigned char> data;

			CImg<unsigned char> logo;
			logo.load("./imagen.png");

			// empaquetamos bien los datos
			for (long y = 0; y < logo.height(); y ++)
				for (long x = 0; x < logo.width(); x ++)
				{
				unsigned char *r = logo.data(x, y, 0, 0);
				unsigned char *g = logo.data(x, y, 0, 1);
				unsigned char *b = logo.data(x, y, 0, 2);
				data.push_back(*r);
				data.push_back(*g);
				data.push_back(*b);
				}

			glGenTextures(1, &textura_id);
   			glBindTexture(GL_TEXTURE_2D, textura_id);

			glActiveTexture(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			// TRASFIERE LOS DATOS A GPU
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, logo.width(), logo.height(),
			0, GL_RGB, GL_UNSIGNED_BYTE, &data[0]);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, textura_id);

			glTranslatef(-4.0,0.0,2.0);
			// glBegin(GL_QUADS);
			// for(unsigned int i= 0;i < n;i++){
			// 	for(unsigned int j = 0;j<n;j++){
			// 		if(j%2==0){
			// 		glTexCoord2f(j/(1.0*n),(n-(i+1))/(1.0*n));
			// 		glVertex3f(j*l/(1.0*n),(i+1)*l/(1.0*n),0.0);

			// 		glTexCoord2f(j/(1.0*n),(n-i)/(1.0*n));
			// 		glVertex3f(j*l/(1.0*n),i*l/(1.0*n),0.0);

			// 		glTexCoord2f((j+1)/(1.0*n),(n-i)/(1.0*n));
			// 		glVertex3f((j+1)*l/(1.0*n),i*l/(1.0*n),0.0);

			// 		glTexCoord2f((j+1)/(1.0*n),(n-(i+1))/(1.0*n));
			// 		glVertex3f((j+1)*l/(1.0*n),(i+1)*l/(1.0*n),0.0);
			// 		}
			// 		else{
			// 		glTexCoord2f(j/(1.0*n),(n-i)/(1.0*n));
			// 		glVertex3f(j*l/(1.0*n),(i+1)*l/(1.0*n),0.0);

			// 		glTexCoord2f(j/(1.0*n),(n-(i+1))/(1.0*n));					
			// 		glVertex3f(j*l/(1.0*n),i*l/(1.0*n),0.0);

			// 		glTexCoord2f((j+1)/(1.0*n),(n-(i+1))/(1.0*n));					
			// 		glVertex3f((j+1)*l/(1.0*n),i*l/(1.0*n),0.0);

			// 		glTexCoord2f((j+1)/(1.0*n),(n-i)/(1.0*n));
			// 		glVertex3f((j+1)*l/(1.0*n),(i+1)*l/(1.0*n),0.0);
			// 		}
			// 	}
			// }
			// glEnd();
			glBegin(GL_QUADS);
			for(unsigned int i= 0;i < n;i++){
				for(unsigned int j = 0;j<n;j++){
					float a = j/(1.0*n);
					float b = (j+1)/(1.0*n);
					float veces = (1.0/n)*k;

					glTexCoord2f(a+veces,(n-(i+1))/(1.0*n));
					glVertex3f(j*l/(1.0*n),(i+1)*l/(1.0*n),0.0);

					glTexCoord2f(a+veces,(n-i)/(1.0*n));
					glVertex3f(j*l/(1.0*n),i*l/(1.0*n),0.0);

					glTexCoord2f(b+veces,(n-i)/(1.0*n));
					glVertex3f((j+1)*l/(1.0*n),i*l/(1.0*n),0.0);

					glTexCoord2f(b+veces,(n-(i+1))/(1.0*n));
					glVertex3f((j+1)*l/(1.0*n),(i+1)*l/(1.0*n),0.0);
				}
			}
			glEnd();

   			glBindTexture(GL_TEXTURE_2D, 0);
   			glDisable(GL_TEXTURE_2D);
		}
		else{
			if(dibujo==5) glScalef(3,3,3);
			objetos[dibujo].draw(modo,1.0,0.0,0.0,0.0,0.0,1.0,3,material1,material2,material3);
		}
	}
}


//**************************************************************************
//
//***************************************************************************

void draw_scene(void)
{
clear_window();
change_observer();
if(LUCES)luz.luces(luz2);
draw_axis();
draw_objects();
glutSwapBuffers();
}



//***************************************************************************
// Funcion llamada cuando se produce un cambio en el tamaño de la ventana
//
// el evento manda a la funcion:
// nuevo ancho
// nuevo alto
//***************************************************************************

void change_window_size(int Ancho1,int Alto1)
{
change_projection();
glViewport(0,0,Ancho1,Alto1);
glutPostRedisplay();
}


//***************************************************************************
// Funcion llamada cuando se produce aprieta una tecla normal
//
// el evento manda a la funcion:
// codigo de la teclaxit(
// posicion x del raton
// posicion y del raton
//***************************************************************************

void normal_keys(unsigned char Tecla1,int x,int y)
{
if (toupper(Tecla1)=='Q') exit(0);
else if (Tecla1=='1')dibujo=0;
else if(Tecla1=='2')dibujo=1;
else if(Tecla1=='3')dibujo=2;
else if(Tecla1=='4')dibujo=3;
else if(Tecla1=='5')dibujo=4;
else if(Tecla1=='6')dibujo=5;
else if(Tecla1=='7')dibujo=6;
else if(Tecla1=='8')dibujo=7;
else if(Tecla1=='9'){
	dibujo=9;
	LUCES = true;
}
else if(Tecla1 == '<' && dibujo == 7) l.sentido = !l.sentido;
else if(Tecla1=='+' && (dibujo==5 || dibujo==6)){
	if(dibujo==5){
		_rotacion aux(++z);
		objetos[5] = aux;
	}
	else{
		_barrido aux(++w,w2,a);
		objetos[6] = aux;
	}
}
else if(Tecla1=='-' && (dibujo==5 || dibujo==6)){
	if( dibujo == 5 && z != 3){
		_rotacion aux(--z);
		objetos[5] = aux;
	}
	else if(dibujo == 6 && w != 2){
		_barrido aux(--w,w2,a);
		objetos[6] = aux;
	}
}
else if(Tecla1=='.' && dibujo == 6){
	_barrido aux(w,++w2,a);
	objetos[6] = aux;
}
else if(Tecla1 == ',' && dibujo == 6 && a != 3){
	_barrido aux(w,--w2,a);
	objetos[6] = aux;
}
else if(toupper(Tecla1) == 'I' && dibujo == 7){
	if(l.esPosibleMovimiento('i')){
		if(l.sentido)l.giro3+=5;
		else l.giro3-=5;
	}
}
else if(toupper(Tecla1) == 'L' && dibujo == 7){
	if(l.esPosibleMovimiento('l')){
		if(l.sentido)l.giro2+=5;
		else l.giro2-=5;
	}
}
else if(toupper(Tecla1) == 'K' && dibujo == 7){
	if(l.esPosibleMovimiento('k')){
		if(l.sentido) l.giro1+=5;
    else l.giro1-=5;
	}
}
else if(toupper(Tecla1) == 'J' && dibujo == 7){
	if(l.esPosibleMovimiento('j')){
		if(l.sentido) l.giro4+=5;
		else l.giro4-=5;
	}
}
else if(toupper(Tecla1) == 'O' && dibujo == 7){
	if(l.esPosibleMovimiento('o')){
		if(l.sentido) l.giro5+=5;
		else l.giro5-=5;
	}
}
else if(toupper(Tecla1) == 'U' && dibujo == 7){
	if(l.esPosibleMovimiento('u')){
		if(l.sentido) l.giro6+=5;
		else l.giro6-=5;
	}
}
else if(toupper(Tecla1) == 'D' && dibujo == 7){
	int r = 0,rant = 0;
	bool sant;
	l.ajustar();
	for(int i = 0; i < 20; i++){
		do{
			r = rand()%6;
			l.sentido = (rand()%2 == 0)?true:false;
		}while(r==rant && l.sentido == !sant);
		sant = l.sentido;
		rant = r;
		for(int i = 0; i < 31; i++){
			switch (r) {
				case 0:
					if(l.sentido)l.giro1+=3;
					else l.giro1-=3;
				break;
				case 1:
					if(l.sentido)l.giro2+=3;
					else l.giro2-=3;
				break;
				case 2:
					if(l.sentido)l.giro3+=3;
					else l.giro3-=3;
				break;
				case 3:
				if(l.sentido)l.giro4+=3;
				else l.giro4-=3;
				break;
				case 4:
				if(l.sentido)l.giro5+=3;
				else l.giro5-=3;
				break;
				case 5:
				if(l.sentido)l.giro6+=3;
				else l.giro6-=3;
				break;
			}
			clear_window();
			change_observer();
			draw_axis();
			draw_objects();
			glutSwapBuffers();
			std::this_thread::sleep_for(std::chrono::milliseconds(15));
		}
	}
}
else if(toupper(Tecla1)=='R' && dibujo == 7){
	stack<char> aux;
	aux = l.solucion;
	l.ajustar();

	while(!aux.empty()){
		char m = aux.top();
		for(int i = 0; i < 19; i++){
			l.decodificarMovimiento(m);
			clear_window();
			change_observer();
			draw_axis();
			draw_objects();
			glutSwapBuffers();
			std::this_thread::sleep_for(std::chrono::milliseconds(12));
		}
		aux.pop();
	}
	while(!l.solucion.empty()){
		l.solucion.pop();
	}
}
else if (toupper(Tecla1)=='E' && !LUCES){
	LUCES=true;
	modo=SUAVIZADO;
}
else if (toupper(Tecla1)=='E' && LUCES){
	LUCES = false;
	modo=LINEAS;
	luz.noluces();
}
else if (toupper(Tecla1)=='G' && LUCES)luz.rotaluzy-=5;
else if (toupper(Tecla1)=='H' && LUCES)luz.rotaluzy+=5;
else if (toupper(Tecla1)=='Y' && LUCES)luz.rotaluzx-=5;
else if (toupper(Tecla1)=='B' && LUCES)luz.rotaluzx+=5;
else if (toupper(Tecla1)=='P')modo=PUNTOS;
else if (toupper(Tecla1)=='N')modo=LINEAS;
else if (toupper(Tecla1)=='C')modo=AJEDREZ;
else if (toupper(Tecla1)=='F')modo=SOLIDO;
else if (toupper(Tecla1)=='M')modo=MULTIPLE;
else if (toupper(Tecla1)=='A')modo=RUBIK;
else if (toupper(Tecla1)=='X' && LUCES==true)modo=NOSUAVIZADO;
else if (toupper(Tecla1)=='S' && LUCES==true)modo=SUAVIZADO;
glutPostRedisplay();
}

//***************************************************************************
// Funcion llamada cuando se produce aprieta una tecla especial
//
// el evento manda a la funcion:
// codigo de la tecla
// posicion x del raton
// posicion y del raton

//***************************************************************************

void special_keys(int Tecla1,int x,int y)
{
switch (Tecla1){
	case GLUT_KEY_LEFT:Observer_angle_y--;break;
	case GLUT_KEY_RIGHT:Observer_angle_y++;break;
	case GLUT_KEY_UP:Observer_angle_x--;break;
	case GLUT_KEY_DOWN:Observer_angle_x++;break;
	case GLUT_KEY_PAGE_UP:Observer_distance*=1.2;break;
	case GLUT_KEY_PAGE_DOWN:Observer_distance/=1.2;break;
	case GLUT_KEY_F1:luz2 = !luz2;break;
	case GLUT_KEY_F2:
		material1 = true;
		material2 = material3 = false;
	break;
	case GLUT_KEY_F3:
		material2 = true;
		material1 = material3 = false;
	break;
	case GLUT_KEY_F4:
		material3 = true;
		material1 = material2 = false;
	break;
	case GLUT_KEY_F5:
		if(luz.difusa <= 0.0) luz.difusa=0.0;
		else luz.difusa-=0.1;
	break;
	case GLUT_KEY_F6:
		if(luz.difusa >= 1.5) luz.difusa=1.5;
		else luz.difusa+=0.1;
	break;
	case GLUT_KEY_F7:
		k+=1.0;
	break;
	case GLUT_KEY_F8:
		k-=1.0;
	break;
	}
glutPostRedisplay();
}



//***************************************************************************
// Funcion de incializacion
//***************************************************************************

void initialize(void)
{
// se inicalizan la ventana y los planos de corte
Window_width=5;
Window_height=5;
Front_plane=10;
Back_plane=1000;

// se inicia la posicion del observador, en el eje z
Observer_distance=2*Front_plane;
Observer_angle_x=0;
Observer_angle_y=0;

// se indica cual sera el color para limpiar la ventana	(r,v,a,al)
// blanco=(1,1,1,1) rojo=(1,0,0,1), ...
glClearColor(1,1,1,1);

// se habilita el z-bufer
glEnable(GL_DEPTH_TEST);
//
change_projection();
//
glViewport(0,0,UI_window_width,UI_window_height);
}

void mostrarMenu(){
	system("clear");
	cout << "**************************************************************************" << endl << endl;
	cout <<  "Práctica 4"																	<< endl;
	cout <<  "Jose Antonio Ruiz Millan 2017-2018"											<< endl;
	cout <<  "Iluminacion y texturas"																<< endl<< endl;
	cout << "*************************************************************************"	<< endl << endl;
	cout <<  "Objetos:"																	<< endl;
	cout <<  "'1' --> Tetraedro"															<< endl;
	cout <<  "'2' --> Cubo"																<< endl;
	cout <<  "'3' --> Piramide"															<< endl;
	cout <<  "'4' --> Rombo"																<< endl;
	cout <<  "'5' --> Ply"																<< endl;
	cout <<  "'6' --> Revolucion"															<< endl;
	cout <<  "'7' --> Barrido"															<< endl;
	cout <<  "'8' --> Cubo de Rubik"														<< endl;
	cout <<  "'9' --> Textura"															<< endl << endl;
	cout <<  "Formas de dibujo:"															<< endl;
	cout <<  "'f' --> Solido"																<< endl;
	cout <<  "'m' --> Multiples colores aleatorios"										<< endl;
	cout <<  "'n' --> Lineas"																<< endl;
	cout <<  "'p' --> Puntos"																<< endl;
	cout <<  "'c' --> Ajedrez"															<< endl;
	cout <<  "'a' --> Rubik"																<< endl << endl;
	cout << "**************************************************************************"	<< endl << endl;
	cout <<  "Para mover el cubo podemos usas las siguientes teclas:"						<< endl;
	cout <<  "'<' --> Cambia el sentido del movimiento"									<< endl;
	cout <<  "'k' --> Mueve la cara 1 (azul)"												<< endl;
	cout <<  "'l' --> Mueve cara 2 (naranja)"												<< endl;
	cout <<  "'i' --> Mueve cara 3 (verde)"												<< endl;
	cout <<  "'j' --> Mueve cara 4 (roja)"												<< endl;
	cout <<  "'o' --> Mueve cara 5 (amarilla)"											<< endl;
	cout <<  "'u' --> Mueve cara 6 (blanca)"												<< endl;
	cout <<  "'d' --> Realiza x movimientos aleatorios para descolocar el cubo"			<< endl;
	cout <<  "'r' --> Resuelve el cubo automaticamente"									<< endl << endl;
	cout << "**************************************************************************"	<< endl << endl;
	cout <<  "Iluminacion:"																<< endl;
	cout <<  "'e' --> Activar/desactivar las luces."									<< endl;
	cout <<  "'s' --> Pintar en modo suavizado"											<< endl;
	cout <<  "'x' --> Pintar en modo NO suavizado"											<< endl;
	cout <<  "'g' --> Rotar luz a izquierdas"											<< endl;
	cout <<  "'h' --> Rotar luz a derechas"												<< endl;
	cout <<  "'b' --> Rotar luz hacia abajo"											<< endl;
	cout <<  "'y' --> Rotar luz hacia arriba"											<< endl;
	cout <<  "'F1' --> Apagar/encender luz 2"											<< endl;
	cout <<  "'F2' --> Material oro"													<< endl;
	cout <<  "'F3' --> Material turquesa"												<< endl;
	cout <<  "'F4' --> Material ruby"													<< endl;
	cout <<  "'F5' --> Disminuye iluminacion luz en movimiento"							<< endl;
	cout <<  "'F6' --> Aumenta iluminacion luz en movimiento"							<< endl;
	cout << "**************************************************************************"	<< endl << endl;
	cout <<  "Textura:"																	<< endl;
	cout <<  "'F7' --> Traslada la textura hacia la izquierda"							<< endl;
	cout <<  "'F8' --> Traslada la textura hacia la derecha"							<< endl;
	cout << "**************************************************************************"	<< endl << endl;
	cout << "'q' --> Salir" 															<< endl << endl;
}

//***************************************************************************
// Programa principal
//
// Se encarga de iniciar la ventana, asignar las funciones e comenzar el
// bucle de eventos
//***************************************************************************

bool inicio = true;
int main(int argc, char **argv)
{
	if(inicio){
		mostrarMenu();
		inicio = false;
	}

// se llama a la inicialización de glut
glutInit(&argc, argv);
srand (time(NULL));

// se indica las caracteristicas que se desean para la visualización con OpenGL
// Las posibilidades son:
// GLUT_SIMPLE -> memoria de imagen simple
// GLUT_DOUBLE -> memoria de imagen doble
// GLUT_INDEX -> memoria de imagen con color indizado
// GLUT_RGB -> memoria de imagen con componentes rojo, verde y azul para cada pixel
// GLUT_RGBA -> memoria de imagen con componentes rojo, verde, azul y alfa para cada pixel
// GLUT_DEPTH -> memoria de profundidad o z-bufer
// GLUT_STENCIL -> memoria de estarcido
glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

// posicion de la esquina inferior izquierdad de la ventana
glutInitWindowPosition(UI_window_pos_x,UI_window_pos_y);

// tamaño de la ventana (ancho y alto)
glutInitWindowSize(UI_window_width,UI_window_height);

// llamada para crear la ventana, indicando el titulo (no se visualiza hasta que se llama
// al bucle de eventos)
glutCreateWindow("Practica 4 Jose Antonio Ruiz Millan");

//Para evitar el calculo de las caras traseras.
glCullFace(GL_BACK);
glEnable(GL_CULL_FACE);

// asignación de la funcion llamada "dibujar" al evento de dibujo
glutDisplayFunc(draw_scene);
// asignación de la funcion llamada "cambiar_tamanio_ventana" al evento correspondiente
glutReshapeFunc(change_window_size);
// asignación de la funcion llamada "tecla_normal" al evento correspondiente
glutKeyboardFunc(normal_keys);
// asignación de la funcion llamada "tecla_Especial" al evento correspondiente
glutSpecialFunc(special_keys);

//creacion de objetos para dibujar

_objeto_ply obj;
if(argc == 2) obj.parametros(argv[1]);

objetos.push_back(_tetraedro(5, 6));
objetos.push_back(_cubo(5));
objetos.push_back(_piramide(5, 6));
objetos.push_back(_rombo(3, 7));
objetos.push_back(obj);
objetos.push_back(_rotacion(z));
objetos.push_back(_barrido(w,w2,a));

// funcion de inicialización
initialize();

// inicio del bucle de eventos
glutMainLoop();

glDeleteTextures(1, &textura_id);
return 0;
}
