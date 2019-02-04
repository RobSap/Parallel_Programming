#include <chrono>
#include <iostream>
#include <array>

#include <stdio.h>
#include <stdlib.h>
#include<omp.h>
#include <utility>
#include <fstream>


typedef std::chrono::high_resolution_clock Clock;

using namespace std;

int main(int argc, char *argv[] )
{
const int n = 100; 
omp_set_num_threads(8);

double ** input_temp = new double*[n+2];
double ** output_temp = new double*[n+2];

cout << "Creating 2 d arrays " << endl;
for(int i = 0; i < n+2; i++){
    input_temp[i]=new double[n+2];
    output_temp[i]=new double [n+2];
}

double c = 0.1;
double s= 1.0/(n+1.0);
double s2 = s*s;
double t = (s2)/(4*c);
int niters = 100;

cout << "Initialize 2d arrays " <<endl;
for ( int i = 0; i <= n +1; ++ i ) {
    for ( int j = 0; j <= n +1; ++ j) {
        if (i ==0 || j == 0|| i==n+1 || j ==n+1)
        {
            input_temp[i][j] = 100.0;
            output_temp[i][j]= 100.0;

        }
        else
        {
            input_temp[i][j] = -100.0;
            output_temp[i][j]= -100.0;
        }
    }
}

int i,j,ii,jj;
auto t1 = Clock::now();

for ( int iter = 0; iter < niters ; ++ iter ) {
    
     #pragma omp parallel for schedule(static) shared(input_temp, output_temp) private(i, j)
    for ( i = 1; i < n +1; ++ i ) {
        for ( j = 1; j < n +1; ++ j) {
            output_temp [ i ][ j ] = input_temp [ i ][ j] + c * t/s2 * 
            ( input_temp [ i +1][ j ] + input_temp [i -1][ j ] -
            4* input_temp [ i ][ j ] +
            input_temp [ i ][ j +1] + input_temp [ i ][ j -1]);
         }
    }

    swap (input_temp ,output_temp );
}

 auto t2 = Clock::now();
cout << "End clock" << endl;

  cout << "Runtime: "
    << (std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count())*0.000000001
    << " seconds" << std::endl;

ofstream myfile ("parallel.txt");
  if (myfile.is_open())
  {
          for( int ii =0; ii < n+2; ii ++){
              for (int jj = 0; jj < n+2; jj++){
          myfile << output_temp[ii][jj] << " " ;
         }
         myfile << "\n";
        }
        myfile.close();
  }
  else cout << "Error opening write out file " << endl;

delete []input_temp;
delete []output_temp;
}
