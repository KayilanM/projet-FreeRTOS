/*
 * TP8 : sémaphore donné depuis une interruption
 * Le bouton B1 ne passe plus par une tâche de scrutation : son interruption EXTI
 * donne directement le sémaphore à tache1, qui réagit immédiatement.
 */

#include "main.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "semphr.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
SemaphoreHandle_t semGo;
/* USER CODE END PV */

/* USER CODE BEGIN 0 */
void tache1(void *p)
{
    int compteur = 0;

    while (1)
    {
        xSemaphoreTake(semGo, portMAX_DELAY);
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        printf("[%lu] la valeur du compteur est de : %d\r\n",
               (unsigned long) xTaskGetTickCount(), compteur);
        compteur++;
    }
}

void tache2(void *p)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(3000));
        xSemaphoreGive(semGo);
    }
}

void tache3(void *p)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(4000));
        xSemaphoreGive(semGo);
    }
}

/* Appelée par le HAL à chaque interruption EXTI du bouton B1 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == B1_Pin && semGo != NULL)
    {
        /* Doit être initialisé à pdFALSE : FreeRTOS ne le met à pdTRUE que si
         * une tâche plus prioritaire a été réveillée par le sémaphore. */
        BaseType_t xTacheReveillee = pdFALSE;

        xSemaphoreGiveFromISR(semGo, &xTacheReveillee);
        portYIELD_FROM_ISR(xTacheReveillee);
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

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    semGo = xSemaphoreCreateBinary();
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_THREADS */
    xTaskCreate(tache1, "t1", 512, NULL, 3, NULL);
    xTaskCreate(tache2, "t2", 512, NULL, 3, NULL);
    xTaskCreate(tache3, "t3", 512, NULL, 3, NULL);
    /* USER CODE END RTOS_THREADS */

    osKernelStart();   /* l'ordonnanceur prend la main, on ne revient jamais ici */

    while (1)
    {
    }
}
