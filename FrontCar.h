#include "systemc.h"

#ifndef FrontCar_H_
#define FrontCar_H_

SC_MODULE(FrontCar){

	//vom Sensor/ Umgebung
	sc_in<double> v_car;
	sc_in<bool> car_sighted;

	//zum Sensor/ Umgebung
	sc_out<double> dist;

	//zur Kontrolleinheit
	sc_out<double> v_acc;	
	sc_out<bool> in_range;

	//von der Kontrolleinheit
	sc_in<int> tempomatstatus;
	
	//vom Motor
	sc_in<double> v_current;

	//Variablendeklaration
	double sensor_range, action_range;
	int i;

	//Prozesse
	SC_CTOR(FrontCar){
		in_range.initialize(false);
		dist.initialize(301);
		SC_METHOD(car2);
		sensitive << car_sighted << v_car;		
	}

	//Prozess

	void car2()
	{
		//Variablen
		i=0;sensor_range=251.0;action_range=100.0;
			
		if(car_sighted==true || (dist<300 && tempomatstatus==1)){

			v_acc=v_car;dist=dist+(v_car-v_current);

			//Damit dist nur beim ersten betreten auf 250 gesetzt wird		
			if (dist>300){dist=250;i=1;}
			//Außerhalb der Reichweite, ACC wir nicht aktiviert
			if (dist>action_range && dist <= sensor_range)in_range=false;				
			//In der Reichweite, ACC wird aktiviert
			if (dist<=action_range && dist>0){
			in_range=true;	
			}
			next_trigger(1,SC_SEC);
		}
		//muss noch eine bessere Loesung her, Variable i dient nur dazu, 		
		//dass er beim ersten mal betreten nicht wieder dist auf 301 setzt	
		if(dist>sensor_range && i==0){ dist=301, in_range=false; i=0;}	
	}
};


#endif

 

