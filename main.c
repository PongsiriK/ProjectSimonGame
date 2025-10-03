
#define STM32F411xE
#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

uint8_t D18_value = 0;
uint8_t D100_value = 0;
uint8_t rounds = 0;
uint8_t previous_arr[100];
uint8_t present_arr[100];
uint32_t seed = 0;
uint32_t soundtime = 1250;


void UART2_SendString(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        while(!(USART2->SR & USART_SR_TXE));
        USART2->DR = str[i];
    }
}

int __io_putchar(int ch) {
    // รอให้ TXE พร้อม
    while(!(USART2->SR & USART_SR_TXE));
    // ส่งตัวอักษรออก
    USART2->DR = (ch & 0xFF);
    return ch;
}

int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        __io_putchar(ptr[i]);
    }
    return len;
}

void my_delay(uint32_t TRESHOLD){
	   const uint32_t TH = (16000000 * (TRESHOLD / 1000)) / 12;
	    for (uint32_t i = 0; i < TH; i++);
}

uint32_t seed_from_temp(void) {

    // เลือก channel 0 (PA0)
    ADC1->SQR3 = 0;    // ช่องแรกเป็น CH0
    ADC1->CR2 |= ADC_CR2_ADON;

    // เริ่มการแปลง
    ADC1->CR2 |= ADC_CR2_SWSTART;

    // รอจนกว่าจะเสร็จ
    while (!(ADC1->SR & ADC_SR_EOC));

    // อ่านค่า
    return ADC1->DR;
}

void init_srng() {
	for (int i=0; i<8; i++) {
	    seed ^= seed_from_temp();
	}
	srand(seed);

}

uint8_t D18() {
	D18_value = rand() % 18; //0-17
	return D18_value;
}

uint8_t chance_checker(uint8_t CHANCE) {
	D100_value = rand() % 100;
	if (D100_value <= CHANCE) {
		return 1;
	}else{
		return 0;
	}
}


void fill_arr(uint8_t* arr, uint8_t value) {
	arr[rounds-1] = value;
}

void play_note(uint16_t frequency, uint16_t duration_ms) {
        if (frequency == 0) {
            // Silence - just delay
            for (volatile uint32_t i = 0; i < duration_ms * 1000; i++);
            return;
        }

        uint32_t half_period = 500000 / frequency;  // Half period in microseconds
        uint32_t cycles = (frequency * duration_ms) / 1000;

        for (uint32_t i = 0; i < cycles; i++) {
            GPIOB->BSRR = (1U << GPIO_BSRR_BS13_Pos);  // PB13 high
            for (volatile uint32_t j = 0; j < half_period; j++);

            GPIOB->BSRR = (1U << GPIO_BSRR_BR13_Pos);  // PB13 low
            for (volatile uint32_t j = 0; j < half_period; j++);
        }
    }


void display_light(uint8_t* arr){
	for (uint8_t i=0;i<rounds;i++){

		GPIOA->BSRR = GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR7  | GPIO_BSRR_BS8 | GPIO_BSRR_BS9;
		GPIOB->BSRR = GPIO_BSRR_BR6 | GPIO_BSRR_BR10;
		GPIOC->BSRR = GPIO_BSRR_BR7;
		switch (arr[i])
		{
		case  0:
			printf("BLUE ");
			GPIOA->BSRR = GPIO_BSRR_BS5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR7 | GPIO_BSRR_BS8 | GPIO_BSRR_BS9;
			GPIOB->BSRR = GPIO_BSRR_BR6 | GPIO_BSRR_BR10;
			GPIOC->BSRR = GPIO_BSRR_BR7;
			play_note(261,soundtime);
			break;
		case  1:
			printf("RED ");
			GPIOA->BSRR = GPIO_BSRR_BR5 | GPIO_BSRR_BS6 | GPIO_BSRR_BR7 | GPIO_BSRR_BS8 | GPIO_BSRR_BS9;
			GPIOB->BSRR = GPIO_BSRR_BR6 | GPIO_BSRR_BR10;
			GPIOC->BSRR = GPIO_BSRR_BR7;
			play_note(277,soundtime);
			break;
		case  2:
			printf("YeLLOW ");
			GPIOA->BSRR = GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BS7 | GPIO_BSRR_BS8 | GPIO_BSRR_BS9;
			GPIOB->BSRR = GPIO_BSRR_BR6 | GPIO_BSRR_BR10;
			GPIOC->BSRR = GPIO_BSRR_BR7;
			play_note(293,soundtime);
			break;
		case  3:
			printf("GREEN ");
			GPIOA->BSRR = GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR7 | GPIO_BSRR_BS8 | GPIO_BSRR_BS9;
			GPIOB->BSRR = GPIO_BSRR_BS6 | GPIO_BSRR_BR10;
			GPIOC->BSRR = GPIO_BSRR_BR7;
			play_note(311,soundtime);
			break;
		case  4:
			printf("0 ");
			GPIOA->BSRR = GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR7  | GPIO_BSRR_BR8 | GPIO_BSRR_BR9;
			GPIOB->BSRR = GPIO_BSRR_BR6 | GPIO_BSRR_BR10;
			GPIOC->BSRR = GPIO_BSRR_BR7;
			play_note(329,soundtime);
			break;
		case  5:
			printf("1 ");
			GPIOA->BSRR = GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR7 | GPIO_BSRR_BR8 | GPIO_BSRR_BR9;
			GPIOB->BSRR = GPIO_BSRR_BR6 | GPIO_BSRR_BR10;
			GPIOC->BSRR = GPIO_BSRR_BS7;
			play_note(349,soundtime);
			break;
		case  6:
			printf("2 ");
			GPIOA->BSRR = GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR7  | GPIO_BSRR_BS8 | GPIO_BSRR_BR9;
			GPIOB->BSRR = GPIO_BSRR_BR6 | GPIO_BSRR_BR10;
			GPIOC->BSRR = GPIO_BSRR_BR7;
			play_note(370,soundtime);
			break;
		case  7:
			printf("3 ");
			GPIOA->BSRR = GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR7  | GPIO_BSRR_BS8 | GPIO_BSRR_BR9;
			GPIOB->BSRR = GPIO_BSRR_BR6 | GPIO_BSRR_BR10;
			GPIOC->BSRR = GPIO_BSRR_BS7;
			play_note(392,soundtime);
			break;
		case  8:
			printf("4 ");
			GPIOA->BSRR = GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR7  | GPIO_BSRR_BR8 | GPIO_BSRR_BR9;
			GPIOB->BSRR = GPIO_BSRR_BR6 | GPIO_BSRR_BS10;
			GPIOC->BSRR = GPIO_BSRR_BR7;
			play_note(415,soundtime);
			break;
		case  9:
			printf("5 ");
			GPIOA->BSRR = GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR7  | GPIO_BSRR_BR8 | GPIO_BSRR_BR9;
			GPIOB->BSRR = GPIO_BSRR_BR6 | GPIO_BSRR_BS10;
			GPIOC->BSRR = GPIO_BSRR_BS7;
			play_note(440,soundtime);
			break;
		case  10:
			printf("6 ");
			GPIOA->BSRR = GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR7  | GPIO_BSRR_BS8 | GPIO_BSRR_BR9;
			GPIOB->BSRR = GPIO_BSRR_BR6 | GPIO_BSRR_BS10;
			GPIOC->BSRR = GPIO_BSRR_BR7;
			play_note(466,soundtime);
			break;
		case  11:
			printf("7 ");
			GPIOA->BSRR = GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR7  | GPIO_BSRR_BS8 | GPIO_BSRR_BR9;
			GPIOB->BSRR = GPIO_BSRR_BR6 | GPIO_BSRR_BS10;
			GPIOC->BSRR = GPIO_BSRR_BS7;
			play_note(493,soundtime);
			break;
		case  12:
			printf("8 ");
			GPIOA->BSRR = GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR7  | GPIO_BSRR_BR8 | GPIO_BSRR_BS9;
			GPIOB->BSRR = GPIO_BSRR_BR6 | GPIO_BSRR_BR10;
			GPIOC->BSRR = GPIO_BSRR_BR7;
			play_note(523,soundtime);
			break;
		case  13:
			printf("9 ");
			GPIOA->BSRR = GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR7  | GPIO_BSRR_BR8 | GPIO_BSRR_BS9;
			GPIOB->BSRR = GPIO_BSRR_BR6 | GPIO_BSRR_BR10;
			GPIOC->BSRR = GPIO_BSRR_BS7;
			play_note(554,soundtime);
			break;
		case 14:
			printf("Up ");
			GPIOA->BSRR = GPIO_BSRR_BS5 | GPIO_BSRR_BS6 | GPIO_BSRR_BS7  | GPIO_BSRR_BR8 | GPIO_BSRR_BS9;
			GPIOB->BSRR = GPIO_BSRR_BS6 | GPIO_BSRR_BR10;
			GPIOC->BSRR = GPIO_BSRR_BR7;
			play_note(587,soundtime);
			break;
		case 15:
			printf("Down ");
			GPIOA->BSRR = GPIO_BSRR_BS5 | GPIO_BSRR_BS6 | GPIO_BSRR_BS7  | GPIO_BSRR_BS8 | GPIO_BSRR_BR9;
			GPIOB->BSRR = GPIO_BSRR_BS6 | GPIO_BSRR_BR10;
			GPIOC->BSRR = GPIO_BSRR_BR7;
			play_note(622,soundtime);
			break;
		case 16:
			printf("Left ");
			GPIOA->BSRR = GPIO_BSRR_BS5 | GPIO_BSRR_BS6 | GPIO_BSRR_BS7  | GPIO_BSRR_BR8 | GPIO_BSRR_BR9;
			GPIOB->BSRR = GPIO_BSRR_BS6 | GPIO_BSRR_BS10;
			GPIOC->BSRR = GPIO_BSRR_BR7;
			play_note(659,soundtime);
			break;
		case 17:
			printf("Right ");
			GPIOA->BSRR = GPIO_BSRR_BS5 | GPIO_BSRR_BS6 | GPIO_BSRR_BS7  | GPIO_BSRR_BS8 | GPIO_BSRR_BR9;
			GPIOB->BSRR = GPIO_BSRR_BS6 | GPIO_BSRR_BS10;
			GPIOC->BSRR = GPIO_BSRR_BR7;
			play_note(698,soundtime);
			break;
		default:
			break;
		}
	}
	printf("\n");


}

void init_device() {
	//Clock setup
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
		RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

		//Set gpio
		GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7 | GPIO_MODER_MODER8 | GPIO_MODER_MODER9);
		GPIOA->MODER |= (0b01 << GPIO_MODER_MODER5_Pos)
					  | (0b01 << GPIO_MODER_MODER6_Pos)
					  | (0b01 << GPIO_MODER_MODER7_Pos)
					  | (0b01 << GPIO_MODER_MODER8_Pos)
					  | (0b01 << GPIO_MODER_MODER9_Pos)
					  | (0b11 << GPIO_MODER_MODER2_Pos);
		GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7 | GPIO_OSPEEDR_OSPEED8 | GPIO_OSPEEDR_OSPEED9);
		GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_5 | GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7 | GPIO_OTYPER_OT_8 | GPIO_OTYPER_OT_9);
		GPIOB->MODER &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER10 | GPIO_MODER_MODER13);
		GPIOB->MODER |= (0b01 << GPIO_MODER_MODER6_Pos)
						| (0b01 << GPIO_MODER_MODER10_Pos)
						| (0b01 << GPIO_MODER_MODER13_Pos);
		GPIOB->OTYPER &= ~(GPIO_OTYPER_OT6 | GPIO_OTYPER_OT10);
		GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED10);
		GPIOC->MODER &= ~(GPIO_MODER_MODER7);
		GPIOC->MODER |= (0b01 << GPIO_MODER_MODER7_Pos);
		GPIOC->OTYPER &= ~(GPIO_OTYPER_OT7);
		GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED7);



		GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
		GPIOA->MODER |= (0b10 << GPIO_MODER_MODER2_Pos)
					  | (0b10 << GPIO_MODER_MODER3_Pos);
		GPIOA->AFR[0] |= (0x7 << (4*2)) | (0x7 << (4*3));

		USART2->BRR = 139; // 115200 @16MHz
		USART2->CR1 |= USART_CR1_UE | USART_CR1_RE | USART_CR1_TE;

}



int main(void)
{
	init_device();
	init_srng();
	for (rounds=1; rounds<=10; rounds++){
		if (rounds == 1) {
			// รอบแรก สุ่มมา 1 ตัวเลย
			fill_arr(present_arr, D18());
		} else {
			// รอบถัดไป copy ของเก่าใส่ present_arr
			for (uint8_t i = 0; i < rounds - 1; i++) {
				present_arr[i] = previous_arr[i];
			}
			// แล้วสุ่มเพิ่มอีก 1 ตัว
			fill_arr(present_arr, D18());
		}

		// แสดงลำดับไฟทั้งหมดของรอบนี้
		display_light(present_arr);
		printf("Round %d: %d\n", rounds, present_arr[rounds - 1]);

		// ดีเลย์เพื่อให้ไฟโชว์นานพอ
		my_delay(5000);

		// copy ค่าเก็บไว้เป็นรอบก่อนหน้า
		for (uint8_t i = 0; i < rounds; i++) {
			previous_arr[i] = present_arr[i];
		}
	}
}



