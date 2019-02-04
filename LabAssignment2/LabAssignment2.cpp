#include <cstdio>
#include <ctime>
#include <array>
#include <iostream>
#define cimg_use_jpeg
#include "CImg.h"
#include <X11/Xlib.h>
#include <math.h>
#include <fstream>
#include <string>

#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"

/* 
 * By Robert Sappington for CIS 531
 * Univerity of Oregon
 *
 * Using ICC and TBB from Intel
 *
 * Swirl implemntation based on JH Labs Java Image Processing Implementation
 * http://www.jhlabs.com/ip/filters/TwirlFilter.html 
 * 
 * Image reader and writer provided by The Clmg Library
 * http://cimg.eu/
 *
 */

void swirl_ser(int[],int[],int[],int[],int[],int[],int,int );

int main (int argc, char *argv[]) {
  std::ifstream file(argv[1]);
  std::string tempString; 
  std::clock_t start;
  

  double duration;
  start = std::clock();
 
  while (std::getline(file, tempString))
  {

    cimg_library::CImg<unsigned char> input_img(tempString.c_str());
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
   auto R2=R;
   auto G2=G;
   auto B2=B;
  
  double swirl_timer = std::clock();
  
  swirl_ser(R2,G2,B2,R,G,B,W,H );
  
  double siwrl_timer2 = ( std::clock() - swirl_timer ) / (double) CLOCKS_PER_SEC;
  std::cout<<"\nTime to swirl : "<< tempString << " " << siwrl_timer2 ;

  //Construct the output img from our RGB channels
  for (int c = 0; c < input_img.width(); ++c) {
    for (int r = 0; r < input_img.height(); ++r) {
      output_img(c, r, 0)=R[r*W+c];
      output_img(c, r, 1)=G[r*W+c];
      output_img(c, r, 2)=B[r*W+c];

    }
  }
   //Save the image 
   std::string tempString2 = tempString + "_serial.jpg";
   output_img.save_jpeg(tempString2.c_str());
  
 }//End while loop

  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  std::cout<<"\nTime for Serial distoration: "<< duration <<'\n';
  return 0;
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
           int index1 = y*W+x;
           int index2 = new_y*W+new_x;
           R[index1] =R2[index2];
           G[index1] =G2[index2];
           B[index1] =B2[index2];
          }
        }
      }
    }
}
