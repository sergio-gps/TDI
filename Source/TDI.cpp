#include <math.h>
#include <fcntl.h>

#include <time.h>

#include <C_General.hpp>
#include <C_Trace.hpp>
#include <C_File.hpp>
#include <C_Arguments.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>
#include <iostream>

C_Image gaussiano(C_Image img);
C_Image gradienteSobel(C_Image img);
C_Image supresionNoMax(C_Image img);

C_Image sobelHorizontal, sobelVertical;

int main(int argc, char **argv)
{
	C_Image original, gauss, sobel, noMax;
	
	original.ReadBMP("Last_Gray.bmp");

	gauss = gaussiano(original); //aqui se hace la convolucion entre las dos matrices

	sobel = gradienteSobel(gauss);

	noMax = supresionNoMax(sobel);

}

C_Image gaussiano(C_Image img) {

	C_Matrix gau (0,2,0,2);

	gau.Gaussian(0.5);
	//esto hace una matriz con valores gaussianos
	//luego hay que hacer la convolucion con la matriz original para obtener la imagen con el filtrado

	//img.palette.Print(3,2);
	gau.Print(5, 2);

	getchar();

	img.Convolution(img, gau);
	img.WriteBMP("gaussian.bmp");
	return img;
}

C_Image gradienteSobel(C_Image img) {
	C_Image result,parcialXY, parcial45 = img;
	double max;

	C_Matrix sobelX = C_Matrix (0, 2, 0, 2);
	sobelX(0, 0) = -1; sobelX(0, 1) = 0; sobelX(0, 2) = 1;
	sobelX(1, 0) = -2; sobelX(1, 1) = 0; sobelX(1, 2) = 2;
	sobelX(2, 0) = -1; sobelX(2, 1) = 0; sobelX(2, 2) = 1;

	sobelHorizontal = img;
	sobelHorizontal.Convolution(img, sobelX);

	max = sobelHorizontal.Max();
	if (max>255) {
		sobelHorizontal.Stretch(0, 255.0); //Esto es para que la matriz tenga valores entre 0 y 255
	}
	sobelHorizontal.WriteBMP("sobelX.bmp");

	/*C_Image sobelHorizontal(img.FirstRow(), img.LastRow(), img.FirstCol(), img.LastCol(), 0.0, 256L);
	sobelHorizontal.CalculateConvolution(img, sobelX);
	getchar();*/

	//cout << sobelHorizontal(1,1);
	/*for(int row = sobelHorizontal.FirstRow(); row <= sobelHorizontal.LastRow(); row++){
		for(int col = sobelHorizontal.FirstCol(); col <= sobelHorizontal.LastCol(); col++){
			cout << sobelHorizontal(row, col);
		}
	}
	getchar();*/	

	C_Matrix sobelY (0, 2, 0, 2);
	sobelY(0, 0) = -1; sobelY(0, 1) = -2; sobelY(0, 2) = -1;
	sobelY(1, 0) = 0; sobelY(1, 1) = 0; sobelY(1, 2) = 0;
	sobelY(2, 0) = 1; sobelY(2, 1) = 2; sobelY(2, 2) = 1;

	sobelVertical = img;
	sobelVertical.Convolution(img, sobelY);

	max = sobelVertical.Max();
	if (max>255) {
		sobelVertical.Stretch(0, 255.0);
	}
	sobelVertical.WriteBMP("sobelY.bmp");

	//-45º

	C_Matrix sobelDiag1(0, 2, 0, 2);
	sobelDiag1(0, 0) = 0; sobelDiag1(0, 1) = 1; sobelDiag1(0, 2) = 2;
	sobelDiag1(1, 0) = -1; sobelDiag1(1,1) = 0; sobelDiag1(1, 2) = 1;
	sobelDiag1(2, 0) = -2; sobelDiag1(2, 1) = -1; sobelDiag1(2, 2) = 0;

	C_Image sobelDiagonal1 = img;
	sobelDiagonal1.Convolution(img, sobelDiag1);

	max = sobelDiagonal1.Max();
	if (max>255) {
		sobelDiagonal1.Stretch(0, 255.0);
	}
	sobelDiagonal1.WriteBMP("sobelDiag1.bmp");

	//45º

	C_Matrix sobelDiag2(0, 2, 0, 2);
	sobelDiag2(0, 0) = -2; sobelDiag2(0, 1) = -1; sobelDiag2(0, 2) = 0;
	sobelDiag2(1, 0) = -1; sobelDiag2(1, 1) = 0; sobelDiag2(1, 2) = 1;
	sobelDiag2(2, 0) = 0; sobelDiag2(2, 1) = 1; sobelDiag2(2, 2) = 2;

	C_Image sobelDiagonal2 = img;
	sobelDiagonal2.Convolution(img, sobelDiag2);

	max = sobelDiagonal2.Max();
	if (max>255) {
		sobelDiagonal2.Stretch(0, 255.0);
	}
	sobelDiagonal2.WriteBMP("sobelDiag2.bmp");

	//Union de las 4 imagenes
	/*cout << "Antes del conv 4";
	getchar();*/
	
	//Esto sobra, sobel ya es un gradiente
	//hay que hacer la raiz cuadrada de la suma de los cuadrados
	//o la media de las 4 sobel
	sobelHorizontal.Gradient(sobelVertical);
	sobelHorizontal.WriteBMP("Last_Sobel_XY.bmp");

	sobelDiagonal1.Gradient(sobelDiagonal2);
	sobelDiagonal1.WriteBMP("Last_Sobel_45.bmp");

	sobelHorizontal.Gradient(sobelDiagonal1);
	
	/*cout << "Union finalizada";
	getchar();*/

	sobelHorizontal.WriteBMP("Last_Sobel_All.bmp");
	return sobelHorizontal;
}

C_Image supresionNoMax(C_Image img) {
	int gx, gy, g1, g2, g3, g4;
	double weight, temp, temp1, temp2;

	C_Image::IndexT row, col;
	C_Image imgNoMax = img;
	imgNoMax.SetValue(0);

	for (int row = img.FirstRow(); row < img.LastRow(); row++) {
		for (int col = img.FirstCol(); col < img.LastCol(); col++) {
			if (img(row, col) != 0) {
				temp = img(row, col);

				gx = sobelHorizontal(row, col);
				gy = sobelVertical(row, col);

				if (abs(gy) > abs(gx)) {
					//Calculamos el radio de interpolacion
					if (gy == 0) {
						weight = 1;
					}
					else {
						weight = fabs((double)gx) / fabs((double)gy);
					}

					g2 = img(row - 1, col);
					g4 = img(row + 1, col);

					if (gx*gy > 0) {
						g1 = img(row - 1, col - 1);
						g3 = img(row + 1, col + 1);
					}
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

					g2 = img(row, col + 1);
					g4 = img(row, col - 1);

					if (gx*gy > 0) {
						g1 = img(row + 1, col + 1);
						g3 = img(row - 1, col - 1);
					}
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

	imgNoMax.WriteBMP("Last_No_Max.bmp");
	cout << "Wait";
	getchar();
	return imgNoMax;
}