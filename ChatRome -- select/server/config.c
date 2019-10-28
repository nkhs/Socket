/*******************************************************************************
* Basic profile implementation -- contains the required header files
* User information structure definition
* Online user list definition
********************************************************************************/
#include "config.h"

/*************************************
Function name: StateMsg
Function: get the corresponding message content according to the operation result
Parameters: stateRet -- the result of the operation
Return value: operation result string
**************************************/
char *stateMsg(int stateRet)
{
	switch(stateRet)
	{
	case EXCEED://已达服务器链接上限
		return "已达服务器链接上限！\n";
		break;
    case SUCCESS: //成功
		return "操作成功！\n";
		break;
	case FAILED:  //失败
		return "操作失败！\n";
		break;    
	case DUPLICATEID: //重复的用户名
		return "重复的用户名！\n";
		break;	
	case INVALID:	//不合法的用户名
		return "不合法输入！\n";
		break;    
	case ID_NOT_EXIST: //账号不存在
		return "账号不存在！\n";
		break;
    case WRONGPWD: //密码错误
		return "密码错误！\n";
		break;
	case ALREADY_ONLINE:
		return "该用户已在线！\n";
		break;
	case ID_NOT_ONLINE:
		return "该用户不在线！\n";
		break;
	case ALL_NOT_ONLINE:
		return "无人在线！\n";
		break;
	case MESSAGE_SELF:   //消息对象不能选择自己
		return "不能给自己发送消息\n";
		break;	
	default:
		return "未知操作结果！\n";
		break;
	}//switch
};

/*************************************
Function name: copyUser
Function: User structure object copy operation
Parameters: user1--target copy object user2--source copy object
Return value: none
**************************************/
void copyUser(User *user1 , User *user2)
{
	strcpy((*user1).userName , (*user2).userName);
	strcpy((*user1).password , (*user2).password);
	(*user1).userAddr = (*user2).userAddr;
	(*user1).sockfd = (*user2).sockfd;
	(*user1).speak = (*user2).speak;
	strcpy((*user2).registerTime , (*user2).registerTime);

}
