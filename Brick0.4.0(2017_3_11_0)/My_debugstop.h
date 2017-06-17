#pragma once
//#define MYDEBUG
#ifdef MYDEBUG
namespace MY_DEBUGSTOP {
	void STOP();
}
#define ERROR_DEBUGSTOP(exp) if (exp){MY_DEBUGSTOP::STOP();}
#else
#define ERROR_DEBUGSTOP(exp) (void)0
#endif