/*
 * MIDI.c
 *
 *  Created on: Nov 1, 2023
 *      Author: Andrea Llorente Gomez
 *
 */
#include "MIDI/MIDI.h"
#include "main.h"
#include "stm32f4xx_hal.h"

#include <stdbool.h>
#include <stdint.h>
#include "stdio.h"

/* Declaración de USART (externa) */
extern UART_HandleTypeDef huart2;

void BucleMIDI(){
	uint8_t resultado_anterior[16] = {0};
	for (uint8_t control=0; control<16; control++){ //recorro 16 valores de control del mux
		bool pulsado1 = false;
		pulsado1 = IdentifyNote(control);
		if (pulsado1){
			printf("Se ha pulsado una tecla de la fila");
			if (resultado_anterior[control] != 1){
				Send_MIDINoteOn_1(control);
				resultado_anterior[control] = 1;
			}
		}else{
			if(resultado_anterior[control] == 1){
				Send_MIDINoteOff_1(control);
				resultado_anterior[control] = 0;
			}
		}
	}
}



uint8_t IdentifyNote(uint8_t control){ //Comprobar la entrada del MUX
	WriteControl(control);
	bool pulsado1 = false;
	pulsado1 = HAL_GPIO_ReadPin(GPIOA, I1_Pin);
//	pulsado2 = HAL_GPIO_ReadPin(GPIOA, I2_Pin);
	return pulsado1;
}

void WriteControl(uint8_t control){ //Escribir en el gpio correspondiente los valores de control del MUX
	uint8_t S0 = control & 0x01; //0000 0001
	uint8_t S1 = control & 0x02; //0000 0010
	uint8_t S2 = control & 0x04; //0000 0100
	uint8_t S3 = control & 0x08; //0000 1000
	HAL_GPIO_WritePin(GPIOA, S0_Pin, S0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, S1_Pin, S1 ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, S2_Pin, S2 ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, S3_Pin, S3 ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void Send_MIDINoteOn_1(uint8_t control){
	//Mensajes de MIDI NoteON
	uint8_t MIDI_C4[3] = {0x90, 0x3C, 0x7F};
	uint8_t MIDI_Csos4[3] = {0x90, 0x3D, 0x7F};
	uint8_t MIDI_D4[3] = {0x90, 0x3E, 0x7F};
	uint8_t MIDI_Dsos4[3] = {0x90, 0x3F, 0x7F};
	uint8_t MIDI_E4[3] = {0x90, 0x40, 0x7F};
	uint8_t MIDI_F4[3] = {0x90, 0x41, 0x7F};
	uint8_t MIDI_Fsos4[3] = {0x90, 0x42, 0x7F};
	uint8_t MIDI_G4[3] = {0x90, 0x43, 0x7F};
	uint8_t MIDI_Gsos4[3] = {0x90, 0x44, 0x7F};
	uint8_t MIDI_A4[3] = {0x90, 0x45, 0x7F};
	uint8_t MIDI_Asos4[3] = {0x90, 0x46, 0x7F};
	uint8_t MIDI_B4[3] = {0x90, 0x47, 0x7F};

	switch (control)
	{
	case 0:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_C4, 3, 100);
		break;
	case 1:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_Csos4, 3, 100);
		break;
	case 2:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_D4, 3, 100);
		break;
	case 3:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_Dsos4, 3, 100);
		break;
	case 4:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_E4, 3, 100);
		break;
	case 5:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_F4, 3, 100);
		break;
	case 6:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_Fsos4, 3, 100);
		break;

	case 7:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_G4, 3, 100);
		break;

	case 8:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_Gsos4, 3, 100);
		break;

	case 9:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_A4, 3, 100);
		break;

	case 10:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_Asos4, 3, 100);
		break;

	case 11:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_B4, 3, 100);
		break;
	}
}

void Send_MIDINoteOff_1(uint8_t control){
	//Mensajes de MIDI NoteOff -> velocidad 0
	uint8_t MIDI_C4[3] = {0x90, 0x3C, 0x00};
	uint8_t MIDI_Csos4[3] = {0x90, 0x3D, 0x00};
	uint8_t MIDI_D4[3] = {0x90, 0x3E, 0x00};
	uint8_t MIDI_Dsos4[3] = {0x90, 0x3F, 0x00};
	uint8_t MIDI_E4[3] = {0x90, 0x40, 0x00};
	uint8_t MIDI_F4[3] = {0x90, 0x41, 0x00};
	uint8_t MIDI_Fsos4[3] = {0x90, 0x42, 0x00};
	uint8_t MIDI_G4[3] = {0x90, 0x43, 0x00};
	uint8_t MIDI_Gsos4[3] = {0x90, 0x44, 0x00};
	uint8_t MIDI_A4[3] = {0x90, 0x45, 0x00};
	uint8_t MIDI_Asos4[3] = {0x90, 0x46, 0x00};
	uint8_t MIDI_B4[3] = {0x90, 0x47, 0x00};

	switch (control)
	{
	case 0:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_C4, 3, 100);
		break;
	case 1:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_Csos4, 3, 100);
		break;
	case 2:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_D4, 3, 100);
		break;
	case 3:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_Dsos4, 3, 100);
		break;
	case 4:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_E4, 3, 100);
		break;
	case 5:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_F4, 3, 100);
		break;
	case 6:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_Fsos4, 3, 100);
		break;

	case 7:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_G4, 3, 100);
		break;

	case 8:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_Gsos4, 3, 100);
		break;

	case 9:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_A4, 3, 100);
		break;

	case 10:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_Asos4, 3, 100);
		break;

	case 11:
		HAL_UART_Transmit(&huart2, (uint8_t*)&MIDI_B4, 3, 100);
		break;
	}
}
