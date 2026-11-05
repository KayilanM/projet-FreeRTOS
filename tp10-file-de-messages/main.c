/*
 * TP10 : communication entre tâches par file de messages (queue)
 * tache2 et tache3 produisent des valeurs pseudo-aléatoires (0-99 et 0-90)
 * et les envoient dans une file ; tache1 les consomme et affiche la moyenne
 * de chaque lot de 20 valeurs.
 */

#include "main.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "queue.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PTD */
typedef struct
{
    TickType_t xtTimeStamp;   /* date de production de la mesure (en ticks) */
    double     data;          /* valeur transmise */
} MesDonnees_t;
/* USER CODE END PTD */

/* USER CODE BEGIN PD */
#define TAILLE_LOT  20
/* USER CODE END PD */

/* USER CODE BEGIN PV */
QueueHandle_t queue1;
/* USER CODE END PV */

/* USER CODE BEGIN 0 */
/* Consommateur : moyenne glissante par lots de TAILLE_LOT valeurs */
void tache1(void *p)
{
    MesDonnees_t m;
    double cumul = 0;
    int compteur = 0;

    while (1)
    {
        xQueueReceive(queue1, &m, portMAX_DELAY);
        compteur++;
        cumul += m.data;

        if (compteur == TAILLE_LOT)
        {
            printf("la moyenne calculee est de %f\r\n", cumul / TAILLE_LOT);
            cumul = 0;
            compteur = 0;
        }
    }
}

/* Producteur 1 : valeur pseudo-aléatoire entre 0 et 99 */
void tache2(void *p)
{
    MesDonnees_t m;

    while (1)
    {
        const TickType_t xtDelayAlea100 = (xTaskGetTickCount()
                + HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) * (xTaskGetTickCount() % 77)) % 100;
        vTaskDelay(pdMS_TO_TICKS(200) + xtDelayAlea100);

        m.xtTimeStamp = xTaskGetTickCount();
        m.data = xtDelayAlea100;
        xQueueSend(queue1, &m, portMAX_DELAY);
    }
}

/* Producteur 2 : valeur pseudo-aléatoire entre 0 et 90 */
void tache3(void *p)
{
    MesDonnees_t m;

    while (1)
    {
        const TickType_t xtDelayAlea90 = (xTaskGetTickCount()
                + HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) * (xTaskGetTickCount() % 77)) % 91;
        vTaskDelay(pdMS_TO_TICKS(200) + xtDelayAlea90);

        m.xtTimeStamp = xTaskGetTickCount();
        m.data = xtDelayAlea90;
        xQueueSend(queue1, &m, portMAX_DELAY);
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

    /* USER CODE BEGIN RTOS_QUEUES */
    queue1 = xQueueCreate(20, sizeof(MesDonnees_t));   /* créée avant les tâches qui l'utilisent */
    /* USER CODE END RTOS_QUEUES */

    /* USER CODE BEGIN RTOS_THREADS */
    xTaskCreate(tache1, "t1", 512, NULL, 2, NULL);
    xTaskCreate(tache2, "t2", 512, NULL, 2, NULL);
    xTaskCreate(tache3, "t3", 512, NULL, 2, NULL);
    /* USER CODE END RTOS_THREADS */

    osKernelStart();   /* l'ordonnanceur prend la main, on ne revient jamais ici */

    while (1)
    {
    }
}
