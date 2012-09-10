#include <printXXX>
#include <embrio01>

new state = 0

@eventGPT1()
{
	if(state == 1){
		toggleLED1()
	}
}

@eventGPT2()
{
	if(state == 1){
		readADC1()
	}
}

@eventEXTI1()
{
	if(state == 0){
		state = 1
		printXXX("\r\nVM 0 UP\r\n")
	}else{
		state = 0
		printXXX("\r\nVM 0 DOWN\r\n")
	}
}

main()
{
	state = 1
    printXXX("\r\nVM 0 UP\r\n")

}
