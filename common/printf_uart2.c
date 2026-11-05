/*
 * printf_uart2.c
 * Redirige printf() vers l'USART2, reliée au port série virtuel du ST-Link.
 * Utilisé par tous les TP qui affichent des messages dans un terminal (PuTTY, 115200 bauds).
 */
#include "main.h"

extern UART_HandleTypeDef huart2;

int _write(int file, char *ptr, int len)
{
    (void) file;

    for (int i = 0; i < len; i++)
    {
        /* Attendre que le registre d'émission soit vide avant d'écrire l'octet suivant */
        while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) != SET)
        {
        }
        huart2.Instance->DR = (uint8_t) ptr[i];
    }

    /* Attendre la fin de la transmission du dernier octet */
    while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) != SET)
    {
    }
    return len;
}
