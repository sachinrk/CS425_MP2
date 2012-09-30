#include "ui.h"
void main()/*interact_with_user()*/
{
    char choice = 0;
    int valid = 0;
    char buf[100];
    char display_buf[600];
    char dislay_buf2[300];
    char user_input[10]; 
    returnCode (*case1function)();    
    state_machine current_state = INIT;   
    do {
       system("clear");
       printf("\n***************************\nGroups-RUs Inc\nNode Membership Interface\n***************************\n\n");
       buf[0] = 0;
       display_buf[0] = 0;
       //printf("Current Membership Status : ");
       switch(current_state) {
           case INIT:
               strcpy(buf,"Not a member");
               strcpy(display_buf,"\n1. Send Membership Join Request\n");
               break;
           case JOIN_REQUEST_SENT:
               strcpy(buf, "Join request sent to admission contact");
               break;
           case TOPOLOGY_FORMED:
               strcpy(buf, "Membership List Received. Updating other nodes");
               break;
           case TOPOLOGY_FORMED_NEIGHBOURS_UPDATED:
               strcpy(buf, "Membership Active");
               strcpy(display_buf,"\n1. Send Membership Quit notification\n");   
               break;
       }
       strcat(display_buf, "2. Display membership list\n3. Quit");
       printf("Current membership Status : %s\n\n%s\n\n", buf, display_buf);
       valid = 0;
       do {
           printf("\nEnter choice : "); 
           user_input[0] = 0;
           gets(user_input);
           choice = user_input[0];
           //printf("\nUser Input : %0x\n", choice);
           valid = 1;
           switch(choice - '0') {
                   case 1: 
                       (*case1function)();
                       break;
                   case 2:
                        //display_membership_list();
                        break;
                   case 3:
                        exit(0);
                        break;
                   default:
                        printf("\nInvalid entry. Please try again\n");
                        valid = 0;
                        break;

          }
            
       }while(valid == 0);
       system("clear"); 
    }while((choice-'0') != 3);
}
