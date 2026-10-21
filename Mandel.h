#ifndef _MANDEL_H
#define _MANDEL_H

#include <complex> // classes de n�meros complexos fazem parte da biblioteca padr�o C++
#include <vector>
#include <mpi.h>

#define dcomplex std::complex<double>
#define TAG_CONFIG  200
#define TAG_RESULT  201

/**
 Fun��o gen�rica que calcula o quadrado de um n�mero complexo.
 */
template <typename Tc> 
Tc sqr(const Tc& a) 
{ 
   return Tc (a.real() * a.real() - a.imag() * a.imag(), 
              2 * a.real() * a.imag()); 
} 

/**
 Classe que encapsula a gera��o de fractais de Mandelbrot.
 */
class Mandel
{

   public:
   
      /**
        Calcula o valor de um ponto no plano complexo.
        */
      unsigned int calcPoint(dcomplex& c, int maxiter);
      
      /**
        Calcula o fractal de Mandelbrot para todos os pontos da tela.
        @param[in]   d      Tela.
        @param[in]   p      Paleta.
        @param[in]   zmin   Ponto que representa o canto inferior esquerdo do fractal.
        @param[in]   zmax   Ponto que representa o canto superior direito do fractal.
        @param[in]   maxiter   N�mero m�ximo de itera��es para o c�lculo de cada ponto.     
        */        
      template<typename Td, typename Tp>
      void calcAndShow(Td& d, const Tp& p, 
         const dcomplex& zmin, const dcomplex& zmax, int maxiter);
      
      // Isso foi a IA que colocou
      template<typename Td, typename Tp>
      void calcAndShowMPI(Td& d, const Tp& p,
         const dcomplex& zmin, const dcomplex& zmax, int maxiter);

      void calcSlave(const dcomplex& zmin,
                  const dcomplex& zmax,
                  int maxiter);
               
   private:
      // Quadrado do raio do plano complexo de Mandelbrot
      static constexpr double Raio2 = 4.0; 
};

/**
 Calcula o valor de um ponto no plano complexo.
 */
unsigned int
Mandel::calcPoint(dcomplex& c, int maxiter)
{
   int iter = 0;
   dcomplex z(0.0, 0.0);
   dcomplex z2; // quadrado de z
   double m2; // quadrado da magnitude de z

   do {
      z = sqr(z) + c;
      m2 = norm(z);
      iter++;
   } while ((m2 < Raio2) && (iter < maxiter));
   return iter;
}


/**
 Calcula o fractal de Mandelbrot para todos os pontos da tela.
 Usou-se templates (polimorfismo est�tico) para permitir que esta fun��o possa
 ser reutilizada para diferentes classes que representam telas e paletas.
 A vincula��o de tipos aos argumentos da fun��o abaixo ocorre no momento da compila��o
 (note que o c�digo abaixo precisa ser compilado para cada combina��o de argumentos, por
 isso a implementa��o est� num arquivo com extens�o ".h").
 */   

template<typename Td, typename Tp>
void
Mandel::calcAndShow(Td& d, const Tp& p, const dcomplex& zmin, const dcomplex& zmax, int maxiter)
{
   unsigned int x, y;
   dcomplex c;
   int iter;
   unsigned int nx = d.width();
   unsigned int ny = d.height();
   
   double dx = (zmax.real() - zmin.real()) / (double) nx;
   double dy = (zmax.imag() - zmin.imag()) / (double) ny;

   for (x = 0; x < nx; x++) 
   {
      for (y = 0; y < ny; y++) 
      {
         // mapeia (x,y) no plano complexo
         c = dcomplex((double) (zmin.real() + ((double) x * dx)), 
                      (double) (zmin.imag() + ((double) y * dy)));
     
         // calcula valor do fractal no ponto c
         iter = calcPoint(c, maxiter);

         // mostra ponto na tela
         d.drawPoint(x, y, p.getColor((float)iter/maxiter));
      }
   }
}

// ------------------------------------------------------------
// NOVO: MESTRE
// ------------------------------------------------------------
template<typename Td, typename Tp>
void
Mandel::calcAndShowMPI(Td& d, const Tp& p,
   const dcomplex& zmin, const dcomplex& zmax, int maxiter)
{
   int rank, size;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &size);

   unsigned int nx = d.width();
   unsigned int ny = d.height();

   int numEscravos = size - 1;

   double dx = (zmax.real() - zmin.real()) / (double)nx;
   double dy = (zmax.imag() - zmin.imag()) / (double)ny;

   int alturaPorEscravo = ny / numEscravos;

   int config[5];

   for (int escravo = 1; escravo <= numEscravos; escravo++)
   {
      int yBegin = (escravo - 1) * alturaPorEscravo;
      int yEnd;

      if (escravo == numEscravos)
         yEnd = ny;
      else
         yEnd = yBegin + alturaPorEscravo;

      config[0] = nx;
      config[1] = ny;
      config[2] = yBegin;
      config[3] = yEnd;
      config[4] = maxiter;

      MPI_Send(config, 5, MPI_INT, escravo,
               TAG_CONFIG, MPI_COMM_WORLD);
   }

   for (int escravo = 1; escravo <= numEscravos; escravo++)
   {
      MPI_Status status;

      int tamanhoFaixa;
      MPI_Recv(&tamanhoFaixa, 1, MPI_INT, escravo,
               TAG_RESULT, MPI_COMM_WORLD, &status);

      std::vector<unsigned int> iteracoes(tamanhoFaixa);

      MPI_Recv(iteracoes.data(), tamanhoFaixa, MPI_UNSIGNED, escravo,
               TAG_RESULT, MPI_COMM_WORLD, &status);

      int yBegin = (escravo - 1) * alturaPorEscravo;
      int yEnd = (escravo == numEscravos)
                    ? ny
                    : yBegin + alturaPorEscravo;

      int k = 0;

      for (int y = yBegin; y < yEnd; y++)
      {
         for (unsigned int x = 0; x < nx; x++)
         {
            int iter = iteracoes[k++];

            d.drawPoint((int)x, (int)y,
                        p.getColor((float)iter / maxiter));
         }
      }
   }
}

// ------------------------------------------------------------
// NOVO: ESCRAVO
// ------------------------------------------------------------
void
Mandel::calcSlave(const dcomplex& zmin,
                  const dcomplex& zmax,
                  int maxiter)
{
   int rank;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   MPI_Status status;

   int config[5];

   MPI_Recv(config, 5, MPI_INT, 0,
            TAG_CONFIG, MPI_COMM_WORLD, &status);

   unsigned int nx = config[0];
   unsigned int ny = config[1];
   int yBegin = config[2];
   int yEnd = config[3];
   maxiter = config[4];

   double dx = (zmax.real() - zmin.real()) / (double)nx;
   double dy = (zmax.imag() - zmin.imag()) / (double)ny;

   int alturaFaixa = yEnd - yBegin;
   int tamanhoFaixa = nx * alturaFaixa;

   std::vector<unsigned int> iteracoes(tamanhoFaixa);

   int k = 0;

   for (int y = yBegin; y < yEnd; y++)
   {
      for (unsigned int x = 0; x < nx; x++)
      {
         dcomplex c(
            zmin.real() + ((double)x * dx),
            zmin.imag() + ((double)y * dy)
         );

         iteracoes[k++] = calcPoint(c, maxiter);
      }
   }

   MPI_Send(&tamanhoFaixa, 1, MPI_INT, 0,
            TAG_RESULT, MPI_COMM_WORLD);

   MPI_Send(iteracoes.data(), tamanhoFaixa, MPI_UNSIGNED, 0,
            TAG_RESULT, MPI_COMM_WORLD);
}


#endif // _MANDEL_H
