#include <iostream>
#include <fstream>
#include <pthread.h>
#include <cstdlib>
#include <cmath>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <unistd.h>
#include <inttypes.h>
#include <ctime>
#include <sys/syspage.h>
#include <process.h>
#include <spawn.h>
#include <cstring>
#include <cstdio>
#include <errno.h>

using namespace std; 

double CyclesTomsu(uint64_t clk)
{
	uint64_t cps=SYSPAGE_ENTRY(qtime)->cycles_per_sec;
	return (double) (clk*1000000)/(double)cps;
}

int main()
{
double C[500];
ifstream in ("matrix_g.txt") ; 	
int n=0,m=0;
int i=0,j=0;
while(!in.eof())
{
	in>>C[j];
	j++;
	if((in.peek()=='\n')) n++;
}
int l=j;
n=n+1;
m=j/n;
double **A, *B;
A=new double *[n];
for(int i=0;i<n;i++)
{
A[i]=new double [m];
}
B=new double [n];
int k=0;
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<m;j++)
		{
			A[i][j]=C[k];
			k++;
		}
	}
	cout<<"Matrix"<<endl;
		for(int i=0;i<n;i++)
	{
		for(int j=0;j<m;j++)
		{
			cout<<A[i][j]<<"  ";
		}
		cout<<endl;
	}
int rcvid;
char message [512];
char rmsg [200];
int chid;
chid=ChannelCreate(0);
cout<<"Parent process created chanel id:"<<chid<<endl;
char tim[50];
sprintf(tim,"%i", chid);
const char *path="/home/ant/gauss_s";
pid_t pid=spawnl(P_NOWAIT, path, path, tim, NULL);
if(pid==-1) cout<<"error", exit(1); 
else cout<<"Parent process send id chanel to child"<<endl;
rcvid=MsgReceive(chid, message, sizeof(message),NULL);
int chid_s=atoi(message);

MsgReply(rcvid, 0, NULL, 0);
cout<<"Parent process receive child chanel id:"<<chid_s<<endl; 
int coid_s;
coid_s=ConnectAttach(0,pid,chid_s,0,0);
cout<<"Parent process connect to child chanel"<<endl;
	sprintf(tim,"%i", n);
	const char *smsg=tim;

if(MsgSend(coid_s,smsg,strlen(smsg)+1,rmsg,sizeof(rmsg))==-1) cout<<"error send";

int q=0,u=0;
uint64_t Q;
uint64_t r=ClockCycles();

while(1)
{
	sprintf(tim,"%f", C[q]);
	const char *smsg=tim;
	if(MsgSend(coid_s,smsg,strlen(smsg)+1,rmsg,sizeof(rmsg))==-1) cout<<"error send";
	q++;
	if(q==l) break;
}
cout<<"Parent process send matrix to child "<<endl;
double *X;
double t;
	X=new double [n];
	q=0;
while(1)
{
	rcvid=MsgReceive(chid, message, sizeof(message),NULL);
    t=atof(message);
	X[q]=t;
	MsgReply(rcvid, EOK, NULL, 0);
	q++;
if(q==n)	break;
}
cout<<"Parent process receive X from child"<<endl;
uint64_t y=ClockCycles();
Q=y-r;


cout<<endl<<"X: [ ";
for(int k=0;k<n;k++)
{
cout<<X[k]<<" ";
}
cout<<"]";
cout<<endl<<"time parent"<<": "<< CyclesTomsu(Q)<<" "<<endl;

ConnectDetach(coid_s);
ChannelDestroy(chid);

return 0;
}
