/*
 * 程序名:crtsurfdata4.cpp 本程序用于生成全国气象站点观测的分钟数据
 * 作者:义哥
 */

#include "_public.h"
//全国气象站点参数结构体
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

struct st_surfdata
{
 char obtid[11];		//站点代码
 char ddatetime[21];	//数据时间：格式yyyymmddhh24miss
 int t;			//气温：单位，0.1摄氏度
 int p;			//气压：0.1百帕
 int u;			//相对湿度：0-100之间
 int wd;			//风向：0-360之间
 int wf;			//风速：单位0.1m/s
 int r;			//降雨量：0.1mm
 int vis;			//能见度：0.1m
};
//存放全国气象站点每分钟观测数据的容器
vector<struct st_surfdata> vsurfdata;

char strddatetime[21];   //观测数据的时间

//模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中
void CrtSurfData();

//把容器vsurfdata中的全国气象站点分钟观测数据写入文件
bool CrtSurfFile(const char *outpath,const char *datafmt);

CLogFile logfile;		//日志类

int main(int argc,char* argv[])
{
//第一个参数:全国气象站点参数文件
//第二个:生成的测试数据存放和目录
//第三个：程序运行的日志
//第四个：数据保存格式
//那么argc = 5
 if(argc != 5)
 {
  printf("Using:./crtsurfdata4 inifile outpath logfile datafmt\n");
  printf("Example:/project/idc1/bin/crtsufdata4 /project/idc1/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata4.log xml,json,csv\n\n");

  printf("inifile：全国气象站点参数文件名。\n");
  printf("outpath：全国气象站点数据文件存放的目录\n");
  printf("logfile：本程序运行的日志文件名\n");
  printf("datafmt 生成数据文件的格式，支持csv,json,xml三种格式，用,分隔\n\n");
  return -1;
 }

 if(logfile.Open(argv[3])==false)
 {
  printf("logfile.Open(%s) failed.\n",argv[3]);
  return -1;
 }

 logfile.Write("crtsurfdata4开始运行。\n");
  
 if(LoadSTCode(argv[1])==false) return -1;

 CrtSurfData();

 if(strstr(argv[4],"xml")!=0) CrtSurfFile(argv[2],"xml");
 if(strstr(argv[4],"json")!=0) CrtSurfFile(argv[2],"json");
 if(strstr(argv[4],"csv")!=0) CrtSurfFile(argv[2],"csv");

 logfile.Write("crtsurfdata4结束运行。\n");

return 0;
}

bool LoadSTCode(const char *inifile)
{
 //打开站点参数文件,只读
 CFile File;
 if(File.Open(inifile,"r",false)==false)
 {
  logfile.Write("File.Open(%s) failed.\n",inifile);
  printf("File.Open(%s) failed.\n",inifile);
  return false;
 }

 char strBuffer[301];

 CCmdStr CmdStr;//拆分字符串
  
 struct st_stcode stcode; 

 

 while(true)
 {
  //memset(strBuffer,0,sizeof(strBuffer));每次读取之前需要进行初始化，防止bug。Fgets函数中已初始化
  if(File.Fgets(strBuffer,300,true)==false) break;
  
  //把读到的拆分成一行
  CmdStr.SplitToCmd(strBuffer,",",true);

  if(CmdStr.CmdCount()!=6) continue;
  //把站点参数的每个数据项保存到站点参数结构体中。
  memset(&stcode,0,sizeof(struct st_stcode));
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

void CrtSurfData()
{
 srand(time(0));
 memset(strddatetime,0,sizeof(strddatetime));
 LocalTime(strddatetime,"yyyymmddhh24miss");

 struct st_surfdata stsurfdata;
 
 for(int ii=0;ii<vstcode.size();ii++)
 {
  memset(&stsurfdata,0,sizeof(struct st_surfdata));

  strncpy(stsurfdata.obtid,vstcode[ii].obtid,10);	
  strncpy(stsurfdata.ddatetime,strddatetime,14);		//数据时间
  stsurfdata.t=rand()%365;
  stsurfdata.p=rand()%265+10000;
  stsurfdata.u=rand()%100+1;
  stsurfdata.wd=rand()%360;
  stsurfdata.wf=rand()%150;
  stsurfdata.r=rand()%16;
  stsurfdata.vis=rand()%5001+100000;

  //把观测数据的结构体放入vsurfdata容器
  vsurfdata.push_back(stsurfdata);
 }
}

bool CrtSurfFile(const char *outpath,const char *datafmt)
{
  CFile File;

  // 拼接生成数据的文件名，例如：/tmp/idc/surfdata/SURF_ZH_20210629092200_2254.csv
  char strFileName[301];
  sprintf(strFileName,"%s/SURF_ZH_%s_%d.%s",outpath,strddatetime,getpid(),datafmt);

  // 打开文件。
  if (File.OpenForRename(strFileName,"w")==false)
  {
    logfile.Write("File.OpenForRename(%s) failed.\n",strFileName); return false;
  }

  //写入第一行标题。
  if (strcmp(datafmt,"csv")==0) File.Fprintf("站点代码,数据时间,气温,气压,相对湿度,风向,风速,降雨量,能见度\n");

  // 遍历存放观测数据的vsurfdata容器。
  for (int ii=0;ii<vsurfdata.size();ii++)
  {
    // 写入一条记录。
    if (strcmp(datafmt,"csv")==0)
      File.Fprintf("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n",\
         vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,\
         vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);
  }

  // 关闭文件。
  File.CloseAndRename();

  logfile.Write("生成数据文件%s成功，数据时间%s，记录数%d。\n",strFileName,strddatetime,vsurfdata.size());
  return true;
}
