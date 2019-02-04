#include <cstdio>
#include <array>
#include <iostream>
#define cimg_use_jpeg
#include "CImg.h"
#include <X11/Xlib.h>
#include <math.h>

void convert_grayscale_ser(int R2[],int G2[],int B2[],int R[],int G[],int B[],int W,int H);

int main() {

  cimg_library::CImg<unsigned char> input_img("bombus.jpg");
 
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

  //Run function for ispc or normal
  convert_grayscale_ser(R2,G2,B2,R,G,B,W,H );

  //Construct the output img from our RGB channels
  for (int c = 0; c < input_img.width(); ++c) {
    for (int r = 0; r < input_img.height(); ++r) {
      output_img(c, r, 0)=R2[r*W+c];
      output_img(c, r, 1)=G2[r*W+c];
      output_img(c, r, 2)=B2[r*W+c];
    }
  }
  //Save the image 
  output_img.save_jpeg("output_bombus.jpg");
  return 0;

}

void convert_grayscale_ser(int R2[],int G2[],int B2[],int R[],int G[],int B[],int W,int H){
  
  for (int c = 0; c < W; ++c) { 
    for (int r = 0; r < H; ++r) {
      
      float avg = ((R[r*W+c] + G[r*W+c] + B[r*W+c])) / 3;
      R2[r*W+c] = G2[r*W+c] = B2[r*W+c]  = avg;
    }
  }
}
