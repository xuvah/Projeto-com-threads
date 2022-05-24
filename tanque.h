#include <stdio.h>
#include <iostream>
#include <thread>
#include <math.h>
using namespace std;


class tank{
  
   int id;
   static int idaux;
   public:
    int R,H,r;
    float qout=0.0;
    float qin =0.0;
    int y = rand ()%3+1;
    float h=0.0;
   
   tank(int RR,int rr,int HH){id = idaux++;R=RR;r=rr;H=HH;}

  
   
   void tankstats(){cout <<"Altura do tank "<<id<<":"<<H <<"Nivel de Agua: " << h<< endl;}

};