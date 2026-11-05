# Programmation temps réel avec FreeRTOS sur STM32

Série de travaux pratiques réalisés en BUT GEII (2024–2025) sur une carte **STM32 Nucleo**,
en C avec **STM32CubeIDE** et **FreeRTOS**. Les TP suivent une progression : d'abord un programme
sans système d'exploitation, puis la création de tâches, les problèmes de partage de ressources
et les mécanismes de synchronisation et de communication entre tâches.

## Contenu

| Dossier | Notion travaillée |
|---|---|
| `tp02-gpio-uart` | GPIO (LED, bouton) et liaison série, sans RTOS ; redirection de `printf` |
| `tp03-premieres-taches` | Création de tâches, `osDelay`, exécution concurrente |
| `tp04-ressource-partagee` | Mise en évidence d'un conflit d'accès à la liaison série partagée |
| `tp05-taches-concurrentes` | API native FreeRTOS (`xTaskCreate`, `vTaskDelay`), scrutation d'un bouton |
| `tp06-mutex` | Protection de la liaison série partagée par un mutex |
| `tp07-semaphore` | Synchronisation de tâches par sémaphore binaire |
| `tp08-semaphore-interruption` | Sémaphore donné depuis une interruption (`xSemaphoreGiveFromISR`) |
| `tp09-event-groups` | Attente de plusieurs événements avec un groupe d'événements |
| `tp10-file-de-messages` | Communication producteur/consommateur par file de messages |
| `common/printf_uart2.c` | Redirection de `printf` vers l'USART2, partagée par les TP |

Chaque fichier `main.c` contient le code que j'ai écrit dans les sections `USER CODE`
du projet généré par STM32CubeMX. Le code d'initialisation généré (horloges, GPIO, UART)
n'est pas reproduit.

## Exemple : protection d'une ressource partagée par mutex (TP6)

Deux tâches écrivent sur la même liaison série. Sans protection, une tâche peut être
interrompue au milieu d'une ligne et les caractères se mélangent. La fonction d'envoi
prend un mutex avant d'écrire et le rend à la fin : une seule tâche à la fois accède
à la liaison série.

```c
if (protege)
{
    xSemaphoreTake(mxProtectionPrintf, portMAX_DELAY);
}
for (int i = 0; i < len; i++)
{
    while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) != SET) { }
    huart2.Instance->DR = (uint8_t) ptr[i];
}
/* ... attente de la fin de transmission ... */
if (protege)
{
    xSemaphoreGive(mxProtectionPrintf);
}
```

## Résultats observés

- **TP4** : avec deux tâches qui écrivent sur la même liaison série à 500 ms et 90 ms,
  l'affichage devient irrégulier. La liaison série est une ressource partagée.
- **TP6** : avec des lignes plus longues, les caractères des deux tâches se mélangent au milieu
  d'une ligne. Donner une priorité plus haute à l'une des tâches ne règle pas le problème,
  puisqu'elle continue d'interrompre l'autre. Avec le mutex, chaque ligne s'affiche complète.
- **TP7** : un sémaphore binaire ne mémorise qu'un seul « jeton » : si plusieurs tâches le
  donnent avant que la tâche 1 ne le prenne, les dons supplémentaires sont perdus.
- **TP10** : la tâche consommatrice affiche la moyenne de chaque lot de 20 valeurs reçues
  (entre 38 et 55 lors des essais, pour des valeurs tirées entre 0 et 99 et entre 0 et 90).

## Reproduire

1. Créer un projet STM32CubeIDE pour la carte Nucleo, avec FreeRTOS (CMSIS_V2) et l'USART2 activés.
2. Copier le contenu des sections `USER CODE` du TP voulu dans `Core/Src/main.c`.
3. Ajouter `common/printf_uart2.c` au projet pour les TP qui utilisent `printf`, sauf pour
   le TP6 qui a sa propre version protégée par mutex. Pour appeler `printf` depuis plusieurs
   tâches, activer aussi `configUSE_NEWLIB_REENTRANT` dans la configuration de FreeRTOS.
4. Pour le TP8, activer l'interruption EXTI du bouton B1 avec une priorité numériquement
   supérieure ou égale à `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` (5 par défaut),
   condition pour appeler une fonction FreeRTOS depuis une interruption.
5. Pour le TP10, cocher l'option *Use float with printf* (affichage de `%f`).
6. Ouvrir un terminal série (PuTTY, 115200 bauds) sur le port du ST-Link.

## Revue du code

Le code a été relu et amélioré après les TP : conditions de boucle, fin propre des tâches,
gestion des interruptions, création des objets FreeRTOS avant les tâches qui les utilisent
et protection de `printf` par mutex.

## Auteurs

Kayilan Mmadi Moilime. Les TP ont été réalisés en binôme.
