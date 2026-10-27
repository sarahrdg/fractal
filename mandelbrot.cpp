#include "X11Display.h"
#include "Palette.h"
#include "Mandel.h"
#include <iostream>
#include <string>
#include <mpi.h>

using namespace std;

// valor default para a largura da tela
const unsigned int DefaultWidth = 800;

// valor default para a altura da tela
const unsigned int DefaultHeight = 600;

// valor default (ponto no plano complexo) para o canto inferior esquerdo da figura a ser gerada 
const dcomplex DefaultZMin = dcomplex(-1.5545173217428, -1.55451670625497);

// valor default (ponto no plano complexo) para o canto superior direito da figura a ser gerada  
const dcomplex DefaultZMax = dcomplex( 0.29958615131552137, 0.39958615593168024); 


// valor default para o numero maximo de iteracoes para o calculo de cada ponto da figura
const int DefaultMaxIter = 20000;

                                         
int 
main(int argc, char *argv[])
{
   string command;
   
   unsigned int largura = DefaultWidth;
   unsigned int altura = DefaultHeight;
   
   int maxiter = DefaultMaxIter;
   
   // Por default, zmin e zmax estão configurados para permitir a visualização
   // da figura de Mandelbrot "por inteiro" (sem zoom).
   dcomplex zmin = DefaultZMin;
   dcomplex zmax = DefaultZMax;

   // Para ver um exemplo de figura com zoom, descomente as linhas abaixo.   
   zmin = dcomplex(-0.482536, -0.635201);
   zmax = dcomplex(-0.565928, -0.579588);
   
   // Novo
   int rank, size;
   MPI_Init(&argc, &argv); // Inicia o MPI
   MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Vê se é mestre ou escravo
   MPI_Comm_size(MPI_COMM_WORLD, &size); // Vê quantos processos são criados

   // Pra rodar 1 mestre e 3 escravos e dar erro se tentar rodar em uma quantidade de processos diferentes
   if (size != 4)
   {
      if (rank == 0)
      {
         cerr << "Execute com 4 processos: 1 mestre e 3 escravos." << endl;
         cerr << "Exemplo: mpirun -np 4 ./mandelbrot" << endl;
      }
      
      MPI_Finalize();
      return 1;
   }
   // MESTRE
   if (rank == 0)
   {
      // Abre a janela gráfica 
      X11Display d;
      d.open(largura, altura, "Mandelbrot MPI");

      // Chama o Mandel
      Mandel m;

      // Cria a paleta de cores, e atualiza a janela
      RGBPalette p(0.4, 0.95, 0.65, 0.45, 0.5, 0.5, 0.5);

      // Chama a função que organiza a divisão de tarefas
      m.calcAndShowMPI(d, p, zmin, zmax, maxiter);

      d.flush();

      // Define que o usuário precisa digitar 'fim' para finalizar o programa
      cout << "Digite fim para encerrar o programa" << endl;

      while (cin >> command)
      {
         if (command == "fim")
            break;
      }

      d.close();
   }
   // ESCRAVO
   else
   {
      Mandel m;

      m.calcSlave(zmin, zmax, maxiter);
   }

   MPI_Finalize();
   return 0;
}
   

