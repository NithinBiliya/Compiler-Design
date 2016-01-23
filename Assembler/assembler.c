/*
Author : Nithin

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct table
{
	char str_val[100][50];
	char int_val[100][50];
	int size;
};

int str_int(char*);
//char * int2str(int,char*);
int read_line(char*,char*,char*,char*);
int strcmpi(char*,char*);
int search(struct table,char*);
void insert(char*,int,struct table*);
int find_length(char*);
void construct_str(char*,int,char*,char*,char*);
void display_table(struct table);
char * dec2hex(int,char*);
void assemble_object_code(char*,char*,char*,char*);
//void dec2bin(int,char*);
void constant2object_code(char*,char*);
int read_line2(char*,char*,char*,char*,char*);
void add_0(char*,int);
void calc_length(char*,char*);

int main(int argc, char *argv[])
{
	int starting_addr,loc_ctr,prgm_length,prev_loc_ctr;
	char str[100],label[100],opcode[100],operand[100],listing_line[70],address[50],object_code[50],addr[50],sss[50];
	struct table symtab,optab={{"STL","LDA","COMP","JEQ","JSUB","J","STA","LDL","RSUB","LDX","TD","RD","STCH","TIX","JLT","STX","LDCH","WD"},
				   {"14","00","28","30","48","3C","0C","08","4C","04","E0","D8","54","2C","38","10","50","DC"},18};
	if(argc!=2)
	{
		printf("Invalid parameters! usage : %s filename",argv[0]);
		return 1;
	}

	//		PASS 1

	FILE *fr,*fw;
	fr=fopen(argv[1],"r");
	fw=fopen("inter_file","w");
	if(fr==NULL)
	{
		printf("Error : %s not found...",argv[1]);
		return 1;
	}
	symtab.size=0;
	fgets(str,sizeof(str),fr);									//	Read first input line
	read_line(str,label,opcode,operand);
	if(!strcmpi(opcode,"START"))									//	if opcode = 'START'
	{
		sscanf(operand,"%x",&starting_addr);							//	save operand as starting address
		loc_ctr=starting_addr;									//	initialize location counter to starting address
		construct_str(str,loc_ctr,label,opcode,operand);
		fputs(str,fw);										//	write line to intermediate file
		fgets(str,sizeof(str),fr);								//	read nect input line
		read_line(str,label,opcode,operand);
	}
	else
		loc_ctr=0;										//	initialize location counter to 0
	while(strcmpi(opcode,"END"))									//	while opcode != 'END'
	{
		prev_loc_ctr=loc_ctr;
		if(str[0]!=';')
		{
			if(strlen(label)!=0)									//	if symbol in label field
			{
				if(search(symtab,label))							//	search symtab for symbol; if found
				{
					printf("Error: Duplicate symbol...");					//	Error
					exit(1);
				}
				else
					insert(label,loc_ctr,&symtab);						//	insert {label,location counter} to SYMTAB
			}
			if(search(optab,opcode))								//	search OPTAb for opcode; if found
				loc_ctr+=3;									//	add 3 to location counter
			else if(!strcmpi(opcode,"WORD"))							//	if opcode = 'WORD'
				loc_ctr+=3;									//	add 3 to location counter
			else if(!strcmpi(opcode,"RESW"))							//	if opcode = 'RESW'
				loc_ctr+=3*str_int(operand);							//	add 3*operand to location counter
			else if(!strcmpi(opcode,"RESB"))							//	if opcode = 'RESB'
				loc_ctr+=str_int(operand);							//	add operand to location counter
			else if(!strcmpi(opcode,"BYTE"))
				loc_ctr+=find_length(operand);							//	add length of constant to location counter
			else
			{
				printf("Error : Invalid operation code : %s...",opcode);			//	error
				exit(1);
			}
			construct_str(str,prev_loc_ctr,label,opcode,operand);
			fputs(str,fw);										//	write line to intermediate file
		}
		fgets(str,sizeof(str),fr);								//	read nect input line
		read_line(str,label,opcode,operand);
	}
	construct_str(str,loc_ctr,label,opcode,operand);
	fputs(str,fw);											//	write last line to intermediate file
	prgm_length=loc_ctr-starting_addr;								//	save the program length
//	display_table(optab);
//	display_table(symtab);
	fclose(fr);
	fclose(fw);


	//		PASS 2
	
	fr=fopen("inter_file","r");
	fw=fopen("out_file","w");	
	fgets(str,sizeof(str),fr);							//	read first input line
	read_line2(str,addr,label,opcode,operand);
	strcpy(listing_line,"H");
	if(!strcmpi(opcode,"START"))							//	if opcode = 'START'
	{
		int i;
		strcat(listing_line,label);
		for(i=strlen(label);i<6;i++)
			strcat(listing_line," ");
		add_0(operand,6);
		strcat(listing_line,operand);
		strcpy(sss,dec2hex(prgm_length,sss));
		add_0(sss,6);
		strcat(listing_line,sss);
		fgets(str,sizeof(str),fr);						//	read nect input line
		read_line2(str,addr,label,opcode,operand);
	}
	strcat(listing_line,"\n");
	fputs(listing_line,fw);								//	write header record to object program
	strcpy(listing_line,"T");							//	initialize first text record
	add_0(addr,6);
	strcat(listing_line,addr);
	strcat(listing_line,"  ");
	int flag,prev_flag=0;
	while(strcmpi(opcode,"END"))							//	while opcode != 'END'
	{
		int code=search(optab,opcode);						//	search OPTAB for opcode
		if(code)								//	if found
		{
			flag=0;
			if(strlen(operand)!=0)						//	if there is a symbol in the operand field
			{
				int add=search(symtab,operand);				//	search SYMTAB for operand
				if(add)							//	if found
					strcpy(address,symtab.int_val[add-1]);					//store symbol vaule as operand address
				else
				{
					strcpy(address,"0");							//store 0 as operand address
					printf("Error : undefined symbol : %s...",operand);			//	error
					exit(1);
				}
			}
			else
				strcpy(address,"0000");								//store 0 as operand address
			assemble_object_code(operand,optab.int_val[code-1],address,object_code);		//assemble the object code instruction
		}
		else if(!strcmpi(opcode,"BYTE") || !strcmpi(opcode,"WORD"))
		{
			flag=0;
			constant2object_code(operand,object_code);						//convert constant to object code
		}
		else if(!strcmpi(opcode,"RESB") || !strcmpi(opcode,"RESW"))
		{
			if(flag==0)
			{
				flag=1;
				strcat(listing_line,"\n");
				calc_length(listing_line,addr);
				fputs(listing_line,fw);									//write text record to object program
			}
			else
				flag++;
		}
		if(strlen(listing_line)+strlen(object_code)>70)					//object code will not fit into current text record)
		{
			strcat(listing_line,"\n");
			calc_length(listing_line,addr);
			fputs(listing_line,fw);									//write text record to object program
			strcpy(listing_line,"T");								//initialize new text record
			add_0(addr,6);
			strcat(listing_line,addr);
			strcat(listing_line,"  ");
		}
		if(flag==0 && prev_flag!=0)
		{
			strcpy(listing_line,"T");								//initialize new text record
			add_0(addr,6);
			strcat(listing_line,addr);
			strcat(listing_line,"  ");
			strcat(listing_line,object_code);							//add object code to textrecord
		}
		else if(flag==0)
			strcat(listing_line,object_code);							//add object code to textrecord
		//write listing line
		fgets(str,sizeof(str),fr);
		read_line2(str,addr,label,opcode,operand);							//read next input line
		prev_flag=flag;
	}
	strcat(listing_line,"\n");
	calc_length(listing_line,addr);
	fputs(listing_line,fw);											//write last text record to object program
	strcpy(listing_line,"E");										//write end record to object program
	strcpy(sss,dec2hex(starting_addr,sss));
	add_0(sss,6);
	strcat(listing_line,sss);
	strcat(listing_line,"\n");
	fputs(listing_line,fw);
	//write last listing line
	fclose(fr);
	fclose(fw);

	return 0;
}

int str_int(char *str)
{
	int i,c=1,num=0;
	for(i=strlen(str)-1;i>=0;i--,c*=10)
		num+=(str[i]-48)*c;
	return num;
}

int read_line(char *str,char *label,char *opcode,char *operand)
{
	int i,j,k,flag;
	char word[3][100];
	strcpy(label,"");
	strcpy(opcode,"");
	strcpy(operand,"");
	flag=1;
	for(i=0,j=0,k=0;str[i]!='\0' && flag==1;i++,k++)
	{
		if(str[i]==';')
		{
	//				word[j][k]='\0';
	//				str[i+1]='\0';
			flag=0;
		}
		else if(str[i]==' ' || str[i]=='\n' || str[i]=='\t')
		{
			word[j][k]='\0';
			k=-1; if(i!=0) j++;
			for(;str[i]==' ' || str[i]=='\n' || str[i]=='\t';i++);
			i--;
		}
		else
			word[j][k]=str[i];
	}
	if(j-1==2)
	{
		strcpy(label,word[0]);
		strcpy(opcode,word[1]);
		strcpy(operand,word[2]);
	}
	else if(j-1==1)
	{
		strcpy(label,"");
		strcpy(opcode,word[0]);
		strcpy(operand,word[1]);
	}
	else if(j-1==0)
	{
		strcpy(label,"");
		strcpy(opcode,word[0]);
		strcpy(operand,"");
	}
	return j-1;
}

int read_line2(char *str,char *addr,char *label,char *opcode,char *operand)
{
	int i,j,k,flag;
	char word[4][100];
	strcpy(addr,"");
	strcpy(label,"");
	strcpy(opcode,"");
	strcpy(operand,"");
	flag=1;
	for(i=0,j=0,k=0;str[i]!='\0' && flag==1;i++,k++)
	{
		if(str[i]==';')
		{
	//				word[j][k]='\0';
	//				str[i+1]='\0';
			flag=0;
		}
		else if(str[i]==' ' || str[i]=='\n' || str[i]=='\t')
		{
			word[j][k]='\0';
			k=-1; if(i!=0) j++;
			for(;str[i]==' ' || str[i]=='\n' || str[i]=='\t';i++);
			i--;
		}
		else
			word[j][k]=str[i];
	}
	if(j-1==3)
	{
		strcpy(addr,word[0]);
		strcpy(label,word[1]);
		strcpy(opcode,word[2]);
		strcpy(operand,word[3]);
	}
	else if(j-1==2)
	{
		strcpy(addr,word[0]);
		strcpy(label,"");
		strcpy(opcode,word[1]);
		strcpy(operand,word[2]);
	}
	else if(j-1==1)
	{
		strcpy(addr,word[0]);
		strcpy(label,"");
		strcpy(opcode,word[1]);
		strcpy(operand,"");
	}
	return j-1;
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

int search(struct table symtab,char *symbol)
{
	int i;
	char label[50];
	if(!symtab.size)
		return 0;
	strcpy(label,symbol);
	i=strlen(label);
	if(label[i-2]==',' && (label[i-1]=='X' || label[i-1]=='x'))
		label[i-2]='\0';
	for(i=0;i<symtab.size;i++)
		if(!strcmp(symtab.str_val[i],label))
			return i+1;
	return 0;
}

void insert(char *label,int loc_ctr,struct table *symtab)
{
	char str[50];
	strcpy(symtab->str_val[symtab->size],label);
	strcpy(symtab->int_val[symtab->size],dec2hex(loc_ctr,str));
	symtab->size++;
}

/*char * int2str(int num,char *str)
{
	int i;
	for(i=48;i>=0&&num;i--,num/=10)
		str[i]=num%10+48;
	str[49]='\0';
	return &(str[i+1]);
}*/

int find_length(char *operand)
{
	int i=0;
	if(operand[0]=='X')
		return 1;
	else for(i=2;operand[i]!='\'';i++);
	return i-2;
}

void construct_str(char *str,int loc_ctr,char *label,char *opcode,char *operand)
{
	char str2[50];
	strcpy(str,"");
	strcat(str,dec2hex(loc_ctr,str2));
	strcat(str," ");
	strcat(str,label);
	if(strlen(label))	strcat(str," ");
	strcat(str,opcode);
	strcat(str," ");
	strcat(str,operand);
	strcat(str,"\n");
}

void display_table(struct table tab)
{
	int i;
	for(i=0;i<tab.size;i++)
		printf("%s  %s\n",tab.str_val[i],tab.int_val[i]);
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

void assemble_object_code(char *operand,char *opcode,char *address,char *object_code)
{
	int i;
	char bin[50];
//	dec2bin(str_int(opcode),bin);
	strcpy(object_code,opcode);
	strcat(object_code,address);
	i=strlen(opcode);
	if(opcode[i-2]==',' && (opcode[i-1]=='X' || opcode[i-1]=='x'))
		object_code[strlen(object_code)-4]+=8;
}
/*
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
}*/

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

void add_0(char *str,int l)
{
	int i,j,len=strlen(str);
	for(i=l-len,j=len;j>=0;j--)
		str[j+i]=str[j];
	for(j=0;i;i--,j++)
		str[j]='0';
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
