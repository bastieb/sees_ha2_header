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
	int dist_old;
	bool first_sight_possible;

	//Prozesse
	SC_CTOR(FrontCar){
		in_range.initialize(false);
		dist.initialize(301);
		SC_METHOD(car2);
		sensitive << car_sighted << v_car;
		dist_old = 300;	
		first_sight_possible = true;
		sensor_range = 251.0;
		action_range = 100.0;
	
	}

	//Prozess

	void car2()
	{
		if(tempomatstatus == 1){

			if(!first_sight_possible && dist > sensor_range){
				first_sight_possible = true;
				dist = 301;
				cout << "\033[31m" << "Frontcar außerhalb des Ranges" << "\033[0m" << endl;
			}

			if(car_sighted == true || (dist < sensor_range)){
				v_acc = v_car;
				dist = dist + v_car - v_current;
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
					if(!in_range) cout << "\033[31m" << "ACC wird aktiviert" << "\033[0m" << endl;
					in_range = true;	
				}
				next_trigger(1,SC_SEC);
			}
		
			//Ausgabe		
			if(abs(dist - dist_old) > 50){
				cout << "\033[31m" << "Abstand Frontcar: " << "\033[35m"<< int(dist) << "m\033[0m" << endl;
				dist_old = int(dist);	
			}
		}else{//wenn der Tempomat aus ist
			dist = 301;
		}
		
	}
};


#endif

 

