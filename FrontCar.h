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
	int i, dist_old;
	bool first_sight_possible;

	//Prozesse
	SC_CTOR(FrontCar){
		in_range.initialize(false);
		dist.initialize(301);
		SC_METHOD(car2);
		sensitive << car_sighted << v_car;
		dist_old = 300;	
		first_sight_possible = false;	
	}

	//Prozess

	void car2()
	{
		//Variablen
		i = 0;
		sensor_range = 251.0;
		action_range = 100.0;

		if(dist < 301) 
			dist = dist + v_car - v_current;
		
		if(car_sighted == true || (dist < 300 && tempomatstatus == 1)){
			v_acc = v_car;

			//Damit dist nur beim ersten betreten auf 250 gesetzt wird		
			if (first_sight_possible){//dist > 300){
				dist = 250;
				first_sight_possible = false;//i = 1;
			}
			//Außerhalb der Reichweite, ACC wir nicht aktiviert
			if (dist > action_range && dist <= sensor_range){
				in_range = false;
			}				
			//In der Reichweite, ACC wird aktiviert
			if (dist <= action_range && dist > 0){
				if(!in_range) cout << "ACC wird aktiviert" << endl;
				in_range = true;	
			}
			next_trigger(1,SC_SEC);
		}else{
			first_sight_possible = true;
		}
		//muss noch eine bessere Loesung her, Variable i dient nur dazu, 		
		//dass er beim ersten mal betreten nicht wieder dist auf 301 setzt	
		/*if(dist > sensor_range && i == 0){
			dist = 301;
			in_range = false; 
			i = 0;
		}*/
		
		//Ausgabe		
		if(abs(dist - dist_old) > 50){
			cout << "Abstand Frontcar: " << int(dist) <<endl;
			dist_old = int(dist);	
		}
		
	}
};


#endif

 

