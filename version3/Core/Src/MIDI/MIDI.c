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
//extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3; //UART para enviar el codigo MIDI al sintetizador


uint8_t pulsado_anterior1[16] = {0}; //Guardamos el resultado anterior de cada valor de control de cada fila
uint8_t pulsado_anterior2[16] = {0};

void BucleMIDI(){
	for (uint8_t control=0; control<16; control++){ //Recorro 16 valores de control del mux: 0000 0000 hasta 0000 1111
		uint8_t pulsado = 0;
		pulsado = IdentifyNote(control); //Por cada valor de control del Mux compruebo la entrada del Mux
		if (pulsado & 0x01){ //compruebo si en la fila 1 (entrada) se ha pulsado
			if (pulsado_anterior1[control] != 1){
				Send_MIDINoteOn_1(control);
				pulsado_anterior1[control] = 1;
			}
		}
		if (!(pulsado & 0x01)){ //compruebo si en la fila 1 (entrada) se ha dejado de pulsar
			if (pulsado_anterior1[control] == 1){
				Send_MIDINoteOff_1(control);
				pulsado_anterior1[control] = 0;
			}
		}
		if (pulsado & 0x02){ //compruebo si en la fila 1 (entrada) se ha pulsado
			if (pulsado_anterior2[control] != 1){
				Send_MIDINoteOn_2(control);
				pulsado_anterior2[control] = 1;
			}
		}
		if (!(pulsado & 0x02)){ //compruebo si en la fila 2 (entrada) se ha dejado de pulsar
			if (pulsado_anterior2[control] == 1){
				Send_MIDINoteOff_2(control);
				pulsado_anterior2[control] = 0;
			}
		}
	}
}

uint8_t IdentifyNote(uint8_t control){ //Comprobar la entrada del MUX (2 entradas)
	WriteControl(control);
	HAL_Delay(0.015); //15 ns, tiempo de propagacion del MUX necesario
	uint8_t pulsado = 0;
	uint8_t pulsado1 = false;
	uint8_t pulsado2 = false;
	pulsado1 = HAL_GPIO_ReadPin(GPIOA, I1_Pin);  //Comprobar si se ha pulsado una nota en la fila 1
	pulsado2 = HAL_GPIO_ReadPin(GPIOA, I2_Pin); //Comprobar si se ha pulsado una nota en la fila 2
	pulsado = ((pulsado1 & 0x01) | (pulsado2<<1 & 0x02));
	return pulsado;
}

void WriteControl(uint8_t control){ //Escribir en el GPIO correspondiente los valores de control del MUX
	uint8_t S0 = control & 0x01; //0000 0001
	uint8_t S1 = control & 0x02; //0000 0010
	uint8_t S2 = control & 0x04; //0000 0100
	uint8_t S3 = control & 0x08; //0000 1000
	HAL_GPIO_WritePin(GPIOA, S0_Pin, S0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, S1_Pin, S1 ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, S2_Pin, S2 ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, S3_Pin, S3 ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void Send_MIDINoteOn_1(uint8_t control){ //Enviar código MIDI NoteOn de la fila 1 correspondiente a cada nota en funcion del codigo de control
	//Mensajes de MIDI NoteON: canal MIDI 1, octava 3 y valocidad máxima
	uint8_t MIDI_C3[3] = {0x90, 0x30, 0x7F};
	uint8_t MIDI_Csos3[3] = {0x90, 0x31, 0x7F};
	uint8_t MIDI_D3[3] = {0x90, 0x32, 0x7F};
	uint8_t MIDI_Dsos3[3] = {0x90, 0x33, 0x7F};
	uint8_t MIDI_E3[3] = {0x90, 0x34, 0x7F};
	uint8_t MIDI_F3[3] = {0x90, 0x35, 0x7F};
	uint8_t MIDI_Fsos3[3] = {0x90, 0x36, 0x7F};
	uint8_t MIDI_G3[3] = {0x90, 0x37, 0x7F};
	uint8_t MIDI_Gsos3[3] = {0x90, 0x38, 0x7F};
	uint8_t MIDI_A3[3] = {0x90, 0x39, 0x7F};
	uint8_t MIDI_Asos3[3] = {0x90, 0x3A, 0x7F};
	uint8_t MIDI_B3[3] = {0x90, 0x3B, 0x7F};

	switch (control)
	{
	case 0:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_C3, 3, 100);
		break;
	case 1:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Csos3, 3, 100);
		break;
	case 2:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_D3, 3, 100);
		break;
	case 3:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Dsos3, 3, 100);
		break;
	case 4:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_E3, 3, 100);
		break;
	case 5:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_F3, 3, 100);
		break;
	case 6:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Fsos3, 3, 100);
		break;

	case 7:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_G3, 3, 100);
		break;

	case 8:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Gsos3, 3, 100);
		break;

	case 9:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_A3, 3, 100);
		break;

	case 10:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Asos3, 3, 100);
		break;

	case 11:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_B3, 3, 100);
		break;
	}
}

void Send_MIDINoteOff_1(uint8_t control){ //Enviar código MIDI NoteOff de la fila 1 correspondiente a cada nota en función del código de control
	//Mensajes de MIDI NoteOff -> velocidad 0
	uint8_t MIDI_C3[3] = {0x90, 0x30, 0x00};
	uint8_t MIDI_Csos3[3] = {0x90, 0x31, 0x00};
	uint8_t MIDI_D3[3] = {0x90, 0x32, 0x00};
	uint8_t MIDI_Dsos3[3] = {0x90, 0x33, 0x00};
	uint8_t MIDI_E3[3] = {0x90, 0x34, 0x00};
	uint8_t MIDI_F3[3] = {0x90, 0x35, 0x00};
	uint8_t MIDI_Fsos3[3] = {0x90, 0x36, 0x00};
	uint8_t MIDI_G3[3] = {0x90, 0x37, 0x00};
	uint8_t MIDI_Gsos3[3] = {0x90, 0x38, 0x00};
	uint8_t MIDI_A3[3] = {0x90, 0x39, 0x00};
	uint8_t MIDI_Asos3[3] = {0x90, 0x3A, 0x00};
	uint8_t MIDI_B3[3] = {0x90, 0x3B, 0x00};

	switch (control)
	{
	case 0:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_C3, 3, 100);
		break;
	case 1:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Csos3, 3, 100);
		break;
	case 2:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_D3, 3, 100);
		break;
	case 3:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Dsos3, 3, 100);
		break;
	case 4:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_E3, 3, 100);
		break;
	case 5:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_F3, 3, 100);
		break;
	case 6:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Fsos3, 3, 100);
		break;

	case 7:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_G3, 3, 100);
		break;

	case 8:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Gsos3, 3, 100);
		break;

	case 9:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_A3, 3, 100);
		break;

	case 10:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Asos3, 3, 100);
		break;

	case 11:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_B3, 3, 100);
		break;
	}
}

void Send_MIDINoteOn_2(uint8_t control){
	//Mensajes de MIDI NoteON: canal MIDI 1, octava 4 y valocidad máxima
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
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_C4, 3, 100);
		break;
	case 1:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Csos4, 3, 100);
		break;
	case 2:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_D4, 3, 100);
		break;
	case 3:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Dsos4, 3, 100);
		break;
	case 4:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_E4, 3, 100);
		break;
	case 5:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_F4, 3, 100);
		break;
	case 6:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Fsos4, 3, 100);
		break;

	case 7:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_G4, 3, 100);
		break;

	case 8:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Gsos4, 3, 100);
		break;

	case 9:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_A4, 3, 100);
		break;

	case 10:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Asos4, 3, 100);
		break;

	case 11:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_B4, 3, 100);
		break;
	}
}

void Send_MIDINoteOff_2(uint8_t control){
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
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_C4, 3, 100);
		break;
	case 1:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Csos4, 3, 100);
		break;
	case 2:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_D4, 3, 100);
		break;
	case 3:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Dsos4, 3, 100);
		break;
	case 4:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_E4, 3, 100);
		break;
	case 5:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_F4, 3, 100);
		break;
	case 6:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Fsos4, 3, 100);
		break;

	case 7:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_G4, 3, 100);
		break;

	case 8:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Gsos4, 3, 100);
		break;

	case 9:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_A4, 3, 100);
		break;

	case 10:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_Asos4, 3, 100);
		break;

	case 11:
		HAL_UART_Transmit(&huart3, (uint8_t*)&MIDI_B4, 3, 100);
		break;
	}
}
