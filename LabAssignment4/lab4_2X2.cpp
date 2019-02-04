#include <chrono>
#include <iostream>
#include <array>

#include <stdio.h>
#include <stdlib.h>
#include<omp.h>
#include <utility>
#include <fstream>
#include "mpi.h"

#define  MASTER   0

using std::cout;
using std::endl;
using std::swap;
using std::ofstream;;
typedef std::chrono::high_resolution_clock Clock;


int main(int argc, char *argv[] )
{
  //MPI material
  int   numtasks, taskid, len;
  char hostname[MPI_MAX_PROCESSOR_NAME];

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
  MPI_Get_processor_name(hostname, &len);

  //End MPI init
 
  //Here we should have our own mpi format

  const int n = 5000;
  double c = 0.1;
  double s= 1.0/(n+1.0);
  double s2 = s*s;
  double t = (s2)/(4*c);
  int niters = 1000;

  double ** input_temp;
  double ** output_temp;
  //cout << " tasked : " << taskid <<endl;

  int my_size;
  int my_size2 = n+2;
  if (taskid == 0 )
  {   my_size = (n/4)+1+1;
  input_temp = new double*[my_size];
  output_temp = new double*[my_size];

  } 

  else if (taskid ==3)
  {
  my_size = (n/4)+1+1;
  input_temp = new double*[my_size];
  output_temp = new double*[my_size];
  

  }
  else if (taskid == 1)
  {
  my_size = (n/4)+0+2;
  input_temp = new double*[my_size];
  output_temp = new double*[my_size];

  }
  else if (taskid==2)
  {
  my_size = (n/4)+0+2;
  input_temp = new double*[my_size];
  output_temp = new double*[my_size];
  
  }

  //cout << "This is task: " << taskid << " with rows: " << my_size << endl;
  //cout << "This is task: " << taskid << " with col: " << n+2  << endl;

  //They all have same num of columns 
  
  //For each row, add a full length column
  for(int i = 0; i < my_size; i++){
    /*if(taskid ==1){
        cout << " On row " << i << " inserted a column of length " << n+2 <<endl;
     }
     */
    input_temp[i]=new double[n+2];
    output_temp[i]=new double [n+2];
    
  }
  //cout << "This is task: " << taskid << " with columns:" << (n+2) << endl;

  //cout << "Initialize 2d arrays " <<endl;
  
  for ( int i = 0; i < my_size; ++ i ) {
      for ( int j = 0; j < my_size2; ++ j) {
          /*if(taskid ==0){
          //cout << " i : " << i << " j : " << j << endl; 
          cout << " " << (my_size-1) << " j : " << my_size2-1<< endl; 
          
          }
          */
          
          if ((i ==0 && taskid ==0)|| j == 0|| (i==my_size-1 && taskid ==3) || j ==my_size2-1)
          {
            input_temp[i][j] = 100.0;
            output_temp[i][j]= 100.0;

          }
          else
          {
            input_temp[i][j] = -100.0;
            output_temp[i][j]= -100.0;
          }
     //  if (taskid ==0)
     //     cout << input_temp[i][j]<<" " ;  
      }
     //if (taskid ==0)
     //     cout << "\n" ;
  }
  //cout << " tasked : " << taskid << " Created arrays, initalized, no issues "<<endl;

   //Print out id array
   /*
   if(taskid==0)
   {
     for ( int i = 0; i < my_size; ++ i ) {
      for ( int j = 0; j < n +1; ++ j) {
          cout << input_temp[i][j] << " ";

        }
       cout << "\n";
    }
    cout << "\n\n\n\n\n";
   }
   */


  //Do the calculation
  int iter = 0;
  int flag = 0;
 auto t1 = Clock::now(); 
 while ( iter <niters )
  {
    //cout << " tasked : " << taskid << " starting iteration: " << (iter+1)<<endl; 
    double myMax = 0.01; 
    iter = iter +1;
    for ( int i = 1; i < my_size-2; ++ i ) {
      for ( int j = 1; j < my_size2-2 ; ++ j) {
         

          //Error checking 
          if(i+1 > my_size-1 || j+1 > (my_size2-2))
          {cout << "\n\ntask id: " <<taskid << endl;
          cout << " i max : " << my_size-2 << " j max " << my_size2-2<<endl;
          cout << " i : " << i << " j: " << j <<endl;
          cout << " i+1 : " << i+1 << " j: " << j <<endl;
          cout << " i : " << i << " j+1: " << j+1 <<endl;
          cout << " i : " << i << " j+1: " << j+1 <<endl;
          }
           //cout << input_temp [ i ][ j] << " + "<< c <<"*"<< t<<"/"<<s2<<" * (" <<input_temp [ i +1][ j ]<< "-4*"<<input_temp[ i ][ j ]<<" + " <<input_temp [ i ][ j +1]<< "+" <<input_temp[ i ][ j -1]<<")" <<endl;
          output_temp [ i ][ j ] =  input_temp [ i ][ j] + c * t/s2 * ( input_temp [ i +1][ j ] + input_temp [i -1][ j ] - 4* input_temp [ i ][ j ] +  input_temp [ i ][ j +1] + input_temp [ i ][ j -1]);
        
        if (output_temp [ i ][ j ] > myMax) 
           { myMax = output_temp [ i ][ j ]; 
           }
      }
    }
    //cout << " tasked : " << taskid << " made it past calcuations on itertions:  " << (iter) <<endl;
    swap (input_temp ,output_temp );
    
    int temp_size = my_size2;
    if(taskid==0)
        {
        MPI_Send(input_temp[my_size-2], (temp_size), MPI_DOUBLE, 1, 0,MPI_COMM_WORLD);
        MPI_Recv(input_temp[my_size-1], temp_size, MPI_DOUBLE, 1, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    else if (taskid==1)
        {
         MPI_Recv(input_temp[0], (temp_size),MPI_DOUBLE, 0, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
         MPI_Send(input_temp[1], temp_size, MPI_DOUBLE, 0, 0,MPI_COMM_WORLD);
        
         MPI_Send(input_temp[my_size-2], temp_size, MPI_DOUBLE, 2, 0,MPI_COMM_WORLD);
         MPI_Recv(input_temp[my_size-1], temp_size, MPI_DOUBLE, 2, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
         }
     else if (taskid==2)
        {
         MPI_Send(input_temp[my_size-2], temp_size,MPI_DOUBLE, 3, 0,MPI_COMM_WORLD);
         MPI_Recv(input_temp[my_size-1], temp_size, MPI_DOUBLE, 3, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
 
         MPI_Recv(input_temp[0], temp_size, MPI_DOUBLE, 1, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
         MPI_Send(input_temp[1], temp_size, MPI_DOUBLE, 1, 0,MPI_COMM_WORLD);
  
         }
     else if(taskid==3) 
        {
         MPI_Recv(input_temp[0], temp_size, MPI_DOUBLE, 2, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
         MPI_Send(input_temp[1], temp_size, MPI_DOUBLE, 2, 0,MPI_COMM_WORLD);
        
         }
     //cout << taskid <<" finshed iteration " << iter << "/"<<niters << endl;
   

   /*
   if(taskid==0)
   {
     for ( int i = 1; i < my_size; ++ i ) {
      for ( int j = 1; j < n +1; ++ j) {
          cout << input_temp[i][j] << " ";
      
        }
       cout << "\n";
    }
    cout << "\n\n\n\n\n";
   }
   */ 
   double temp_myMax; 
   MPI_Reduce(&myMax, &temp_myMax, 1, MPI_DOUBLE, MPI_MAX, MASTER, MPI_COMM_WORLD); 

   //cout << taskid <<" finshed send all on iteration " << iter << "/"<<niters << endl;
   if (taskid ==0)
   {   //cout << "myMaxn " << temp_myMax << endl;
       if (0.1 > (100-myMax))
           flag = 1;
    
   }
   
   MPI_Bcast(&flag, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    
    //printf("Task %d  made it here\n", taskid);
    if (flag) {
      printf("Task %d finished\n", taskid);
      break;
    }
   }
auto t2 = Clock::now();

if(taskid==0)
{  cout << "Runtime: "
    << (std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count())*0.000000001
    << " seconds" << std::endl;
}
// delete []input_temp;
 // delete []output_temp;
  

  cout << " tasked : " << taskid << " ended "<<endl;


  MPI_Finalize();

   //return 0;
}

