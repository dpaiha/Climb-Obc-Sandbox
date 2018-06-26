
#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

#include "arch/lpc_arch.h" 	//  SysTick_Enable() from lwip/arch/lpc17xx_40xx_systick_arch.c
#include "webserver.h"
#include "lwip_fs.h"
#include "Page.hh"
#include "Site.hh"


/* Sets up system hardware */
void setupHardware(void)
{
	/* LED0 is used for the link status, on = PHY cable detected */
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED state is off to show an unconnected cable state */
	Board_LED_Set(0, true);
	Board_LED_Set(1, true);
	Board_LED_Set(2, true);

	/* Setup a 1mS sysTick for the primary time base */
	SysTick_Enable(1);

}

static Site MySite = Site();

extern "C" fs_file *get_fs_from_page(const char *name) {
	Page *p = MySite.FindPage(name);
	if (p != 0) {
		DEBUGOUT("[%s]: Returning: %s \n\r", name, p->myurl.c_str());
		return p->GetHttpFile();
	} else {
		DEBUGOUT("[%s]: not found \n\r", name);
		return 0;
	}
}


int main(void) {

	setupHardware();
	ws_init();

	while(1) {
		// Poll and check for PHY stat changes of Ethernet
		uint32_t physts = ws_poll_physts();
		if (physts & PHY_LINK_CHANGED) {
			// Not much usefull info to show here. We always have ame speed/duplex
			if (physts & PHY_LINK_CONNECTED) {
				Board_LED_Set(LED_RED, !(physts & PHY_LINK_ERROR));
			} else {
				Board_LED_Set(LED_RED, true);
			}
		}
	}
}
