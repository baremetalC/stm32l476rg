#include "stm32l4xx.h" 

void Add_Delay(unsigned int delayMS){
	unsigned int ticks = 0;
	while(++ticks < delayMS){
		__NOP();
	}
}

	
int main(void){
	//Step 1: initialize gpio for led (set clock and modes)
	//Initializing clock for GPIO-A
	RCC->AHB2ENR = 1ul;
	
	//(Here, 10=2*5(5th pin)...each pin has 2 bits)
	
	// Unsetting Mode at port GPIOA-5
	GPIOA->MODER &= ~(3ul << 10); // 
	// Setting Mode to output i.e, 01 
	//(00 is input(reset), 10 is alternate function, 11 is analog mode)
	GPIOA->MODER |= (1ul << 10);
	
	// Setting type of output i.e., 0 for push-pull, 1 would be open drain
	// Here 5 cause only 1 bit
	GPIOA->OTYPER &= ~(1ul << 5);
	
	//Unsetting the speed of I/0
	GPIOA->OSPEEDR &= ~(3ul << 10);
	//Setting speed to medium i.e., 01 
	//(00 for low speed, 10 for fast speed, 11 for high speed)
	GPIOA->OSPEEDR |= (1ul << 10);
	
	//Unsetting the I/O pull up pull down
	//setting to 00 for no pull up pull down (01 for pull up, 10 for pull down, 11 for resrved)
	GPIOA->PUPDR &= ~(3ul << 5);
	
	//Step 2: infinite loop -> led_on, add_delay, led_off, add_delay
	while(1){
		// ODR is the ouput data register and can be set directly to 1 for on and 0 for off
		// BSSR is bit set reset register and has 16 set bit and 16 reset bits
		// set bit is set for on and reset bit is set for off
		// Set the 5th bit (for set)
		GPIOA->BSRR = (1ul << 5);
		//Add delay (arbitary big number..for exact delay have to check clock speed)
		Add_Delay(200000);
		// Set the 21st bit (for reset)
		GPIOA->BSRR = (1ul << 21);
		Add_Delay(200000);
	}
	return 0;
}