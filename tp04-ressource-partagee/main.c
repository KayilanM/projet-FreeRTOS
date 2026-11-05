/*
 * TP4 : mise en évidence d'un problème de partage de ressource
 * Deux tâches écrivent sur la même liaison série sans protection.
 * Avec des périodes de 500 ms et 90 ms, l'affichage devient irrégulier :
 * la liaison série est une ressource partagée qu'il faut protéger (par un mutex).
 */

#include "main.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

#define NB_AFFICHAGES  40

/* Tâche 1 : affiche "X" toutes les 500 ms */
void StartDefaultTask(void *argument)
{
    /* USER CODE BEGIN 5 */
    for (int i = 0; i < NB_AFFICHAGES; i++)
    {
        osDelay(500);
        printf("X\r\n");
    }
    /* Une tâche FreeRTOS ne doit jamais se terminer par un simple return :
     * elle se supprime elle-même une fois son travail fini. */
    osThreadExit();
    /* USER CODE END 5 */
}

/* Tâche 2 : affiche "O" toutes les 100 ms (puis 90 ms pour la 2e expérience) */
void StartTaskPrint(void *argument)
{
    /* USER CODE BEGIN StartTaskPrint */
    for (int j = 0; j < NB_AFFICHAGES; j++)
    {
        osDelay(100);
        printf("O\r\n");
    }
    osThreadExit();
    /* USER CODE END StartTaskPrint */
}
