/*
 * TP2 : GPIO et liaison série, sans RTOS
 * Seules les sections USER CODE sont reproduites ; le reste est généré par STM32CubeMX.
 */

#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* USER CODE BEGIN 0 */
/* Envoi d'un caractère directement par les registres de l'USART2 */
static void uart2_putc(char c)
{
    while ((USART2->SR & USART_SR_TXE) == 0U)
    {
    }
    USART2->DR = (uint8_t) c;
}
/* printf() est redirigé vers l'USART2 par common/printf_uart2.c */
/* USER CODE END 0 */

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();

    /* USER CODE BEGIN 2 */
    /* Étape 2 : envoi des chiffres 0 à 9 par les registres, puis retour à la ligne */
    for (uint32_t uCount = 0; uCount < 10; uCount++)
    {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        HAL_Delay(500);
        uart2_putc((char) ('0' + uCount));
    }
    uart2_putc('\r');
    uart2_putc('\n');

    uint32_t uCount = 0;
    /* USER CODE END 2 */

    while (1)
    {
        /* USER CODE BEGIN 3 */
        /* Étape 3 : compteur/décompteur affiché avec printf().
         * Le bouton B1 de la Nucleo est actif à l'état bas : relâché = GPIO_PIN_SET. */
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        HAL_Delay(500);
        printf("Compteur/Decompteur : %lu\r\n", (unsigned long) uCount);

        if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_SET)
        {
            uCount++;           /* bouton relâché : on compte */
        }
        else if (uCount > 0)
        {
            uCount--;           /* bouton appuyé : on décompte, sans passer sous zéro */
        }
        /* USER CODE END 3 */
    }
}
