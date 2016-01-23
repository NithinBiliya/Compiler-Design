#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <math.h>

int str_int(char*);
char * int_str(int,char*);
int strcmpi(char*,char*);
char * dec2hex(int,char*);
void dec2bin(int,char*);
void add_0(char*,int);
void constant2object_code(char*,char*);
void calc_length(char*,char*);

int main(int argc, char *argv[])
{
	int i=str_int("9999");
	char str[50],ll[70];
	printf("%d",i);

	printf("\n%d\n",strcmpi("yah0o","yAH0o0"));

	printf("\n%s\n",int_str(10780094,str));

	strcpy(str,"1000");
	sscanf(str,"%x",&i);
	printf("\nin integer : %d\n",i);

	printf("\nhexadecimal value : %s\n",dec2hex(3456,str));

	dec2bin(45,str);
	printf("\nbinary value :%s\n",str);

	strcpy(str,"fff");
	add_0(str,5);
	printf("\nadding 0s : %s\n",str);

	constant2object_code("C'EOF'",str);
	printf("\nObject code : %s\n",str);

	strcpy(ll,"T001000  1410334820390010362810303010154820613C100300102A0C103900102D");
	strcpy(str,"101E");
	calc_length(ll,str);
	printf("\nListing line : %s\n",ll);
	return 0;
}

int str_int(char *str)
{
	int i,c=1,num=0;
	for(i=strlen(str)-1;i>=0;i--,c*=10)
		num+=(str[i]-48)*c;
	return num;
}

char * int_str(int num,char *str)
{
	int i;
	for(i=48;i>=0&&num;i--,num/=10)
		str[i]=num%10+48;
	str[49]='\0';
	return &(str[i+1]);
}

int strcmpi(char *str1,char *str2)
{
	int i,len=strlen(str1);
	if(len!=strlen(str2))
		return -1;
	for(i=0;i<len;i++)
	{
		if(str1[i]==str2[i])
			continue;
		else if(str1[i]!=str2[i] && str1[i]>='a' && str1[i]<='z' && str1[i]-32==str2[i])
			continue;
		else if(str1[i]!=str2[i] && str1[i]>='A' && str1[i]<='Z' && str1[i]+32==str2[i])
			continue;
		else
			return 1;
	}
	return 0;
}

char * dec2hex(int num,char *hex)
{
	int i=48;
	for(;num;num/=16)
		switch(num%16)
		{
			case 10: hex[i--]='A';	break;
			case 11: hex[i--]='B';	break;
			case 12: hex[i--]='C';	break;
			case 13: hex[i--]='D';	break;
			case 14: hex[i--]='E';	break;
			case 15: hex[i--]='F';	break;
			default : hex[i--]=(num%16)+48;
		}
	hex[49]='\0';
	return &(hex[i+1]);
}

void dec2bin(int decimal,char *binary)
{
	int k=0,n=0;
	int neg_flag=0;
	int remain,old_decimal;
	char temp[80];
	if(decimal<0)
	{
		decimal=-decimal;
		neg_flag=1;
	}
	do
	{
		old_decimal=decimal;
		remain=decimal%2;
		decimal/=2;
		temp[k++]=remain+'0';
	}while(decimal>0);
	if(neg_flag)
		temp[k++]='-';
//	else
//		temp[k++]=' ';
	while(k>=0)
		binary[n++]=temp[--k];
	binary[n-1]=0;
}

void add_0(char *str,int l)
{
	int i,j,len=strlen(str);
	for(i=l-len,j=len;j>=0;j--)
		str[j+i]=str[j];
	for(j=0;i;i--,j++)
		str[j]='0';
}

void constant2object_code(char *operand,char *object_code)
{
	int i,j;
	char temp[50];
	strcpy(temp,operand);
	if(temp[0]=='x' || temp[0]=='X')
	{
		for(i=0;i<=strlen(temp)-2;i++)
			temp[i]=temp[i+2];
		temp[strlen(temp)-1]=0;
		strcpy(object_code,temp);
	}
	else if(temp[0]=='c' || temp[0]=='C')
	{
		strcpy(object_code,"");
		for(i=2;operand[i]!='\'';i++)
			strcat(object_code,dec2hex(operand[i],temp));
	}
	else
	{
		strcpy(temp,dec2hex(str_int(operand),temp));
		for(i=6-strlen(temp),j=0;i;i--,j++)
			object_code[j]='0';
		object_code[j]=0;
		strcat(object_code,temp);
	}
}

void calc_length(char *listing_line,char *addr)
{
	char op1[50],op2[50];
	int iop1,iop2,i;
	strcpy(op1,addr);
	for(i=1;i<=6;i++)
		op2[i-1]=listing_line[i];
	op2[i-1]=0;
	sscanf(op1,"%x",&iop1);
	sscanf(op2,"%x",&iop2);
	strcpy(op1,dec2hex(iop1-iop2,op1));
	add_0(op1,2);
	for(i=7;i<=8;i++)
		listing_line[i]=op1[i-7];
}

