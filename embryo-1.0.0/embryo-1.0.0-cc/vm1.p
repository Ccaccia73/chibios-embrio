#include <printXXX>
#include <embrio01>

new state = 0

@eventGPT1()
{
	if(state == 1){
		toggleLED2()
	}
}

@eventGPT3()
{
	if(state == 1){
		readSPI1()
	}
}

@eventEXTI2()
{
	if(state == 0){
		state = 1
		printXXX("\r\nVM 1 UP\r\n")
	}else{
		state = 0
		printXXX("\r\nVM 1 DOWN\r\n")
	}
}

main()
{
	state = 1
    printXXX("\r\nVM 1 UP\r\n")

}
