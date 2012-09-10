#include <printXXX>

@test() {
	printXXX("Test!\r\n");
}

@event() {
	printXXX("Event in hello1!\r\n");
}

main() {
    printXXX("\r\nHello world from hello1 script\r\n");

	while(1) {
		sleepXXX();
		@test();
	}
}
