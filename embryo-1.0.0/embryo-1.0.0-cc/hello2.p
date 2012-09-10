#include <printXXX>

@event() {
	printXXX("Event in hello2!\r\n");
}

main() {
    printXXX("\r\nHello world from hello2 script\r\n");

	while(1) {
		sleepXXX();
		toggleXXX();
	}
}
