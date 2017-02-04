#include "systemc.h"

#ifndef Control_H_
#define Control_H_

SC_MODULE(Control){
	//Signale
	//vom Nutzer/Umgebung
	sc_in<int> B_start,B_stop, p_gas, p_bremse, B_set, B_vm, B_vp;
	sc_out<int> tempomatstatus;

	//zum Motor
	sc_out<int> S_on,S_off;
	sc_out<double>m_throttle;

	//vom Motor
	sc_in<double> v_current;
	
	//vom Frontcar
	sc_in<double> v_acc;
	sc_in<bool> in_range;

	//Variablen
	//int tempomatstatus;
	double v_d;
	int t, tempomatstatus_lok, throttle_lok;
	static const int v_min = 0; // [m/s]
	static const int v_max = 55; // [m/s]

	//Prozessdefinition
	SC_CTOR(Control){
		
		SC_METHOD(control_unit);
		t=0;
		sensitive << B_start << B_stop << p_gas << p_bremse << B_set << B_vm << B_vp;
	}

	//Prozess
	void control_unit()
	{ 
		if(B_start.read()){//B!
			//Motor wird gestartet
			S_on.write(1);

			//Gas wurde gedrückt						
			if(!p_bremse.read()){
				m_throttle.write(p_gas.read());
				//if(p_gas)cout << "Gas ist " << p_gas << endl;
			}
			//Bremse wurde gedrückt
			if(p_bremse.read()){
				m_throttle.write(-p_bremse.read()); 
				t=0; 
				tempomatstatus_lok = 0;
				tempomatstatus.write(tempomatstatus_lok);
				//cout << "Bremse ist " << p_bremse << endl;
			}

			//Tempomat wurde angeschalten
			if(B_set.read()){
				tempomatstatus_lok = 1;
				tempomatstatus.write(tempomatstatus_lok); 
			}
			//Tempomatmodus
			if(tempomatstatus_lok && !p_bremse.read()){
					//v_d=v_current nur beim Anschalten
					if(t == 0){
						v_d=v_current.read();
						}
					t=1;	
					//v_d anpassen
					if (B_vm.read() && v_current.read() > v_min && v_d >= v_min) v_d=v_d-1;
					if (B_vp.read() && v_current.read() < v_max && v_d <= v_max) v_d=v_d+1;
					
					//Tempomat passt m_throttle an
					if(!p_gas.read()){
						
						//Tempomat normal 
						if(!in_range.read())
							m_throttle.write(controller(v_d, v_current.read()));
//B! warum nicht else?						
						//ACC, FrontCar gesichtet
						if(in_range.read()){
							//ACC wenn v_d>v_acc (Vergleich double ist schlecht)
							if(v_d > v_acc.read())
								throttle_lok = controller(v_acc.read(), v_current.read());
								m_throttle.write(throttle_lok);
							//Tempomat
							if(v_d <= v_acc.read())
								throttle_lok = controller(v_d, v_current.read());
								m_throttle.write(throttle_lok);
						}	
					}			
					next_trigger(1,SC_SEC);	
			}
		}
		else{
			S_off.write(1);
			S_on.write(0); 
			m_throttle.write(0);
			tempomatstatus_lok=0; 
			tempomatstatus.write(tempomatstatus_lok); 
			t = 0;}
	}


	double controller(double v_d, double v)
		{
		
		// Parameter und Hilfsvariablen für PD-Controller		
		int kp=8, kd=4;
		double e_old = 0.0, e_new, dE, throttle_new;
		
		e_new = v_d-v;
		dE = e_new - e_old;
		e_old = e_new;
		
		throttle_new = kp*e_new + kd*dE;

		if(throttle_new > 100)
			return 100;
		else if ( throttle_new < -100)
			return -100;
		else
			return throttle_new;

		}
};
#endif

 

