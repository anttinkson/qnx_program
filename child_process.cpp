#include <iostream>
#include <stdio.h>
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

void gauss(double **A, double *B, int n)
{
	double t;
	int i,j,k;
	for(i=0;i<n;i++)
	{
		t=A[i][i];
		for(j=n;j>=i;j--)
		{
			A[i][j]/=t;
		}
		for(j=i+1;j<n;j++)
		{
			t=A[j][i];
			for(k=n;k>=i;k--)
			{
				A[j][k]-=t*A[i][k];
			}
		}
	}	
	
	B[n-1]=A[n-1][n];
	for(i=n-2;i>=0;i--)
	{
		B[i]=A[i][n];
		for(j=i+1;j<n;j++)
		{	
			B[i]-=A[i][j]*B[j];
		}
	}	
}
int main(int argc, char* argv[])
{
double **A, *B;
double C[500];

int rcvid;
int chid;
chid=atoi(argv[1]);
cout<<"Child process receive id chanel from parent id: "<<chid<<endl;
int coid; 
char rmsg [200];
char message [512];

pid_t pid=getppid();

coid=ConnectAttach(0,pid,chid,0,0);
if(coid==-1)
{
	cout<<"error";
}
else cout<<"Child process connect to parent chanel"<<endl;
int n;
int chid_s;
chid_s=ChannelCreate(0);
cout<<"Child process created chanel id: "<<chid_s<<endl;
	char tim[50];
	sprintf(tim,"%i", chid_s);
	const char *smsg=tim;
MsgSend(coid,smsg,strlen(smsg)+1,rmsg,sizeof(rmsg));
cout<<"Child process send id chanel to parent"<<endl;
	rcvid=MsgReceive(chid_s, message, sizeof(message),NULL);
    int t=atoi(message);
	n=t;
	MsgReply(rcvid, EOK, NULL, 0);

A=new double *[n];
for(int i=0;i<n;i++)
{
A[i]=new double [n+1];
}
B=new double [n];

int q=0,u=0;
int l=n*(n+1);

double p;
while(1)
{
	rcvid=MsgReceive(chid_s, message, sizeof(message),NULL);
    p=atof(message);
	C[q]=p;
	MsgReply(rcvid, EOK, NULL, 0);
	q++;
	if(q==l)	break;
}
cout<<"Child process receive matrix from parent process"<<endl;
int k=0;
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n+1;j++)
		{
			A[i][j]=C[k];
			k++;
		}
	}
	uint64_t r=ClockCycles();
	gauss(A,B,n);
	uint64_t y=ClockCycles();
	uint64_t Q=y-r;
	
q=0;
while(1)
{
	sprintf(tim,"%f", B[q]);
	const char *smsg=tim;
	if(MsgSend(coid,smsg,strlen(smsg)+1,rmsg,sizeof(rmsg))==-1) cout<<"error send";
	q++;
	if(q==n) break;
}
cout<<"Child process send X to parent process"<<endl;
cout<<endl<<"time son: "<< CyclesTomsu(Q)<<" "<<endl;

ConnectDetach(coid);
ChannelDestroy(chid_s);

return 0;
}