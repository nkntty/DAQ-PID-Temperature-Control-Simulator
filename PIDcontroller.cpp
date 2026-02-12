#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;
class CSVLogger {
   private:
    string filename_;
   public:
   explicit CSVLogger(const string& filename) : filename_(filename) {
        
        ofstream out(filename_,ios::trunc);
        if (!out.is_open()) {
            throw runtime_error("Could not open " + filename_); 
            //"throw" keyword is used to signal an exceptional circumstance or error//
        }
        out << "time,temp,setpoint,control\n";
    }

    void log(int time, double temp, double setpoint, double control) {
        ofstream out(filename_, ios::app); // "append" mode:: specify that all output operations should append data to the end of the file. 
        // the default mode of file “truncate” , so EACH LOOP DELETES the previous lines and writes only one row.
        if (!out.is_open()) {
            cerr << "Error: Could not open " << filename_ << "\n";
            return;
        }
        out << time << "," << temp << "," << setpoint << "," << control << "\n";
    }

};

class ThermalSystem {
public:
    double temperature = 20.0; 
    double ambient = 20.0;     

    // temperature += (cooling_to_ambient + heating_from_control) * dt
    void update(double heater_power, double dt) {
        // heater_power is expected to be [0.0 ,1.0] (%)
        double cooling = -0.1 * (temperature - ambient);
        double heating = 1.0 * heater_power; // heating gain (tunable)
        temperature += (cooling + heating) * dt;
    }
};

class PID {
private:
    double Kp, Ki, Kd;

    double integral = 0.0;
    double prev_error = 0.0;
public:
    PID(double kp, double ki, double kd) : Kp(kp), Ki(ki), Kd(kd) {}

    // Returns control output (here we clamp to 0..1 for "heater power")
    double compute(double setpoint, double measurement, double dt) {
        double error = setpoint - measurement;

        // Integral (accumulate error over time)
        integral += error * dt;

        // Derivative (rate of change of error)
        double derivative = (dt > 0.0) ? (error - prev_error) / dt : 0.0;

        // PID output
        double u = Kp * error + Ki * integral + Kd * derivative;

        prev_error = error;

        // In <algorithm> library, Clamp to [0, 1] because heater power can't be negative or >100%. used to limit a value to a specified range between a minimum and a maximum boundary.
        u=clamp(u, 0.0, 1.0); 
        return u;
        
    }

    void reset() {
        integral = 0.0;
        prev_error = 0.0;
    }
};

int main() {
   //try and catch keywords are used to implement exception handling
    try {
        
        double dt = 0.1;            
        int steps = 1000;           
        double setpoint = 25000;    
        double ambient = 20.0;      
        double initial_temp = 20.0; 

        
        double Kp = 0.8;
        double Ki = 0.05;
        double Kd = 0.02;

        
        ThermalSystem system;
        system.ambient = ambient;
        system.temperature = initial_temp;

        PID pid(Kp, Ki, Kd);

        CSVLogger logger("data.csv");

        
        for (int t = 0; t < steps; ++t) {
            // Measurement (sensor reading) - in simulation it's just the true temperature
            double measurement = system.temperature;

            // Controller decides heater power based on error
            double control = pid.compute(setpoint, measurement, dt);

            // System evolves based on control input
            system.update(control, dt);

            // Log results
            logger.log(t, system.temperature, setpoint, control);
        }

        cout << "Done! Wrote data.csv (time,temp,setpoint,control)\n";
        cout << "Tip: Open data.csv in Excel or plot it to see the response.\n";
        //an error object thrown by a program.  "throw error → catch error → handle gracefully"

    } catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}