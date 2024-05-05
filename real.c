#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
  char *name;
  int period;
  int cpuBurst;
  int arrivalOrder;
} Task;

typedef struct TaskExecution {
  Task *task;
  int executionTime;
  int startTime;
  int endTime;
} TaskExecution;

typedef struct Node {
  struct TaskExecution *task;
  struct Node *next;
  struct Node *prev;
} Node;

typedef struct LogNode {
  struct TaskExecution *task;
  char status[2];
  int units;
  struct LogNode *next;
} LogNode;

typedef struct TotalLogNode {
  struct Task *task;
  int totalTasks;
  struct TotalLogNode *next;
} TotalLogNode;

void createTaskExecutionList(Node *headTaskList, Node **headTaskExecutionList,
                             int totalTime);
void insertTask(Node **head, TaskExecution *taskExecution);
void removeTask(Node **head, TaskExecution *taskExecution);
void readTasksFromFile(FILE *file, Node **head, int *totalTime,
                       TotalLogNode **headLost, TotalLogNode **headComplete,
                       TotalLogNode **headKilled);
Node *getTaskToExecute(int currentTime, Node *headTaskExecutionList,
                       int algorithm);
void createLog(LogNode **headLog, TaskExecution *taskExecution, char status[2],
               int units);
void algorithm(Node **headTaskExecutionList, LogNode **headLog, int totalTime,
               TotalLogNode **headLost, TotalLogNode **headComplete,
               TotalLogNode **headKilled, int algorithm);
void printLog(LogNode *head, TotalLogNode *headLost, TotalLogNode *headComplete,
              TotalLogNode *headKilled);
void createTotalLog(TotalLogNode **headLost, TotalLogNode **headComplete,
                    TotalLogNode **headKilled, Task *task);
void incrementTotalLog(TotalLogNode **head, Task *task);
void killRemainingTasks(Node **headTaskExecutionList, TotalLogNode **headKilled,
                        TotalLogNode **headLost, int totalTime);

int main(int argc, char *argv[]) {
  Node *headTaskList = NULL;
  Node *headTaskExecutionList = NULL;
  LogNode *headLog = NULL;
  int totalTime = 0;

  TotalLogNode *headLost = NULL;
  TotalLogNode *headComplete = NULL;
  TotalLogNode *headKilled = NULL;

  FILE *file = fopen(argv[1], "r");

  if (file == NULL) {
    printf("Error opening file\n");
    return 1;
  }

  readTasksFromFile(file, &headTaskList, &totalTime, &headLost, &headComplete,
                    &headKilled);
  createTaskExecutionList(headTaskList, &headTaskExecutionList, totalTime);

  if (strcmp(argv[0], "./rate") == 0) {
    freopen("rate.out", "w", stdout);
    printf("EXECUTION BY RATE");

    printf("\n");

    algorithm(&headTaskExecutionList, &headLog, totalTime, &headLost,
              &headComplete, &headKilled, 0);

    killRemainingTasks(&headTaskExecutionList, &headKilled, &headLost,
                       totalTime);

    printLog(headLog, headLost, headComplete, headKilled);

  } else if (strcmp(argv[0], "./edf") == 0) {
    freopen("edf.out", "w", stdout);
    printf("EXECUTION BY EDF");

    printf("\n");

    algorithm(&headTaskExecutionList, &headLog, totalTime, &headLost,
              &headComplete, &headKilled, 1);

    killRemainingTasks(&headTaskExecutionList, &headKilled, &headLost,
                       totalTime);

    printLog(headLog, headLost, headComplete, headKilled);
  }

  fclose(file);

  return 0;
}

void printLog(LogNode *head, TotalLogNode *headLost, TotalLogNode *headComplete,
              TotalLogNode *headKilled) {
  LogNode *current = head;
  while (current != NULL) {
    if (strcmp(current->status, "I") == 0) {
      printf("idle for %d units\n", current->units);
    } else {
      printf("[%s] for %d units - %s\n", current->task->task->name,
             current->units, current->status);
    }
    current = current->next;
  }

  TotalLogNode *currentLost = headLost;
  printf("\nLOST DEADLINES\n");
  while (currentLost != NULL) {
    printf("[%s] %d\n", currentLost->task->name, currentLost->totalTasks);
    currentLost = currentLost->next;
  }

  TotalLogNode *currentComplete = headComplete;
  printf("\nCOMPLETE EXECUTION\n");
  while (currentComplete != NULL) {
    printf("[%s] %d\n", currentComplete->task->name,
           currentComplete->totalTasks);
    currentComplete = currentComplete->next;
  }

  TotalLogNode *currentKilled = headKilled;
  printf("\nKILLED\n");
  while (currentKilled != NULL) {
    if (currentKilled->next == NULL) {
      printf("[%s] %d", currentKilled->task->name, currentKilled->totalTasks);
    } else {
      printf("[%s] %d\n", currentKilled->task->name, currentKilled->totalTasks);
    }
    currentKilled = currentKilled->next;
  }
}

void incrementTotalLog(TotalLogNode **head, Task *task) {
  TotalLogNode *current = *head;
  while (current != NULL) {
    if (current->task == task) {
      current->totalTasks++;
      return;
    }
    current = current->next;
  }
}

void killRemainingTasks(Node **headTaskExecutionList, TotalLogNode **headKilled,
                        TotalLogNode **headLost, int totalTime) {

  Node *current = *headTaskExecutionList;
  if (current == NULL) {
    return;
  }
  while (current != NULL) {
    Node *nextNode = current->next;
    if (current->task->executionTime >= 0 &&
        current->task->endTime <= totalTime) {
      incrementTotalLog(headLost, current->task->task);
      removeTask(headTaskExecutionList, current->task);
    } else {
      incrementTotalLog(headKilled, current->task->task);
      removeTask(headTaskExecutionList, current->task);
    }
    current = nextNode;
  }
}

void algorithm(Node **headTaskExecutionList, LogNode **headLog, int totalTime,
               TotalLogNode **headLost, TotalLogNode **headComplete,
               TotalLogNode **headKilled, int algorithm) {
  int currentTime = 0;
  int idleTime = 0;
  int totalExecutionTime = 0;
  Node *currentTask = NULL;

  while (currentTime < totalTime) {
    Node *taskToExecute =
        getTaskToExecute(currentTime, *headTaskExecutionList, algorithm);

    if (taskToExecute != NULL) {
      if (currentTask != NULL) {
        if (currentTask->task->executionTime >=
            currentTask->task->task->cpuBurst) {
          createLog(headLog, currentTask->task, "F", totalExecutionTime);
          incrementTotalLog(headComplete, currentTask->task->task);
          removeTask(headTaskExecutionList, currentTask->task);
          currentTask = NULL;
          totalExecutionTime = 0;

          continue;
        } else if (currentTask->task->endTime == currentTime &&
                   currentTask->task->executionTime <
                       taskToExecute->task->task->cpuBurst) {
          if (totalExecutionTime > 0) {
            createLog(headLog, currentTask->task, "L", totalExecutionTime);
          }
          incrementTotalLog(headLost, currentTask->task->task);
          removeTask(headTaskExecutionList, currentTask->task);
          totalExecutionTime = 0;
          currentTask = NULL;
          continue;
        } else if (currentTask != taskToExecute) {
          if (currentTask->task->executionTime <
              currentTask->task->task->cpuBurst) {
            createLog(headLog, currentTask->task, "H", totalExecutionTime);
            totalExecutionTime = 0;
          }
        } else {
          totalExecutionTime++;
          currentTask = taskToExecute;

          if (idleTime > 0) {
            createLog(headLog, NULL, "I", idleTime);
            idleTime = 0;
          }

          taskToExecute->task->executionTime++;
          currentTime++;

          if (currentTask->task->endTime == currentTime &&
              taskToExecute->task->executionTime <
                  taskToExecute->task->task->cpuBurst) {
            Node *nextInLine = getTaskToExecute(
                currentTime, *headTaskExecutionList, algorithm);

            if (nextInLine != NULL && nextInLine != taskToExecute) {
              createLog(headLog, taskToExecute->task, "H", totalExecutionTime);
              totalExecutionTime = 0;
            } else if (totalExecutionTime > 0) {
              createLog(headLog, currentTask->task, "L", totalExecutionTime);
            }
            incrementTotalLog(headLost, currentTask->task->task);
            removeTask(headTaskExecutionList, currentTask->task);
            totalExecutionTime = 0;
            currentTask = NULL;
            continue;
          }

          if (currentTime == totalTime &&
              taskToExecute->task->executionTime <
                  taskToExecute->task->task->cpuBurst) {
            Node *nextInLine = getTaskToExecute(
                currentTime, *headTaskExecutionList, algorithm);

            if (nextInLine != NULL && nextInLine != taskToExecute) {
              createLog(headLog, taskToExecute->task, "H", totalExecutionTime);
              totalExecutionTime = 0;
            } else {
              createLog(headLog, taskToExecute->task, "K", totalExecutionTime);
            }

            incrementTotalLog(headKilled, taskToExecute->task->task);
            removeTask(headTaskExecutionList, taskToExecute->task);
            totalExecutionTime = 0;
            continue;
          }

          if (currentTime == totalTime &&
              taskToExecute->task->executionTime ==
                  taskToExecute->task->task->cpuBurst) {
            createLog(headLog, taskToExecute->task, "F", totalExecutionTime);
            incrementTotalLog(headComplete, taskToExecute->task->task);
            removeTask(headTaskExecutionList, taskToExecute->task);
            totalExecutionTime = 0;
          }
        }
      }

      currentTask = taskToExecute;
    } else {
      currentTask = NULL;
      idleTime++;
      currentTime++;

      if (currentTime == totalTime) {
        createLog(headLog, NULL, "I", idleTime);
      }
    }
  }
}

Node *getTaskToExecute(int currentTime, Node *headTaskExecutionList,
                       int algorithm) {
  Node *current = headTaskExecutionList;
  Node *taskToExecute = NULL;
  while (current != NULL) {
    if (algorithm == 0) {
      if (current->task->startTime <= currentTime &&
          current->task->endTime >= currentTime) {
        if (taskToExecute == NULL) {
          taskToExecute = current;
        } else {
          if (current->task->task->period < taskToExecute->task->task->period) {
            taskToExecute = current;
          } else if (current->task->task->period ==
                     taskToExecute->task->task->period) {
            if (current->task->task->arrivalOrder <
                taskToExecute->task->task->arrivalOrder) {
              taskToExecute = current;
            }
          }
        }
      }

    } else {
      if (current->task->startTime <= currentTime &&
          current->task->endTime >= currentTime) {
        if (taskToExecute == NULL) {
          taskToExecute = current;
        } else {
          if (current->task->endTime < taskToExecute->task->endTime) {
            taskToExecute = current;
          } else if (current->task->endTime == taskToExecute->task->endTime) {
            if (current->task->startTime < taskToExecute->task->startTime) {
              taskToExecute = current;
            }
          }
        }
      }
    }

    current = current->next;
  }
  return taskToExecute;
}

void createLog(LogNode **headLog, TaskExecution *taskExecution, char status[2],
               int units) {
  LogNode *newNode = (LogNode *)malloc(sizeof(LogNode));
  newNode->task = taskExecution;
  strcpy(newNode->status, status);
  newNode->units = units;
  newNode->next = NULL;

  if (*headLog == NULL) {
    *headLog = newNode;
  } else {
    LogNode *current = *headLog;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = newNode;
  }
}

void createTaskExecutionList(Node *headTaskList, Node **headTaskExecutionList,
                             int totalTime) {
  Node *current = headTaskList;
  while (current != NULL) {
    int period = current->task->task->period;
    int numberOfExecutions = totalTime / period;
    int rest = totalTime % period;
    if (rest > 0) {
      numberOfExecutions++;
    }
    for (int i = 0; i < numberOfExecutions; i++) {
      TaskExecution *taskExecution =
          (TaskExecution *)malloc(sizeof(TaskExecution));
      taskExecution->task = current->task->task;
      taskExecution->executionTime = 0;
      taskExecution->startTime = period * i;
      taskExecution->endTime = period * (i + 1);
      insertTask(headTaskExecutionList, taskExecution);
    }

    if (rest == 0) {

      TaskExecution *taskExecution =
          (TaskExecution *)malloc(sizeof(TaskExecution));
      taskExecution->task = current->task->task;
      taskExecution->executionTime = 0;
      taskExecution->startTime = period * numberOfExecutions;
      taskExecution->endTime = period * (numberOfExecutions + 1);
      insertTask(headTaskExecutionList, taskExecution);
    }
    current = current->next;
  }
}

void insertTask(Node **head, TaskExecution *taskExecution) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  newNode->task = taskExecution;
  newNode->next = NULL;
  newNode->prev = NULL;

  if (*head == NULL) {
    *head = newNode;
    return;
  }

  Node *current = *head;
  while (current->next != NULL) {
    current = current->next;
  }

  current->next = newNode;
  newNode->prev = current;
}

void removeTask(Node **head, TaskExecution *taskExecution) {
  Node *current = *head;
  while (current != NULL) {
    if (current->task == taskExecution) {
      if (current->prev == NULL) {
        *head = current->next;
      } else {
        current->prev->next = current->next;
      }

      if (current->next != NULL) {
        current->next->prev = current->prev;
      }

      free(current);
      return;
    }
    current = current->next;
  }
}

void createTotalLog(TotalLogNode **headLost, TotalLogNode **headComplete,
                    TotalLogNode **headKilled, Task *task) {

  TotalLogNode *newNodeLost = (TotalLogNode *)malloc(sizeof(TotalLogNode));
  TotalLogNode *newNodeComplete = (TotalLogNode *)malloc(sizeof(TotalLogNode));
  TotalLogNode *newNodeKilled = (TotalLogNode *)malloc(sizeof(TotalLogNode));

  newNodeLost->task = task;
  newNodeLost->totalTasks = 0;
  newNodeLost->next = NULL;

  newNodeComplete->task = task;
  newNodeComplete->totalTasks = 0;
  newNodeComplete->next = NULL;

  newNodeKilled->task = task;
  newNodeKilled->totalTasks = 0;
  newNodeKilled->next = NULL;

  if (*headLost == NULL) {
    *headLost = newNodeLost;
  } else {
    TotalLogNode *current = *headLost;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = newNodeLost;
  }

  if (*headComplete == NULL) {
    *headComplete = newNodeComplete;
  } else {
    TotalLogNode *current = *headComplete;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = newNodeComplete;
  }

  if (*headKilled == NULL) {
    *headKilled = newNodeKilled;
  } else {
    TotalLogNode *current = *headKilled;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = newNodeKilled;
  }
}

void readTasksFromFile(FILE *file, Node **head, int *totalTime,
                       TotalLogNode **headLost, TotalLogNode **headComplete,
                       TotalLogNode **headKilled) {
    char ch;
    char *line = (char *)malloc(sizeof(char));
    int len = 0;
    int arrivalOrder = 0;

    while ((ch = fgetc(file)) != '\n') {
        line[len++] = ch;
        line = realloc(line, len + 1);
    }
    line[len] = '\0';
    *totalTime = atoi(line);
    free(line);

    while ((ch = fgetc(file)) != EOF) {
        len = 0;
        line = (char *)malloc(sizeof(char));
        while (ch != '\n' && ch != EOF) {
            line[len++] = ch;
            line = realloc(line, len + 1);
            ch = fgetc(file);
        }
        line[len] = '\0';

        char *name = strtok(line, " ");
        char *periodStr = strtok(NULL, " ");
        char *cpuBurstStr = strtok(NULL, " ");

        Task *task = (Task *)malloc(sizeof(Task));
        task->name = strdup(name);
        task->period = atoi(periodStr);
        task->cpuBurst = atoi(cpuBurstStr);
        task->arrivalOrder = arrivalOrder++;

        TaskExecution *taskExecution =
            (TaskExecution *)malloc(sizeof(TaskExecution));
        taskExecution->task = task;
        taskExecution->executionTime = 0;
        taskExecution->startTime = 0;
        taskExecution->endTime = 0;

        insertTask(head, taskExecution);
        createTotalLog(headLost, headComplete, headKilled, task);

        free(line);
        if (ch == EOF) {
            break;
        }
    }
}