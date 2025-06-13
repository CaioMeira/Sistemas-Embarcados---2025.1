/* Generated by itemis CREATE code generator. */

#ifndef SC_STATEMACHINE_H_
#define SC_STATEMACHINE_H_

namespace sc {

/*! \file 
Basic interface for state machines.
*/
class StatemachineInterface
{
	public:
	
		virtual ~StatemachineInterface() = 0;
		
		/*!
		Enters the state machine. Sets the state machine into a defined state.
		*/
		virtual void enter() = 0;
	
		/*!
		Exits the state machine. Leaves the state machine with a defined state.
		*/
		virtual void exit() = 0;
		
		/*!
		Checks whether the state machine is active. 
	 	A state machine is active if it has been entered. It is inactive if it has not been entered at all or if it has been exited.
	 	*/	
		virtual	sc_boolean isActive() const = 0;
		
		/*!
		Checks if all active states are final. 
	 	If there are no active states then the state machine is considered being inactive. In this case this method returns false.
	 	*/
		virtual sc_boolean isFinal() const = 0;
};

inline StatemachineInterface::~StatemachineInterface() {}

} /* namespace sc */

#endif /* SC_STATEMACHINE_H_ */
