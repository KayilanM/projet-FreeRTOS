/*
 * TP5 : deux tâches créées avec l'API native FreeRTOS (xTaskCreate)
 * - taskLed bascule la LED toutes les 2 s ;
 * - taskInput surveille le bouton B1 toutes les 100 ms et bascule la LED à chaque changement d'état.
 */

#include "main.h"
#include "cmsis_os.h"

/* USER CODE BEGIN 0 */
void taskInput(void *p)
{
    GPIO_PinState etatAncien = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
        const GPIO_PinState nouvelEtat = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);

        if (nouvelEtat != etatAncien)
        {
            HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
            etatAncien = nouvelEtat;
        }
    }
}

void taskLed(void *p)
{
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(2000));
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    }
}
/* USER CODE END 0 */

int main(void)
{
    /* Initialisations générées par CubeMX */
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();

    osKernelInitialize();

    /* USER CODE BEGIN RTOS_THREADS */
    xTaskCreate(taskLed,   "led", 512, NULL, 3, NULL);
    xTaskCreate(taskInput, "inp", 512, NULL, 3, NULL);
    /* USER CODE END RTOS_THREADS */

    osKernelStart();   /* l'ordonnanceur prend la main, on ne revient jamais ici */

    while (1)
    {
    }
}
