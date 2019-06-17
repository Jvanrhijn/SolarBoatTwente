/* 
 * File:   PID_caller.cpp
 * Author: arjan
 * 
 * Created on 11 maart 2019, 12:23
 */
//#include "structures.h"

#include "../../../lib-cpp/Debugging/easy_debugging.hpp"

#include "pid.h"
#include "PID_caller.h"

using namespace MIO;
using namespace structures;
using namespace Control;
using namespace std;

MIO::Control::PID_caller::PID_caller(){
  pid_roll = new PID();
  pid_height = new PID();
  pid_pitch = new PID();
  file.open("/root/logfiles/pd_log_test_06_07.csv");
}

MIO::Control::PID_caller::~PID_caller() {
}

void MIO::Control::PID_caller::PID_in(structures::FlyMode fly_mode){ 
  DataStore::RealData inputdata = m_complementary_data->GetComplementaryData();
  DataStore::PIDDataTotal PIDData;
  
  pid_roll->set_PID_roll(STATE7);
  PIDData.Force_roll = pid_roll->calculate(0, inputdata.Real_roll);

  pid_pitch->set_PID_pitch(STATE1);            //0, val = reference, previous value
  PIDData.Force_pitch = pid_pitch->calculate(0, inputdata.Real_pitch);
  
  pid_height->set_PID_height(STATE1);
  PIDData.Force_height = pid_height->calculate(0.2, inputdata.Real_height);

  switch(fly_mode) {
    case NO_FLY: 
      PIDData.Force_height = pid_height->calculate(0, inputdata.Real_height); 
      M_INFO<<"USING STATE NO_FLY";
      break;
    case FLY: 
      PIDData.Force_height = pid_height->calculate(0.3, inputdata.Real_height);  
      M_INFO<<"USING STATE FLY";
      break;
    case BRIDGE: 
      PIDData.Force_height = pid_height->calculate(0.1, inputdata.Real_height); 
      M_INFO<<"USING STATE BRIDGE";
      break;
    case SLALOM: 
      PIDData.Force_height = pid_height->calculate(-0.05, inputdata.Real_height); 
      M_INFO<<"USING STATE SLALOM";
      break;
  }
  M_INFO<<"PID INPUT VALUES:\n"<<"Roll: "<<inputdata.Real_roll<<" | Pitch: "
      << inputdata.Real_pitch<<" | Height: "<<inputdata.Real_height;
  M_INFO<<"\nPID FORCES: \n"<<"Roll: "<<PIDData.Force_roll<<" | Pitch: "
      << PIDData.Force_pitch<<" | Height: "<<PIDData.Force_height;
  
  M_INFO<<"SPLIT VALUES HEIGHT: \n"
      <<"Pout: "<<pid_height->get_PID_values().Pout<<" | Iout: "<<pid_height->get_PID_values().Iout
      <<" | Dout: "<< pid_height->get_PID_values().Dout;
  
  file<<"SPLIT VALUES ROLL: "
    <<"Pout: "<<pid_roll->get_PID_values().Pout<<"\tIout: "<<pid_roll->get_PID_values().Iout<<"\tIntegral: "<<pid_roll->get_PID_values().intergral
    <<"\tDout: "<< pid_roll->get_PID_values().Dout<< "\tRoll: "<<inputdata.Real_roll<<"\nAngle_right: "<<inputdata.Real_height <<"\n"<< flush  ;

  //Finally copy the PIDData into the DataStore object
  m_PID_data-> PutPIDData(&PIDData);
}
    

