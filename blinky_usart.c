#include "stm32l4xx.h" 
volatile uint8_t state;
volatile uint8_t input[2]; 

void Led_On(void){
	// Set the 5th bit (for set)
	GPIOA->BSRR = (1ul << 5);
	state = 1;
}

void Led_Off(void){
	// Set the 21st bit (for reset)
		GPIOA->BSRR = (1ul << 21);
		state = 0;
}
// the interrupt set by config_timer
void TIM6_DAC_IRQHandler(void){
	
	// Check if update interrupt pending flag is set
	if (TIM6->SR & (1ul)) // ((1ul)->TIM_SR_UIF)
	{
		// Set the flag off as the interrupt has been handled
		TIM6->SR &= ~(1ul);
		if (state == 0){
			Led_On();
		} else {
			Led_Off();
		}
	}
}

void Config_Timer(void){
	
	// Initialize clock for TIM6 set RCC_APB1ENR1_TIM6EN 
	RCC->APB1ENR1 |= (1ul << 4);
	
	// Setting Control register of timer
	// Enable bit 0 to enable counter TIM_CR1_CEN
	// Enable bit 2 to only send interrupt when overflow/underflow TIM_CR1_URS
	TIM6->CR1 |= ((1ul) | (1ul << 2));
	
	// Setting our prescaler value since clock speed is 4mhz  by default 4mhz/(61) ~= 1 event per sec (not very accurate?) 
	// Calculation is for uint16 bit counter ..counter value can be set by TIM6->ARR
	TIM6->PSC = (62-1);
	
	// Enable update interrupt set TIM_DIER_UIE
	TIM6->DIER |= (1ul);
	
	// Since NVIC handles all interrupts and exception...need to configure it
	// Function provided to set priority has inputs IRQn_Type IRQn, uint32_t priority
	// Priority of 0 is set whisch is the highest priority
	// IRQn_Type of TIM6_DAC_IRQn = 54 is set
	NVIC_SetPriority(TIM6_DAC_IRQn, 0);
	// Enable the interrupt request
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
}
void Config_GPIOA(void){
	
	//Step 1: initialize gpio for led (set clock and modes)
	//Initializing clock for GPIO-A
	RCC->AHB2ENR |= 1ul;
	
	//(Here, 10=2*5(5th pin)...each pin has 2 bits)
	
	// Unsetting Mode at port GPIOA-5
	GPIOA->MODER &= ~(3ul << 10); // 
	// Setting Mode to output i.e, 01 
	//(00 is input(reset), 10 is alternate function, 11 is analog mode)
	GPIOA->MODER |= (1ul << 10);
	
	// Setting type of output i.e., 0 for push-pull, 1 would be open drain
	// Here 5 cuz only 1 bit
	GPIOA->OTYPER &= ~(1ul << 5);
	
	//Unsetting the speed of I/0
	GPIOA->OSPEEDR &= ~(3ul << 10);
	//Setting speed to medium i.e., 01 
	//(00 for low speed, 10 for fast speed, 11 for high speed)
	GPIOA->OSPEEDR |= (1ul << 10);
	
	//Unsetting the I/O pull up pull down
	//setting to 00 for no pull up pull down (01 for pull up, 10 for pull down, 11 for resrved)
	GPIOA->PUPDR &= ~(3ul << 5);
}

void checkInput(){
	if (input[0] != 'o'){
		return;
	}
	if(input[1] == 'n'){
		Led_On();
	}
	
	if (input[1] == 'f'){
		Led_Off();
	}
}

int main(void){
	Config_GPIOA();
//	Config_Timer();
	
	//configure gpio pin to be used for transmission gpioc RCC_AHB2ENR_GPIOAEN
//	RCC->AHB2ENR |= (1ul); already done for led
	
	//	GPIOA-2 PA-2 with AF7 is for tx of USART2->
	// Set mode to alternate function
	GPIOA->MODER &= ~(3ul << 2*2);
	GPIOA->MODER |= (2ul << 2*2);
	// Output to push pull
	GPIOA->OTYPER &= ~(1ul << 2);
	// Since 4 bit configuation for AFR and pin is 2nd and set 7 for AF7
	GPIOA->AFR[0] &=~(15ul << 4*2);
	GPIOA->AFR[0] |= (7ul << 4*2);
	
	// need to configure PA-3 for rx of USART2 set AF7 (can be done at once by bitwise OR the above code)
	GPIOA->MODER &= ~(3ul << 2*3);
	GPIOA->MODER |= (2ul << 2*3);
	GPIOA->OTYPER &= ~(1ul << 3);
	
	GPIOA->AFR[0] &= ~(15ul << 4*3);
	GPIOA->AFR[0] |= (7ul << 4*3);
	// set the timer for usart2 RCC_APB1ENR1_USART2EN
	RCC->APB1ENR1 |= (1ul << 17);
	
	USART2->BRR = SystemCoreClock/115200;
	//bit to transmit is 8 by default UARTx->CR1 to configure
	// Enable UART by setting 1st bit USART_CR1_UE
	// Enable the transmitter by setting the 3rd bit USART_CR1_TE
	// Enable the reciever by setting the 2nd bit USART_CR1_RE
	USART2->CR1 |= (1ul | 1ul << 3 | 1ul << 2);
	// Enable auto baud rate USART_CR2_ABREN (this is not working??)
//	USART2->CR2 |= (1ul << 20);

	uint8_t out[] = "Control the led. Type on for ON and of for OFF!\n";
	for (int i = 0; i < sizeof(out); i++){
		//checking for Transmission complete bit is set USART_ISR_TC
			while(!(USART2->ISR & 1 << 6));
			// data to send
			USART2->TDR = out[i];
		}
	
	while(1){
		//checking for Reciever bit is not empty USART_ISR_RXNE
		while(!(USART2->ISR & 1 << 5));
		//recive the data and populate the array by shifting prev char by -1 position
		input[0] = input[1];
		input[1] =  USART2->RDR;
		while(!(USART2->ISR & 1 << 6));
		USART2->TDR = USART2->RDR;
		checkInput();
	}
	return 0;
}