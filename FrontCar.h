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
	double sensor_range, action_range, dist_lok;
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
			
		if(car_sighted.read()==true || (dist_lok<300 && tempomatstatus.read()==1)){

			v_acc.write(v_car.read());
			dist_lok = dist_lok + v_car.read() - v_current.read();
			dist.write(dist_lok);

			//Damit dist nur beim ersten betreten auf 250 gesetzt wird		
			if (dist_lok > 300){
				dist_lok=250;
				dist.write(dist_lok);
				i=1;
			}
			//Außerhalb der Reichweite, ACC wir nicht aktiviert
			if (dist_lok > action_range && dist_lok <= sensor_range){			
				in_range.write(false);
			}
 			//In der Reichweite, ACC wird aktiviert
			if (dist_lok <= action_range && dist_lok > 0){
				in_range.write(true);	
			}
			next_trigger(1,SC_SEC);
		}
		//muss noch eine bessere Loesung her, Variable i dient nur dazu, 		
		//dass er beim ersten mal betreten nicht wieder dist auf 301 setzt	
		if(dist_lok > sensor_range && i==0){ 
			dist_lok=301; 
			dist.write(dist_lok);
			in_range.write(false); 
			i=0;
		}	
	}
};


#endif

 

