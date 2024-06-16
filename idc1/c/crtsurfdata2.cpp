/*
 * 程序名:crtsurfdata2.cpp 本程序用于生成全国气象站点观测的分钟数据
 * 作者:义哥
 */

#include "_public.h"

struct st_stcode
{
 char provname[31];	//省
 char obtid[11];		//站号
 char obtname[31];	//站名
 double lat;		//纬度
 double lon;		//经度
 double height;		//海拔高度
};

//存放全国气象站点参数的容器
vector<struct st_stcode> vstcode;

//把站点参数文件中的数据加载到容器的函数
bool LoadSTCode(const char *inifile);

CLogFile logfile;		//日志类



int main(int argc,char* argv[])
{
//第一个参数:全国气象站点参数文件
//第二个:生成的测试数据存放和目录
//第三个：程序运行的日志
//那么argc = 4
 if(argc != 4)
 {
  printf("Using:./crtsurfdata2 inifile outpath logfile\n");
  printf("Example:/project/idc1/bin/crtsufdata2 /project/idc1/ini/stcode.ini /tmp/surfdata /log/idc/crtsurfdata2.log\n\n");

  printf("inifile：全国气象站点参数文件名。\n");
  printf("outpath：全国气象站点数据文件存放的目录\n");
  printf("logfile：本程序运行的日志文件名\n\n");

  return -1;
 }

 if(logfile.Open(argv[3])==false)
 {
  printf("logfile.Open(%s) failed.\n",argv[3]);
  return -1;
 }

 logfile.Write("crtsurfdata2开始运行。\n");
  
 if(LoadSTCode(argv[1])==false) return -1;

 logfile.Write("crtsurfdata2结束运行。\n");

return 0;
}

bool LoadSTCode(const char *inifile)
{
 //打开站点参数文件,只读
 CFile File;
 if(File.Open(inifile,"r")==false)
 {
  logfile.Write("File.Open(%s) failed.\n",inifile);
  printf("File.Open(%s) failed.\n",inifile);
  return false;
 }

 CCmdStr CmdStr;//拆分字符串
  
 struct st_stcode stcode; 

 char strBuffer[301];

 while(true)
 {
  //memset(strBuffer,0,sizeof(strBuffer));每次读取之前需要进行初始化，防止bug。Fgets函数中已初始化
  if(File.Fgets(strBuffer,300,true)==false) break;
  logfile.Write("=%s=\n",strBuffer);
  //把读到的拆分成一行
  CmdStr.SplitToCmd(strBuffer,",",true);
  if(CmdStr.CmdCount()!=6) continue;
  //把站点参数的每个数据项保存到站点参数结构体中。
  CmdStr.GetValue(0, stcode.provname,30);
  CmdStr.GetValue(1, stcode.obtid,30);
  CmdStr.GetValue(2, stcode.obtname,30);
  CmdStr.GetValue(3,&stcode.lat);
  CmdStr.GetValue(4,&stcode.lon);
  CmdStr.GetValue(5,&stcode.height);
  //把站点参数结构体放入站点参数容器
  vstcode.push_back(stcode);
 }
 /*
 for(int ii=0;ii<vstcode.size();ii++)
 {
  logfile.Write("provname=%s,obtid=%s,obtname=%s,lat=%.2f,lon=%.2f,height=%.2f\n",\
                 vstcode[ii].provname,vstcode[ii].obtid,vstcode[ii].obtname,vstcode[ii].lat,\
                 vstcode[ii].lon,vstcode[ii].height);
 }
*/
 return true;
}
