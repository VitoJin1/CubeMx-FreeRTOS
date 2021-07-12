#include "taskstatedisplay.h"

volatile unsigned long long FreeRTOSRunTimeTicks;
void task_state_display(void *pvParameters){
    #if GETTASKSTATE
    char MessageToShow[400];
    #endif
    #if GETSYSTEMSTATE
    uint32_t TotalRunTime;
	UBaseType_t ArraySize,x;
	TaskStatus_t *StatusArray;
    #endif

    #if GETTASKINFO
    TaskHandle_t TaskHandle;
    TaskStatus_t TaskStatus;
    #endif
    
    #if GETTASKRUNTIME
    char RunTimeInfo[400];
    #endif
    
    for( ; ; ) {
        #if GETSYSTEMSTATE
        printf("Get System State\r\n");
        ArraySize  = uxTaskGetNumberOfTasks();
        StatusArray= pvPortMalloc(ArraySize*sizeof(TaskStatus_t));
        if(StatusArray!=NULL){
            ArraySize=uxTaskGetSystemState((TaskStatus_t* 	)StatusArray,
                                    (UBaseType_t		)ArraySize, 	
                                    (uint32_t*		)&TotalRunTime);
            printf("TaskName\t\tPriority\t\tTaskNumber\t\t\r\n");
            for(x=0;x<ArraySize;x++){
                printf("%s\t\t%d\t\t\t%d\t\t\t\r\n",				
                        StatusArray[x].pcTaskName,
                        (int)StatusArray[x].uxCurrentPriority,
                        (int)StatusArray[x].xTaskNumber);
            }
        }
        vPortFree(StatusArray);//release memory
        #endif

        #if GETTASKINFO
        printf("/***********show task info***************/\r\n");
        TaskHandle=xTaskGetHandle("start_task");	//source taskhandle by task name
	    vTaskGetInfo((TaskHandle_t	) TaskHandle,   
                     (TaskStatus_t *)&TaskStatus,   
				     (BaseType_t	)     pdTRUE,	
			         (eTaskState	)  eInvalid);   
        printf("taskname:              %s\r\n",TaskStatus.pcTaskName);
	    printf("number:                %d\r\n",(int)TaskStatus.xTaskNumber);
	    printf("tasktatus:             %d\r\n",TaskStatus.eCurrentState);
	    printf("taskpiroty:            %d\r\n",(int)TaskStatus.uxCurrentPriority);
	    printf("base piroty:           %d\r\n",(int)TaskStatus.uxBasePriority);
	    printf("task baseaddr:         %#x\r\n",(int)TaskStatus.pxStackBase);
	    printf("historical minmum satck:%d\r\n",TaskStatus.usStackHighWaterMark);
        printf("/*******************end********************/\r\n");
        #endif
        #if GETTASKSTATE
        printf("/*************all tasks state show***********/\r\n");
	    vTaskList(MessageToShow);							
	    printf("%s\r\n",MessageToShow);					
	    printf("/*********************end************************/\r\n");
        #endif
        #if GETTASKRUNTIME
        memset(RunTimeInfo,0,400);
        vTaskGetRunTimeStats(RunTimeInfo);
        printf("name  runtime   percentage  \r\n");
        printf("%s\r\n",RunTimeInfo);
        #endif
        vTaskDelay(500);
    }

}

