/* 
* top_gate.h
*
* Created: 2016-03-13 16:08:45
* Author: jan.pleszynski
*/


#ifndef __TOP_GATE_H__
#define __TOP_GATE_H__


class Top_gate
{
//functions
public:
	Top_gate();
	bool transmit(volatile uint16_t regs[]);
protected:
private:
	void init();

//variables
public:
protected:
private:

}; //top_gate

#endif //__TOP_GATE_H__
