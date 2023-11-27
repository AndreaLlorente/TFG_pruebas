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
extern UART_HandleTypeDef huart2; //UART para enviar el codigo MIDI al sintetizador


uint8_t pulsado_anterior1[16] = {0}; //Guardamos el resultado anterior de cada valor de control de cada fila
uint8_t pulsado_anterior2[16] = {0};
uint8_t pulsado_anterior3[16] = {0};

uint8_t volumen = 0x7F;  // Inicializamos el volumen al maximo
uint8_t vibrato_activo = 0;

void MIDILoop(){
	for (uint8_t control=0; control<16; control++){ //Recorro 16 valores de control del mux: 0000 0000 hasta 0000 1111
		uint8_t pulsado = 0;
		pulsado = IdentifyRow(control); //Por cada valor de control del Mux compruebo la entrada del Mux
		if (pulsado & 0x01){ //compruebo si en la fila 1 (entrada) se ha pulsado (flag subida)
			if (pulsado_anterior1[control] != 1){
				Send_MIDINoteOn_1_WithVolume(control);
				pulsado_anterior1[control] = 1;
			}
		}
		if (!(pulsado & 0x01)){ //compruebo si en la fila 1 (entrada) se ha dejado de pulsar (flag bajada)
			if (pulsado_anterior1[control] == 1){
				Send_MIDINoteOff_1(control);
				pulsado_anterior1[control] = 0;
			}
		}
		if (pulsado & 0x02){ //compruebo si en la fila 1 (entrada) se ha pulsado
			if (pulsado_anterior2[control] != 1){
				Send_MIDINoteOn_2WithVolume(control);
				pulsado_anterior2[control] = 1;
			}
		}
		if (!(pulsado & 0x02)){ //compruebo si en la fila 2 (entrada) se ha dejado de pulsar
			if (pulsado_anterior2[control] == 1){
				Send_MIDINoteOff_2(control);
				pulsado_anterior2[control] = 0;
			}
		}
		if (pulsado & 0x04){
			if (pulsado_anterior3[control] != 1){
				ControlsUser(control);
				pulsado_anterior3[control] = 1;
			}
		}
		if (!(pulsado & 0x04)){ //flanco de bajada no hago nada
			if (pulsado_anterior3[control] == 1){
				pulsado_anterior3[control] = 0;
			}
		}
	}
}



uint8_t IdentifyRow(uint8_t control){ //Comprobar la entrada del MUX (2 entradas)
	WriteControl(control);
	// TODO Hal_Delay toma uint32_t, 0.030 es float, comprobar
	HAL_Delay(0.030); //30 o 15 ns, tiempo de propagacion del MUX necesario
	//probar a no poner delay??
	uint8_t pulsado = 0;
	uint8_t pulsado1 = false;
	uint8_t pulsado2 = false;
	uint8_t userControls = false;
	pulsado1 = HAL_GPIO_ReadPin(I1_GPIO_Port, I1_Pin);  //Comprobar si se ha pulsado una nota en la fila 1
	pulsado2 = HAL_GPIO_ReadPin(I2_GPIO_Port, I2_Pin); //Comprobar si se ha pulsado una nota en la fila 2
	userControls = HAL_GPIO_ReadPin(I3_GPIO_Port, I3_Pin);
	pulsado = ((pulsado1 & 0x01) | (pulsado2<<1 & 0x02) | (userControls<<2 & 0x04));
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


void ControlsUser(uint8_t control){
	switch(control)
	{
	case 0:
		volumeDown();
		break;
	case 1:
		volumeUp();
		break;

	case 2:
		vibrato();
		break;
	}
}

void volumeUp() {
	if (volumen < 0x7F) {
		volumen += 0x0A; //Sumo el volumen de 10 en 10
	}
}
void volumeDown() {
	if (volumen > 0X00) {
		volumen -= 0x0A; //Resto el volumen de 10 en 10
	}
}


void vibrato(){
	uint8_t vibratoOn[3] = {0xB0, 0x01, 0x7F}; //MIDI Control Change Modulation wheel (vibrato) al maximo
	uint8_t vibratoOff[3] = {0xB0, 0x01, 0x00};

	vibrato_activo = !vibrato_activo; //Conmutamos cada vez que pulsamos para activar o desactivar vibrato
	if (vibrato_activo) {
		HAL_UART_Transmit(&huart2, (uint8_t*)&vibratoOn, 3, 100);
	} else {
		HAL_UART_Transmit(&huart2, (uint8_t*)&vibratoOff, 3, 100);
	}
}


void Send_MIDINoteOn_1_WithVolume(uint8_t control){ //Enviar código MIDI NoteOn de la fila 1 correspondiente a cada nota en funcion del codigo de control
	//Mensajes de MIDI NoteON: canal MIDI 1, Sol2 a La3

//	uint8_t MIDI_G2[3] = {0x90, 0x2B, volumen};
//	uint8_t MIDI_Gs2[3] = {0x90, 0x2C, volumen};
//	uint8_t MIDI_A2[3] = {0x90, 0x2D, volumen};
//	uint8_t MIDI_As2[3] = {0x90, 0x2E, volumen};
//	uint8_t MIDI_B2[3] = {0x90, 0x2F, volumen};
//	uint8_t MIDI_C3[3] = {0x90, 0x30, volumen};
//	uint8_t MIDI_Cs3[3] = {0x90, 0x31, volumen};
//	uint8_t MIDI_D3[3] = {0x90, 0x32, volumen};
//	uint8_t MIDI_Ds3[3] = {0x90, 0x33, volumen};
//	uint8_t MIDI_E3[3] = {0x90, 0x34, volumen};
//	uint8_t MIDI_F3[3] = {0x90, 0x35, volumen};
//	uint8_t MIDI_Fs3[3] = {0x90, 0x36, volumen};
//	uint8_t MIDI_G3[3] = {0x90, 0x37, volumen};
//	uint8_t MIDI_Gs3[3] = {0x90, 0x38, volumen};
//	uint8_t MIDI_A3[3] = {0x90, 0x39, volumen};

	if (control <= 14){
		uint8_t MIDI[3] = {0x90, 0x2B + control, volumen};
		HAL_UART_Transmit(&huart2, MIDI, 3, 100);
	}
}


void Send_MIDINoteOff_1(uint8_t control){ //Enviar código MIDI NoteOff de la fila 1 correspondiente a cada nota en función del código de control
	//Mensajes de MIDI NoteOff -> velocidad 0

//	uint8_t MIDI_G2[3] = {0x90, 0x2B, volumen};
//	uint8_t MIDI_Gs2[3] = {0x90, 0x2C, volumen};
//	uint8_t MIDI_A2[3] = {0x90, 0x2D, volumen};
//	uint8_t MIDI_As2[3] = {0x90, 0x2E, volumen};
//	uint8_t MIDI_B2[3] = {0x90, 0x2F, volumen};
//	uint8_t MIDI_C3[3] = {0x90, 0x30, volumen};
//	uint8_t MIDI_Cs3[3] = {0x90, 0x31, volumen};
//	uint8_t MIDI_D3[3] = {0x90, 0x32, volumen};
//	uint8_t MIDI_Ds3[3] = {0x90, 0x33, volumen};
//	uint8_t MIDI_E3[3] = {0x90, 0x34, volumen};
//	uint8_t MIDI_F3[3] = {0x90, 0x35, volumen};
//	uint8_t MIDI_Fs3[3] = {0x90, 0x36, volumen};
//	uint8_t MIDI_G3[3] = {0x90, 0x37, volumen};
//	uint8_t MIDI_Gs3[3] = {0x90, 0x38, volumen};
//	uint8_t MIDI_A3[3] = {0x90, 0x39, volumen};

	if (control <= 14){
		uint8_t MIDI[3] = {0x90, 0x2B + control, 0x00};
		HAL_UART_Transmit(&huart2, MIDI, 3, 100);
	}
}

void Send_MIDINoteOn_2WithVolume(uint8_t control){
	//Mensajes de MIDI NoteON: canal MIDI 1, Las3 a Do5

//	uint8_t MIDI_As3[3] = {0x90, 0x3A, 0x00};
//	uint8_t MIDI_B3[3] = {0x90, 0x3B, 0x00};
//	uint8_t MIDI_C4[3] = {0x90, 0x3C, 0x00};
//	uint8_t MIDI_Cs4[3] = {0x90, 0x3D, 0x00};
//	uint8_t MIDI_D4[3] = {0x90, 0x3E, 0x00};
//	uint8_t MIDI_Ds4[3] = {0x90, 0x3F, 0x00};
//	uint8_t MIDI_E4[3] = {0x90, 0x40, 0x00};
//	uint8_t MIDI_F4[3] = {0x90, 0x41, 0x00};
//	uint8_t MIDI_Fs4[3] = {0x90, 0x42, 0x00};
//	uint8_t MIDI_G4[3] = {0x90, 0x43, 0x00};
//	uint8_t MIDI_Gs4[3] = {0x90, 0x44, 0x00};
//	uint8_t MIDI_A4[3] = {0x90, 0x45, 0x00};
//	uint8_t MIDI_As4[3] = {0x90, 0x46, 0x00};
//	uint8_t MIDI_B4[3] = {0x90, 0x47, 0x00};
//	uint8_t MIDI_C5[3] = {0x90, 0x48, 0x00};

	if (control <= 14){
		uint8_t MIDI[3] = {0x90, 0x3A + control, volumen};
		HAL_UART_Transmit(&huart2, MIDI, 3, 100);
	}
}

void Send_MIDINoteOff_2(uint8_t control){
	//Mensajes de MIDI NoteOff -> velocidad 0

//	uint8_t MIDI_As3[3] = {0x90, 0x3A, 0x00};
//	uint8_t MIDI_B3[3] = {0x90, 0x3B, 0x00};
//	uint8_t MIDI_C4[3] = {0x90, 0x3C, 0x00};
//	uint8_t MIDI_Cs4[3] = {0x90, 0x3D, 0x00};
//	uint8_t MIDI_D4[3] = {0x90, 0x3E, 0x00};
//	uint8_t MIDI_Ds4[3] = {0x90, 0x3F, 0x00};
//	uint8_t MIDI_E4[3] = {0x90, 0x40, 0x00};
//	uint8_t MIDI_F4[3] = {0x90, 0x41, 0x00};
//	uint8_t MIDI_Fs4[3] = {0x90, 0x42, 0x00};
//	uint8_t MIDI_G4[3] = {0x90, 0x43, 0x00};
//	uint8_t MIDI_Gs4[3] = {0x90, 0x44, 0x00};
//	uint8_t MIDI_A4[3] = {0x90, 0x45, 0x00};
//	uint8_t MIDI_As4[3] = {0x90, 0x46, 0x00};
//	uint8_t MIDI_B4[3] = {0x90, 0x47, 0x00};
//	uint8_t MIDI_C5[3] = {0x90, 0x48, 0x00};

	if (control <= 14){
		uint8_t MIDI[3] = {0x90, 0x3A + control, 0x00};
		HAL_UART_Transmit(&huart2, MIDI, 3, 100);
	}
}
