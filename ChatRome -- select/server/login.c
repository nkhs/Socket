/*******************************************************************************
* The server processes the user basic operation processing implementation file
* 2015-12-14 yrr achieve
*
********************************************************************************/

#include "config.h"

/*Declare global variables -- Online user list*/
extern ListNode *userList;

/**************************************************
Function name：loginUser
Features：User login function implementation
Parameter：msg--Login message sent by the user sockfd-socket descriptor
Return value：Successful login back SUCCESS, Otherwise return the exception type
****************************************************/
int loginUser(Message *msg , int sockfd)
{
	int ret;
	/*Declare user information*/
	User user;
	char buf[MAX_LINE];
	
	/*Declare database variables*/
	sqlite3 *db;
	sqlite3_stmt *stmt;
	const char *tail;

	/*statement sql Statement storage variable*/
	char sql[128];

	/*Store operation result message*/
	Message message;

	/*接收用户信息*/
	recv(sockfd , buf , sizeof(user) , 0);
	memset(&user , 0 , sizeof(user));
	memcpy(&user , buf , sizeof(buf));
	user.userAddr = (*msg).sendAddr;
	user.sockfd = sockfd;

	/*View online user list, whether the user is online*/
	if(isOnLine(userList , &user) == 1)
		return ALREADY_ONLINE;

	/*（1）Open the database*/
	ret = sqlite3_open(DB_NAME, &db);
	if(ret != SQLITE_OK)
	{
		printf("unable open database.\n");
		return FAILED;
	}//if

	/*（2）Check login username and password*/
	memset(sql , 0 , sizeof(sql));
	sprintf(sql , "select * from User where userName='%s' and password='%s';",user.userName , user.password);
	
	ret = sqlite3_prepare(db , sql , strlen(sql) , &stmt , &tail);	
	if(ret != SQLITE_OK)
	{
		ret = sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		printf("database select fail!\n");
		return FAILED;		
	}//if
	/*carried out*/
	ret = sqlite3_step(stmt);
	//Return if there is data SQLITE_ROW，Return when reaching the end SQLITE_DONE
	while(ret == SQLITE_ROW)
	{
		ret = sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		ret = SUCCESS;
		/*If the login operation is successful, add to the online user list*/
		userList = insertNode(userList , &user);
		return ret;
	}//while
	/*Destroy the handle and close the database*/
	sqlite3_finalize(stmt);
	sqlite3_close(db);	
	 
	return FAILED;
}





