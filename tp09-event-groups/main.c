/*
 * TP9 : synchronisation par groupe d'événements (event group)
 * tache1 se réveille dès qu'un des trois bits est positionné :
 * BIT_0 par tache2 (3 s), BIT_1 par tache3 (4 s), BIT_2 par tache4 (bouton B1).
 * La valeur affichée entre crochets indique quel(s) bit(s) l'ont réveillée.
 */

#include "main.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "event_groups.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PD */
#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)
/* USER CODE END PD */

/* USER CODE BEGIN PV */
EventGroupHandle_t groupe;
/* USER CODE END PV */

/* USER CODE BEGIN 0 */
void tache1(void *p)
{
    int compteur = 0;

    while (1)
    {
        /* pdTRUE : les bits sont effacés en sortie ; pdFALSE : un seul bit suffit */
        EventBits_t v = xEventGroupWaitBits(groupe, BIT_0 | BIT_1 | BIT_2,
                                            pdTRUE, pdFALSE, portMAX_DELAY);
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        printf("[%lu] la valeur du compteur est de : %d [%u]\r\n",
               (unsigned long) xTaskGetTickCount(), compteur, (unsigned) v);
        compteur++;
    }
}

void tache2(void *p)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(3000));
        xEventGroupSetBits(groupe, BIT_0);
    }
}

void tache3(void *p)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(4000));
        xEventGroupSetBits(groupe, BIT_1);
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
            xEventGroupSetBits(groupe, BIT_2);
            etatAncien = nouvelEtat;
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

    /* USER CODE BEGIN RTOS_THREADS */
    groupe = xEventGroupCreate();
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
