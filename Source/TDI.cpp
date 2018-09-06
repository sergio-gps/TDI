#include <math.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

#include <C_General.hpp>
#include <C_Trace.hpp>
#include <C_File.hpp>
#include <C_Arguments.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>
#include <iostream>

int main(int argc, char ** argv);

C_Image leerImagen(C_Image img);
C_Image gaussiano(C_Image img);
C_Image gradienteSobel(C_Image img);
C_Image supresionNoMax(C_Image img);
C_Image gradienteMag(C_Image imagenGradienteX, C_Image imagenGradienteY);
C_Image contornosCandidatos(C_Image noMax, int maxHister, int minHister);
C_Image histeresis(C_Image img);
void seguirContornos(int i, int j);

C_Image sobelHorizontal, sobelVertical, auxiliar;

int main(int argc, char **argv)
{
	C_Image original, gauss, sobel, noMax, imgHis, puntosCandidatos;
	char str[100];
	int maxHister =0, minHister =0;
	
	printf("Introduzca nombre de la imagen: ");
	scanf("%s", str);
	printf("Introduzca umbral maximo para el proceso de histeresis:");
	scanf("%d", &maxHister);
	printf("Introduzca umbral minimo para el proceso de histeresis:");
	scanf("%d", &minHister);
	original.ReadBMP(str);

	gauss = gaussiano(original); //aqui se hace la convolucion entre las dos matrices
	gauss.WriteBMP("finalGauss.bmp");

	sobel = gradienteSobel(gauss);
	sobel.WriteBMP("finalSobel.bmp");

	noMax = supresionNoMax(sobel);
	noMax.WriteBMP("finalNoMax.bmp");

	puntosCandidatos = contornosCandidatos(noMax, maxHister, minHister);
	auxiliar = puntosCandidatos;

	imgHis = histeresis(auxiliar);
	imgHis.WriteBMP("finalImagenCannyFinal.bmp");
	
	system("pause");
	return 0;
}

C_Image leerImagen(C_Image img)
{
	char str[100];
	C_Trace("Introduzca nombre de la imagen: ");
	cin >> str;
	img.ReadBMP(str);
	C_IfError(img.Fail(), "Could not read the image file", leerImagen(img));
	return img;
}

C_Image gaussiano(C_Image img) {

	C_Matrix gau (0,2,0,2);

	gau.Gaussian(0.5);
	//esto hace una matriz con valores gaussianos
	//luego hay que hacer la convolucion con la matriz original para obtener la imagen con el filtrado

	printf("Matriz con valores gaussianos\n");
	gau.Print(5, 2);

	img.Convolution(img, gau);
	printf("Convolucion acabada\n");
	return img;
}

C_Image gradienteSobel(C_Image img) {
	C_Image result,parcialXY, parcial45 = img;

	//Horizontal
	C_Matrix sobelX = C_Matrix (0, 2, 0, 2);
	sobelX(0, 0) = -1; sobelX(0, 1) = 0; sobelX(0, 2) = 1;
	sobelX(1, 0) = -2; sobelX(1, 1) = 0; sobelX(1, 2) = 2;
	sobelX(2, 0) = -1; sobelX(2, 1) = 0; sobelX(2, 2) = 1;

	sobelHorizontal = img;
	sobelHorizontal.Convolution(img, sobelX);

	if (sobelHorizontal.Max()>255) {
		sobelHorizontal.Stretch(0, 255.0); //Esto es para que la matriz tenga valores entre 0 y 255
	}
	sobelHorizontal.WriteBMP("sobelX.bmp");

	//Vertical
	C_Matrix sobelY (0, 2, 0, 2);
	sobelY(0, 0) = -1; sobelY(0, 1) = -2; sobelY(0, 2) = -1;
	sobelY(1, 0) = 0; sobelY(1, 1) = 0; sobelY(1, 2) = 0;
	sobelY(2, 0) = 1; sobelY(2, 1) = 2; sobelY(2, 2) = 1;

	sobelVertical = img;
	sobelVertical.Convolution(img, sobelY);

	if (sobelVertical.Max()>255) {
		sobelVertical.Stretch(0, 255.0);
	}
	sobelVertical.WriteBMP("sobelY.bmp");

	/*
	//-45
	C_Matrix sobelDiag1(0, 2, 0, 2);
	sobelDiag1(0, 0) = 0; sobelDiag1(0, 1) = 1; sobelDiag1(0, 2) = 2;
	sobelDiag1(1, 0) = -1; sobelDiag1(1,1) = 0; sobelDiag1(1, 2) = 1;
	sobelDiag1(2, 0) = -2; sobelDiag1(2, 1) = -1; sobelDiag1(2, 2) = 0;

	C_Image sobelDiagonal1 = img;
	sobelDiagonal1.Convolution(img, sobelDiag1);

	if (sobelDiagonal1.Max()>255) {
		sobelDiagonal1.Stretch(0, 255.0);
	}
	sobelDiagonal1.WriteBMP("sobelDiag1.bmp");

	//45
	C_Matrix sobelDiag2(0, 2, 0, 2);
	sobelDiag2(0, 0) = -2; sobelDiag2(0, 1) = -1; sobelDiag2(0, 2) = 0;
	sobelDiag2(1, 0) = -1; sobelDiag2(1, 1) = 0; sobelDiag2(1, 2) = 1;
	sobelDiag2(2, 0) = 0; sobelDiag2(2, 1) = 1; sobelDiag2(2, 2) = 2;

	C_Image sobelDiagonal2 = img;
	sobelDiagonal2.Convolution(img, sobelDiag2);

	if (sobelDiagonal2.Max()>255) {
		sobelDiagonal2.Stretch(0, 255.0);
	}
	sobelDiagonal2.WriteBMP("sobelDiag2.bmp");*/
	
	//Sobel ya es un gradiente
	//hay que hacer la raiz cuadrada de la suma de los cuadrados
	//o la media de las 4 sobel
	
	return /*gradienteMag(gradienteMag(sobelDiagonal1,sobelDiagonal2),*/gradienteMag(sobelHorizontal,sobelVertical);
}


C_Image gradienteMag(C_Image imagenGradienteX, C_Image imagenGradienteY) {

	C_Image imagenGradienteMagnitud = imagenGradienteX;
	for (int i = imagenGradienteMagnitud.FirstRow(); i < imagenGradienteMagnitud.LastRow()-1; i++) {
		for (int j = imagenGradienteMagnitud.FirstCol(); j < imagenGradienteMagnitud.LastCol()-1; j++) {
			double gX = imagenGradienteX(i, j);
			double gY = imagenGradienteY(i, j);
			imagenGradienteMagnitud(i, j) = (int)(sqrt(gX*gX + gY*gY));
		}
	}
	// Si nuestra imagen excede del rango 255, hacemos que la imagen se ajuste
	double max = imagenGradienteMagnitud.Max();
	if (max>255.0) {
		imagenGradienteMagnitud.Stretch(0, 255);
	}
	printf("Magnitud del gradiente calculada\n");
	return imagenGradienteMagnitud;
}

C_Image supresionNoMax(C_Image img) {
	int gx, gy, g1, g2, g3, g4;
	double weight, temp, temp1, temp2;

	C_Image imgNoMax = img;
	imgNoMax.SetValue(0);

	for (int row = img.FirstRow(); row < img.LastRow(); row++) {
		for (int col = img.FirstCol(); col < img.LastCol(); col++) {
			if (img(row, col) != 0) {
				temp = img(row, col);

				gx = sobelHorizontal(row, col);
				gy = sobelVertical(row, col);

				if (abs(gy) > abs(gx)) {
					if (gy == 0) {
						weight = 1;
					}
					else {
						weight = fabs((double)gx) / fabs((double)gy);
					}

					//|___|g2_|___|
					//|___|_O_|___|
					//|___|g4_|___|
					g2 = img(row - 1, col);
					g4 = img(row + 1, col);

					//|g1_|___|___|
					//|___|_O_|___|
					//|___|___|g3_|
					if (gx*gy > 0) {
						g1 = img(row - 1, col - 1);
						g3 = img(row + 1, col + 1);
					}

					//|___|___|g1_|
					//|___|_O_|___|
					//|g3_|___|___|
					else {
						g1 = img(row - 1, col + 1);
						g3 = img(row + 1, col - 1);
					}
				}
				else {
					if (gx == 0) {
						weight = 1;
					}
					else {
						weight = fabs((double)gy) / fabs((double)gx);
					}

					//|___|___|___|
					//|g4_|_O_|g2_|
					//|___|___|___|
					g2 = img(row, col + 1);
					g4 = img(row, col - 1);

					//|g3_|___|___|
					//|___|_O_|___|
					//|___|___|g1_|
					if (gx*gy > 0) {
						g1 = img(row + 1, col + 1);
						g3 = img(row - 1, col - 1);
					}

					//|___|___|g1_|
					//|___|_O_|___|
					//|g3_|___|___|
					else {
						g1 = img(row - 1, col + 1);
						g3 = img(row + 1, col - 1);
					}
				}

				temp1 = weight*g1 + (1 - weight)*g2;
				temp2 = weight*g3 + (1 - weight)*g4;

				if (temp >= temp1 && temp >= temp2) {
					imgNoMax(row, col) = temp;
				}
				else if (temp < temp1 || temp < temp2) {
					imgNoMax(row, col) = 0;
				}
			}
		}
	}
	printf("No supresion de maximos terminada\n");
	return imgNoMax;
}

C_Image contornosCandidatos(C_Image imgNoMax, int maxHister, int minHister) {
	
	C_Image auxiliar = imgNoMax;

	//Recorremos la matriz de imgNoMax
	for (int i = imgNoMax.FirstRow(); i< imgNoMax.LastRow(); i++) {
		for (int j = imgNoMax.FirstCol(); j<imgNoMax.LastCol(); j++) {
			int valor = imgNoMax(i, j);
			if (valor >= maxHister) {
				auxiliar(i, j) = 1;
			}
			if ((valor<maxHister) && valor >= minHister) {
				auxiliar(i, j) = 2;
			}
		}
	}
	return auxiliar;
}

C_Image histeresis(C_Image auxiliar) {
	for (int i = auxiliar.FirstRow(); i < auxiliar.LastRow(); i++) {
		for (int j = auxiliar.FirstCol(); j < auxiliar.LastCol(); j++) {
			//Si nos hallamos en los bordes, no hacemos nada
			if (i == auxiliar.FirstRow() || i == auxiliar.RowN() - 1 || j == auxiliar.FirstCol() || j == auxiliar.ColN() - 1) {
			}
			else {
				//Si se trata de un punto fuerte
				if (auxiliar(i, j) == 1) {
					//Buscamos vecinos
					seguirContornos(i, j);
				}
			}
		}
	}
	C_Image imagenFinal = auxiliar;
	//los que no son considerados puntos fuertes su valor pasa de 2 a 0
	imagenFinal.SetValue(2, 0);

	imagenFinal.SetValue(1, 255);
	printf("Proceso de histeresis terminado\n");
	return imagenFinal;
}

void seguirContornos(int i, int j) {
	int k, valor =0;
	int x[8] = { 1, 1, 0, -1, -1, -1, 0, 1 }, y[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
	
	for (k = 0; k < 8; k++) {
		valor = auxiliar(i + x[k], j + y[k]);
		//Si el pixel es debil lo ponemos como fuerte
		if (valor == 2) {
			auxiliar(i + x[k], j + y[k]) = 1;
		}
	}
}