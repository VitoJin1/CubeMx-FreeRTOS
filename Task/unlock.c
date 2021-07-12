#include "unlock.h"
uint8_t RobotUnlockFlag=0;
void unlockRobotDectect(void *pvParameters){
    static uint8_t RobotUnlockSignalCnt=0;
    
    for(;;){
        uint8_t checkforunlock=CheckForUnlock();
        if( (RemoteCommand.Left_X_Speed>0.9f)&&(RemoteCommand.Right_X_Speed<-0.9f)&&checkforunlock==1)
        RobotUnlockSignalCnt++;
        else
        RobotUnlockSignalCnt=0;
        if(RobotUnlockSignalCnt>=15){
            RobotUnlockFlag=1;
            if(Start_Task_Handler!=NULL)
            xTaskNotifyGive(Start_Task_Handler);
        }
        vTaskDelay(200);
    }
}
uint8_t CheckForUnlock(void){
    if(RemoteCommand.screw_switch==0&&RemoteCommand.mode_switch==MANUAL_MODE&&RemoteCommand.balance_switch==0&&\
    RemoteCommand.valve_switch==0&&RemoteCommand.Left_Prop_Speed==0&&RemoteCommand.Right_Prop_Speed==0&&\
    RemoteCommand.Max_Speed==0&&RemoteCommand.Roller_Speed==0&&RemoteCommand.pose_switch==0&&RemoteCommand.pump_switch==0&&\
    RemoteCommand.acro_switch==0)
    {return 1;}
    else 
    {return 0;}
}