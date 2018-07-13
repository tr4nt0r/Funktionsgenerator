#ifndef AD9833_H
#define AD9833_H

#include <SPI.h>

class AD9833
{
  public:
    //Initialise the AD9833   
    //_mclk is the frequency of the crystal generator 
    AD9833(int _FSYNC, unsigned long _mclk);
    //Set and get for frequency
    void setFreq(unsigned long _freq);
    unsigned long getFreq();
    //Set and get for phase
    void setPhase(int _phase);
    int getPhase();
    //Set and get for control register
    void setCtrlReg(unsigned long _controlRegister);
    int getCtrlReg();
    //Send data to AD9833
    void writeData(int data);
    //Send the control register
    void writeCtrlReg();
    //Update the control register with appopiate values for sleep/reset/mode
    //s = 0 No power down
    //s = 1 DAC powered down
    //s = 2 Internal clock powered down
    //s = 3 Both DAC and internal clock powered down
    void sleep(int s);
    //r = 0 Disable Reset
    //r = 1 Enable Reset
    void reset(int r);
    //m = 0 Sine
    //m = 1 Triangle
    //m = 2 Square
    void mode(int m);
    //r = 0 FREQ0 and PHASE0 are used
    //r = 1 FREQ1 and PHASE1 are used
    void setFPRegister(int r);


  private:
    int FSYNC;          //FSYNC Pin of AD9833 has to be connected to a GPIO pin
    unsigned long freq; //Output frequency
    int phase;          //Signal phase shift
    unsigned long mclk; //External oscillator frequency
    int fqRegister;    //Switch between Frequency and Phase register 1/0
    int pRegister;
    int controlRegister;
};

#endif
