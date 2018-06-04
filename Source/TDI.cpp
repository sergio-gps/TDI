#include <math.h>
#include <fcntl.h>

#include <time.h>

#include <C_General.hpp>
#include <C_Trace.hpp>
#include <C_File.hpp>
#include <C_Arguments.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>

C_Image gaussiano(C_Image img);

int main(int argc, char **argv)
{
	C_Image original;
	

	original.ReadBMP("Aguadulce_Gris.bmp");

	gaussiano(original);

	//aqui se hace la convolucion entre las dos matrices
	
	original.WriteBMP("gau.bmp");
}

C_Image gaussiano(C_Image img) {

	C_Image gau (1,3,1,3);

	gau.Gaussian((float)0.3);
	//esto hace una matriz con valores gaussianos
	//luego hay que hacer la convolucion con la matriz original para obtener la imagen con el filtrado

	//img.palette.Print(3,2);
	//img.Print(5, 2);

	img.WriteBMP("gau.bmp");

	return img;

}