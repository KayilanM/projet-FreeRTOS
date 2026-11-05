/*
 * TP6 : protection d'une ressource partagée par mutex
 * Deux tâches écrivent de longues lignes sur la même liaison série (toutes les 500 ms
 * et toutes les 90 ms). Sans protection, une tâche peut être interrompue au milieu
 * d'une ligne et les caractères des deux tâches se mélangent. Donner une priorité
 * plus haute à l'une des tâches ne suffit pas : elle interrompt toujours l'autre.
 * Le mutex garantit qu'une seule tâche à la fois utilise la liaison série.
 */

#include "main.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "semphr.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
SemaphoreHandle_t mxProtectionPrintf;
/* USER CODE END PV */

/* USER CODE BEGIN 0 */
/* printf() redirigé vers l'USART2, protégé par le mutex */
int _write(int file, char *ptr, int len)
{
    (void) file;

    /* Le mutex n'est utilisable qu'une fois créé et l'ordonnanceur démarré */
    const int protege = (mxProtectionPrintf != NULL)
                     && (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING);

    if (protege)
    {
        xSemaphoreTake(mxProtectionPrintf, portMAX_DELAY);
    }

    for (int i = 0; i < len; i++)
    {
        while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) != SET)
        {
        }
        huart2.Instance->DR = (uint8_t) ptr[i];
    }
    while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) != SET)
    {
    }

    if (protege)
    {
        xSemaphoreGive(mxProtectionPrintf);
    }
    return len;
}

void tache1(void *p)
{
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(500));
        printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\r\n");
    }
}

void tache2(void *p)
{
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(90));
        printf("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\r\n");
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

    /* USER CODE BEGIN RTOS_MUTEX */
    mxProtectionPrintf = xSemaphoreCreateMutex();   /* créé avant les tâches qui l'utilisent */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_THREADS */
    xTaskCreate(tache1, "t1", 512, NULL, 3, NULL);
    xTaskCreate(tache2, "t2", 512, NULL, 3, NULL);
    /* USER CODE END RTOS_THREADS */

    osKernelStart();   /* l'ordonnanceur prend la main, on ne revient jamais ici */

    while (1)
    {
    }
}
