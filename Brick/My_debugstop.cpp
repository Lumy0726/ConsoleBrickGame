#include <assert.h>
namespace MY_DEBUGSTOP {
	void STOP() {
		int Temp = 0;
		assert(false);
		Temp = 0;
	}
}