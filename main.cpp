/******************************************************************************
PROJETO DE AUTOMAÇÃO EM TEMPO REAL 
FILIPE AUGUSTO MARQUES DE PAULA - ENGENHARIA DE SISTEMAS
*******************************************************************************/
#include <stdio.h>
#include <iostream>
#include <thread>
#include "tanque.h"
#include <mutex>
#include<chrono>
#include <math.h>
#include <condition_variable>
using namespace std;
int tank::idaux=1;

float h1a,h2a;

mutex mh1,mh2,mq,mqo,mq2;
condition_variable h1,h2;

void simul1(tank&t1,tank&t2){
      //cout<<"thread1..."<<endl;
      this_thread::sleep_for(chrono::milliseconds(200));
      unique_lock lock(mh1);
      mq2.lock();
      mq.lock();
     // cout<<"status tank 1... qin: "<<t1.qin<<" qouts: "<<t1.qout<<", "<<t2.qout<<"altura: " <<t1.h <<endl;
      if((t1.qin - t1.qout - t2.qin)/(M_PI*pow(t1.r+(((t1.R-t1.r)/t1.H)*t1.h),2))<0)
      t1.h=0;
      else{
            h1a=t1.h;
            t1.h = (t1.qin - t1.qout - t2.qin)/(3.14*pow(t1.r+(((t1.R-t1.r)/t1.H)*t1.h),2));
      }
      mq2.unlock();
      mq.unlock();
      h1.notify_one();
      mqo.lock();
      t1.qout= t1.y*1.2*sqrt(t1.h);// equaçao de vazao     
      mqo.unlock();                      
      t1.tankstats();
      //cout<<"fim thread1"<<endl;
  //notificaçao para controle

}
void simul2(tank&t2){
   // cout<<" thread2..."<<endl;
    this_thread::sleep_for(chrono::milliseconds(200));
    unique_lock lock2(mh2);
    mq2.lock();
     //cout<<"status tank 2... qin: "<<t2.qin<<" qouts: "<<t2.qout<<"altura: " <<t2.h <<endl;
    if((t2.qin - t2.qout )/(3.14*pow(t2.r+(((t2.R-t2.r)/t2.H)*t2.h),2))<0)
    t2.h=0;
    else{
           h2a=t2.h;
          t2.h = (t2.qin - t2.qout )/(3.14*pow(t2.r+(((t2.R-t2.r)/t2.H)*t2.h),2));
    }
    mq2.unlock();
    h2.notify_one();   // notificaçao DE controle
    mqo.lock();
    t2.qout= t2.y*1.2*sqrt(t2.h);// equaçao de vazao
    mqo.unlock();
    t2.tankstats();
   // cout<<"fim thread2"<<endl;
}
void PLC(tank&t1,tank&t2) {
        unique_lock lock(mh1);
        unique_lock lock2(mh2);
        h1.wait(lock);
    if (t1.h<=t1.H*0.8||t1.h<=h1a)
    {
     mq.lock();
     t1.qin++;
     cout<<"Aumentandoo fluxo de agua para tank 1 "<<endl;
     mq.unlock();
    }
    else if (t1.h>=t1.H*0.95)
    {
     mq.lock();
     t1.qin--;
     cout<<"Reduzindo entrada de agua no tank 1 "<<endl;
     mq.unlock();
    //cout<<t1.h<<"<-altura de agua"<<t1.H<<"<- altura do tank" <<endl;
    }
     //else  cout<<"tank 1 ENCHENDO"<<endl;
     
     h2.wait(lock2);
    if (t2.h<=t2.H*0.8||t2.h<=h2a)
    {
      mq.lock();
      mq2.lock();
      if(t1.h>t1.H*0.4){
     t2.qin++;
     cout<<" Aumentnado fluxo de agua para tank 2 "<<endl; 
      }
      else{ cout<<" Não ha agua no tank 1 suficiente ainda "<<endl;}
      mq.unlock();
      mq2.unlock();
    }
    else if (t2.h>=t2.H*0.95)
    {
     mq.lock();
     t2.qin--;
     cout<<"Reduzindo entrada de agua no tank 2 "<<endl;
     mq.unlock();
   // cout<<t2.h<<"<-altura de agua"<<t2.H<<"<- altura do tank" <<endl;   
    }
     // else  cout<<" tank 2 enchendo"<<endl;
    //cout<< "fim da thread de controle"<<endl;
     mh1.unlock();
     mh2.unlock();
 }

int main(){
    tank t1(2,2,4),t2(4,4,6);

    int aux=0;
    //for(;aux<2000;aux++)
    //{
      while(!aux){
      thread p_t1(simul1,ref(t1),ref(t2)),p_t2(simul2,ref(t2)),sPLC(PLC,ref(t1),ref(t2));
       p_t1.join();
       p_t2.join();
      sPLC.join(); 
      cout << "tank 1: " <<t1.h<< " ,tank 2: "<<t2.h<< endl<<endl;
    }
}