#include "systemc.h"

#ifndef Engine_H_
#define Engine_H_

SC_MODULE(Engine){

	//von der kontrolleinheit
	sc_in<int> S_on, S_off;//B! evtl. bool
	sc_in<double> m_throttle;

	//zur Kontrolleinheit
	sc_out<double> v_current;	

	//Variablendeklaration
	int j;//B! besserer Name für J, zb timer oder counter
	int v_old;
	//Prozesse
	SC_CTOR(Engine){
		SC_METHOD(start);
		sensitive << m_throttle << S_on << S_off;
		cout << "\033[32m\033[1m" << "Timestamp\tthrottle\tv_current" << "\033[0m" << endl;		
	}

	//Funktionen
	void start()
	{
		if (S_on){
			v_current = v_current + acceleration(m_throttle,v_current);
			//Ausgabe alle 60 Sekunden
			if(j==60){
				cout << "\033[32m" << sc_time_stamp() << "\t\t"<< int(m_throttle) << "\t\t" << int(v_current) << " [m/s]" << "\033[0m" << endl;
			j=0;
			}
			//Ausgabe wenn sich die Geschwindigkeit ändert
			if(int(v_current) != v_old){
				cout << "\033[32m" << sc_time_stamp() << "\t\t"<< int(m_throttle) << "\t\t" << int(v_current) << " [m/s]" << "\033[0m" << endl;
			}
			v_old = int(v_current.read());
			j++;	
			next_trigger(1,SC_SEC);}
	}


	double acceleration(double throttle, double v)
	{
		int a_max = 7; // [m/s^2]
		int v_min = 0; // [m/s]
		int v_max = 55; // [m/s]
		int v_descent = 21; // [m/s]
		if(throttle > 100)
			throttle = 100;
		if(throttle < -100)
			throttle = -100;

	    if(v >= v_min || throttle >= 0) {
	    	if(v <= v_descent || throttle < 0)
		    	return (a_max * throttle)/100;
	    	else
		    	return ( ((v-v_descent)*(-a_max)*throttle)/(v_max-v_descent) + a_max*throttle )/100;
	    } else
		return 0.0;
	}




};


#endif

 

