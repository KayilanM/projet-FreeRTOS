/*
 * TP7 : synchronisation de tâches par sémaphore binaire
 * - tache1 attend le sémaphore, bascule la LED et affiche un compteur ;
 * - tache2 et tache3 donnent le sémaphore toutes les 3 s et 4 s ;
 * - tache4 le donne à chaque changement d'état du bouton B1.
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

void tache4(void *p)
{
    GPIO_PinState etatAncien = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
        const GPIO_PinState nouvelEtat = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);

        if (nouvelEtat != etatAncien)
        {
            xSemaphoreGive(semGo);
            etatAncien = nouvelEtat;   /* mémoriser l'état pour ne détecter que les changements */
        }
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
    semGo = xSemaphoreCreateBinary();      /* créé avant les tâches qui l'utilisent */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_THREADS */
    xTaskCreate(tache1, "t1", 512, NULL, 3, NULL);
    xTaskCreate(tache2, "t2", 512, NULL, 3, NULL);
    xTaskCreate(tache3, "t3", 512, NULL, 3, NULL);
    xTaskCreate(tache4, "t4", 512, NULL, 3, NULL);
    /* USER CODE END RTOS_THREADS */

    osKernelStart();   /* l'ordonnanceur prend la main, on ne revient jamais ici */

    while (1)
    {
    }
}
