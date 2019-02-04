#include <cstdio>
#include <ctime>
#include <array>
#include <iostream>
#define cimg_use_jpeg
#include "CImg.h"
#include <X11/Xlib.h>
#include <math.h>
#include "LabAssignment1_ispc.h"

/* 
 * By Robert Sappington for CIS 531
 * Univerity of Oregon
 *
 * Using ISPC Compiler from Intel
 * https://ispc.github.io/
 *
 * Swirl implemntation based on JH Labs Java Image Processing Implementation
 * http://www.jhlabs.com/ip/filters/TwirlFilter.html 
 * 
 * Image reader and writer provided by The Clmg Library
 * http://cimg.eu/
 *
 */

void convert_grayscale_ser(int[],int[],int[],int,int );
void mirror_ser(int[],int[],int[],int,int );
void color_invert_ser(int[],int[],int[],int,int );
void swirl_ser(int[],int[],int[],int[],int[],int[],int,int );

int main() {

  //Read Image 
  cimg_library::CImg<unsigned char> input_img("bombus.jpg");
 
  // New output imaage (width, height, depth, channels (RGB) )
  cimg_library::CImg<unsigned char> output_img(input_img.width(), input_img.height(), 1, 3);
  
  //Create arrays to use for RGB channels
  int W = input_img.width();
  int H = input_img.height();
  int *R = new int [W*H];
  int *G = new int [W*H];
  int *B = new int [W*H]; 

  //Put RGB values in array
  for (int c = 0; c < input_img.width(); ++c) {
    for (int r = 0; r < input_img.height(); ++r) {
      R[r*W+c] = input_img(c, r, 0);
      G[r*W+c] = input_img(c, r, 1);
      B[r*W+c] = input_img(c, r, 2);
 
    }

  }

   std::clock_t start;
   double duration;
  
   auto R2=R;
   auto G2=G;
   auto B2=B;

  std::cout << "Running Swirl" << std::endl;
  start = std::clock();

  //Serial Function
  //convert_grayscale_ser(R,G,B,W,H);
  //mirror_ser(R,G,B,W,H );
  //color_invert_ser(R,G,B,W,H );
  swirl_ser(R2,G2,B2,R,G,B,W,H );

  //Print serial time
  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  std::cout<<"\nTime for Serial execuation: "<< duration <<'\n'; 

  std::cout << "Saving serial image" <<std::endl;
  //Save the image 
  output_img.save_jpeg("output_serial.jpg");
  

  start = std::clock();
  //Parallel Function
  //ispc::convert_grayscale_parallel(R,G,B,W,H); 
  ispc::swirl_parallel(R2,G2,B2,R,G,B,W,H); 
  
  //Print parallel time
  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  std::cout<<"\nTime for Parallel execution: "<< duration <<'\n';
  

  //Construct the output img from our RGB channels
  for (int c = 0; c < input_img.width(); ++c) {
    for (int r = 0; r < input_img.height(); ++r) {
      output_img(c, r, 0)=R[r*W+c];
      output_img(c, r, 1)=G[r*W+c];
      output_img(c, r, 2)=B[r*W+c];

    }
  }
  std::cout << "Saving parallel image" <<std::endl;
  //Save the image 
  output_img.save_jpeg("output_parallel.jpg");
 
  std::cout << "\nExiting program" <<std::endl; 
  return 0;


}


void convert_grayscale_ser(int R[],int G[],int B[],int W,int H ){
  
  for (int c = 0; c < W; ++c) { 
    for (int r = 0; r < H; ++r) {
      
      float avg = ((R[r*W+c]*77 + G[r*W+c]*151 + B[r*W+c])*28) / 3;
      R[r*W+c] = G[r*W+c] = B[r*W+c]  = avg;
    }
  }
}

void mirror_ser(int R[],int G[],int B[],int W,int H ){

  for (int c = 0; c < (W/2); ++c) {
    for (int r = 0; r < H; ++r) {
        float tempR = R[r*W+c];
        R[r*W+c] = R[r*W+(W-c)];
        R[r*W+(W-c)] = tempR;

        float tempG = G[r*W+c];
        G[r*W+c] = G[r*W+(W-c)];
        G[r*W+W-c] = tempG;

        float tempB = B[r*W+c];
        B[r*W+c] = B[r*W+(W-c)];
        B[r*W+W-c] = tempB;


    }
  }
}

void color_invert_ser(int R[],int G[],int B[],int W,int H ){

  for (int c = 0; c < (W); ++c) {
    for (int r = 0; r < H; ++r) {
        R[r*W+c] = 255-R[r*W+c];
        G[r*W+c] = 255-G[r*W+c];
        B[r*W+c] = 255-B[r*W+c];

    }
  }
}

void swirl_ser(int R2[],int G2[],int B2[],int R[],int G[],int B[],int W,int H ){

        float centerx = (W)/2;
        float centery = (H)/2;
        
        float radius = 0;
        if ( centerx > centery)
            radius = centerx;
        else
            radius = centery;
   
        float radius2 = radius * radius;

        for (int y = 0; y < H; ++y) {

            for (int x = 0; x < (W); ++x) {
             
               float x2 = x - centerx;
               float y2 = y - centery;
               float distance = (x2*x2) + (y2*y2);

               if (distance > radius2)
               {
                        R[y*W+x] =R2[y*W+x];
                        G[y*W+x] =G2[y*W+x];
                        B[y*W+x] =B2[y*W+x];
               }
               else{
 
                   float real_distance = sqrt(distance);
                   float angle = 3; 
                   float temp = atan2(y2,x2) + angle * (radius-real_distance)/radius;
                   int new_x =centerx + real_distance*cos(temp);
                   int new_y = centery+ real_distance*sin(temp);
           
                   if ((new_x)>=0 && new_y >= 0 && (new_x) < W && new_y < H)
                   {
                    /* 
                    std::cout << W << " w " << std::endl;
                    std::cout << W << " w " << std::endl;
                    std::cout << new_x << " new x " << std::endl;
                    std::cout << new_y << " new y " << H << std::endl;
                    std::cout << W*new_y+new_x << " matrix " << std::endl;
                    std::cout << W*H << " size" <<std::endl;
                    std::cout << std::endl;
                    */ 
                        int index1 = y*W+x;
                        int index2 = new_y*W+new_x;
                        R[index1] =R2[index2];
                        G[index1] =G2[index2];
                        B[index1] =B2[index2];
                    }
              }

           }
        }
    std::cout << "Finished writing new array"<<std::endl;
}

