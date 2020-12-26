#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "soapH.h"
#include "wsseapi.h"
#include "wsaapi.h"
#include "PTZBinding.nsmap"
 
//宏定义设备鉴权的用户名和密码
//注意对于海康相机而言，鉴权的用户名和密码需要单独设置，不一定等同于登录账户密码
//设置方法参考https://zongxp.blog.csdn.net/article/details/89632354
#define USERNAME    "admin"
#define PASSWORD    "Ab12345678"
 
int main(int argc, char** argv)
{
    struct soap soap;
	soap_init(&soap);
			
    char * ip;
    char Mediaddr[256]="";
    char profile[256]="";
    float pan = 1;
    float panSpeed = 1;
    float tilt = 1;
    float tiltSpeed = 0.5;
    float zoom = 0;
    float zoomSpeed = 0.5;
    struct _tds__GetCapabilities            	req;
    struct _tds__GetCapabilitiesResponse    	rep;
    struct _trt__GetProfiles 			getProfiles;
    struct _trt__GetProfilesResponse		response;	
    struct _tptz__AbsoluteMove           absoluteMove;
    struct _tptz__AbsoluteMoveResponse   absoluteMoveResponse;
	       	
    req.Category = (enum tt__CapabilityCategory *)soap_malloc(&soap, sizeof(int));
    req.__sizeCategory = 1;
    *(req.Category) = (enum tt__CapabilityCategory)0;
       
    //第一步：获取capability
    char endpoint[255];
    memset(endpoint, '\0', 255);
    if (argc > 1)
    {
        ip = argv[1];
    }
    else
    {
        ip = "192.168.100.145"; 
    }
    sprintf(endpoint, "http://%s/onvif/device_service", ip);    
    soap_call___tds__GetCapabilities(&soap, endpoint, NULL, &req, &rep);
    if (soap.error)  
    {  
        printf("[%s][%d]--->>> soap result: %d, %s, %s\n", __func__, __LINE__, 
	                                        soap.error, *soap_faultcode(&soap), 
	                                        *soap_faultstring(&soap));  	 
    } 
    else
	{
        printf("get capability success\n");
        //printf("Dev_XAddr====%s\n",rep.Capabilities->Device->XAddr);
        printf("Med_XAddr====%s\n",rep.Capabilities->Media->XAddr);
        //printf("PTZ_XAddr====%s\n",rep.Capabilities->PTZ->XAddr);
        strcpy(Mediaddr,rep.Capabilities->Media->XAddr);
    }	
    printf("\n");
	
    //第二步：获取profile,需要鉴权	
    //自动鉴权
    soap_wsse_add_UsernameTokenDigest(&soap, NULL, USERNAME, PASSWORD);
	
    //获取profile
    if(soap_call___trt__GetProfiles(&soap,Mediaddr,NULL,&getProfiles,&response)==SOAP_OK)
    {
        strcpy(profile, response.Profiles[0].token);
        printf("get profile succeed \n");		
	    printf("profile====%s\n",profile);	
    }
    else
    {
        printf("get profile failed \n");
	    printf("[%s][%d]--->>> soap result: %d, %s, %s\n", __func__, __LINE__, 
	                                        soap.error, *soap_faultcode(&soap), 
	                                        *soap_faultstring(&soap));  
    }
    printf("\n");	
		
    //第三步：PTZ结构体填充
    char PTZendpoint[255];
    memset(PTZendpoint, '\0', 255);
    sprintf(PTZendpoint, "http://%s/onvif/PTZ", ip);
    printf("PTZendpoint is %s \n", PTZendpoint);        
    
    absoluteMove.ProfileToken = profile;
    //setting pan and tilt
    absoluteMove.Position = soap_new_tt__PTZVector(&soap, -1);
    absoluteMove.Position->PanTilt = soap_new_tt__Vector2D(&soap, -1);
    absoluteMove.Speed = soap_new_tt__PTZSpeed(&soap, -1);
    absoluteMove.Speed->PanTilt = soap_new_tt__Vector2D(&soap, -1);
    //pan
    absoluteMove.Position->PanTilt->x = pan;
    absoluteMove.Speed->PanTilt->x = panSpeed;
    //tilt
    absoluteMove.Position->PanTilt->y = tilt;
    absoluteMove.Speed->PanTilt->y = tiltSpeed;
    //setting zoom
    absoluteMove.Position->Zoom = soap_new_tt__Vector1D(&soap, -1);
    absoluteMove.Speed->Zoom = soap_new_tt__Vector1D(&soap, -1);
    absoluteMove.Position->Zoom->x = zoom;
    absoluteMove.Speed->Zoom->x = zoomSpeed;
    
    //第四步：执行绝对位置控制指令，需要再次鉴权
    soap_wsse_add_UsernameTokenDigest(&soap, NULL, USERNAME, PASSWORD);
    soap_call___tptz__AbsoluteMove(&soap, PTZendpoint, NULL, &absoluteMove, 
	                                        &absoluteMoveResponse);			 
    //第五步：清除结构体
    soap_destroy(&soap); // clean up class instances
    soap_end(&soap); // clean up everything and close socket, // userid and passwd were deallocated
    soap_done(&soap); // close master socket and detach context
    printf("\n");	
		
    return 0;
}


