/*
 * TP3 : premières tâches FreeRTOS (interface CMSIS-RTOS v2 générée par CubeMX)
 * Deux tâches : la LED clignote (100 ms allumée, 500 ms éteinte) pendant qu'une
 * autre tâche affiche un compteur toutes les 300 ms.
 */

#include "main.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Tâche par défaut : clignotement de la LED LD2 */
void StartDefaultTask(void *argument)
{
    /* USER CODE BEGIN 5 */
    for (;;)
    {
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
        osDelay(100);
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
        osDelay(500);
    }
    /* USER CODE END 5 */
}

/* Deuxième tâche : affichage d'un compteur sur la liaison série */
void StartTaskPrint(void *argument)
{
    /* USER CODE BEGIN StartTaskPrint */
    uint32_t compteur = 0;

    for (;;)
    {
        printf("compteur : %lu\r\n", (unsigned long) compteur);
        compteur++;
        osDelay(300);
    }
    /* USER CODE END StartTaskPrint */
}
