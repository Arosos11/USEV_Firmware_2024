#include "mbed.h"

//Basic Function
float Wait_time = 0.001;  //how fast does the duty cylce update will be a slight underestimation as 

// PWM Production Variables
float Ref_Position;
float Power;
float Max_Power;

//Hall Sensor variables
AnalogIn voltage_pin_1(A0);  // Replace A0 with the actual pin for hall 1
AnalogIn voltage_pin_2(A1);  // Replace A1 with the actual pin for hall 2
AnalogIn voltage_pin_3(A2);  // Replace A2 with the actual pin for hall 3

//Motor Speed Variables
float Motor_Speed;
float Position_t1;
float Position_t2;
float Position_t3;

//Acceleration variables
int MaxPPS;
float Kp; // value between 0-1, 1 is max proportional contorl and 0 is no control
float K_Idle; // value between 0-1 how close to car speed does motor idle when not required

// Function to control the motor based on position input
void Control_Motor(float Ref_Position, float Power, PwmOut &m_drive_11, PwmOut &m_drive_12) {
    
    // Calculate sine of Ref_Position
    float sin_Ref_Position = sin(Ref_Position);

    // Calculate motor control signal
    float Control_Signal = (sin_Ref_Position * Power); //where abouts on sin wave is the motor to generate correct part of pwm sin. Scaled by power factor - what proportion of max power through sin do we want 

    // Take modulus if control signal is negative
    if (sin_Ref_Position < 0.0) {
        Control_Signal = fabs(Control_Signal);
        m_drive_11.write(0.0);  // Turn off the pwm 11
        m_drive_12.write(Control_Signal);  // Apply control signal ad duty cycle to pwm 12 0-1
    } else {
        Control_Signal = (Control_Signal > 1.0);
        m_drive_11.write(Control_Signal);  // Apply control signal as duty cycle to pwm 11 0-1
        m_drive_12.write(0.0);  // Turn off pwm 12
    }
}

void Hall_Sensor_Position(){
    
    float voltage_1 = voltage_pin_1.read();  // Example: Replace this with any necessary scaling
    float voltage_2 = voltage_pin_2.read();  // Example: Replace this with any necessary scaling
    float voltage_3 = voltage_pin_3.read();  // Example: Replace this with any necessary scaling
    Ref_Position = ;//maths to produce calibrated postion as angle in degrees where 0 is when pwm1 has zero voltage required.
}

void Motor_Speed(float Ref_Position, float *Position_t1, float *Position_t2, float Wait_time, float *Motor_Speed ){
// stores last 3 positions and divides by time intervale (Wait_time) to calculatue speed=

    *Position_t2 = (*Position_t1);
    *Position_t1 = Ref_Position;
    *Motor_Speed =(*Position_t2 - *Position_t1)/Wait_time;
}

void Acceleration (float Motor_Speed, float Desired_Speed, float *Power, float MaxRPS, float Kp, float K_Idle){ /// ------- could implement a full PI control here but not sure if its necessary,
    
    float DS_Ratio = Desired_Speed/MaxRPS;
    float MS_Ratio = Motor_Speed/MaxRPS;
    float Error = DS_Ratio - MS_Ratio; //Percentage between current speed and desired speed.
    
    if Motor_Speed > Desired_Speed {
        if DS_Ratio <0.1 {
            *Power = 0;
        }   
        else {
            *Power = MS_Ratio * K_Idle;
        }
    }
    else {
    *Power = (Error * Kp) + MS_Ratio; // - Power level 0-1 = "current power level" + Kp control of error
        if *Power > 1{
            *Power = 1;
        }
    }
} 

void LED_Display(){
    // Update status LED.
}

int main() {
    
    // PWM Production
    Ref_Position = 0.0;   // Position of motor from which everything including sin wave is calculated - - honestly probably doesnt have to be exactly right
    Power = 0;        // Initalise Calibrated Power as a decimal 0-1
    PwmOut m_drive_11(D3);       // Replace D3 with the actual pin connected positive drive of coil one.
    PwmOut m_drive_12(D4);       // Replace D4 with the actual pin connected to negative drive of coil one

    
    //Hall Sensor Varaibles
    //CAN Signal -----------------------

    //Motor Speed Variables
    Motor_Speed  = 0;
    Position_t1 = 0;
    Position_t2 = 0;


    //Acceleration Variables
    MaxRPS= 10000; // update 
    Desired_Speed = // from CAN ----------------
    Kp = 0.2;
    K_Idle = 0.9;


    // Main control loop
    while (1) {
        
        Hall_Sensor_Position(&Ref_Position);  // Hall Sensor function updates motor postion

        Motor_Speed(Ref_Position, &Position_t1, &Position_t2, Wait_time, &Motor_Speed); //Calculates Motor Speed

        Acceleration(Motor_Speed, Desired_Speed, &Power, MaxRPS, Kp, K_Idle); //Takes decoded can bus of accelerometer and updates 

        //Call the motor control function
        Control_Motor(Ref_Position, Power, m_drive_11, m_drive_12);

        // Add a delay to control the update rate (you may adjust this based on your requirements)
        wait(Wait_time);  // Example: Update every 0.1 seconds
    }
}
