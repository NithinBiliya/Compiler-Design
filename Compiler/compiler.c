/*		mini C compiler
	Developed by :
		Nithin : 	4nm09cs066
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node
{
	char var[50];					//	stores variable name
	int val;					//	stores variable value
	struct node * left, * right;
};
typedef struct node *NODEPTR;

NODEPTR maketree (char*,int);
NODEPTR getnode();
void setleft(NODEPTR,char*,int);
void setright(NODEPTR,char*,int);
void preorder(NODEPTR p);

void error(int,int,char *str);
int check_comment(char*);
void mylex(char*,char [100][200],int*);
int identifier(char*);
int match(char,char);
int var_check(char *);

int main(int argc, char *argv[])
{
	int line_count=0,S[200],t=-1,flag=0,i,j;
	char input_line[200],output_line[200],statement[200],tokens[100][200];
/*	line_count : contains the line number of line currently being scanned
	S : an integer stack
	t : points to top of stack S
	input_line : contains the current line to be scanned
	output_line : contains the line to be written into output file
	tokens : contains the tokens in the input_line after tokanization	*/
	NODEPTR root=NULL,p,q;
	FILE *fr,*fw;
	if(argc!=2)													//	2 arguments compulsary
	{
		error(1,line_count,argv[0]);
		return 1;
	}
	fr=fopen(argv[1],"r");
	if(!fr)
	{
		error(2,line_count,argv[1]);
		return 1;
	}
	if(!(argv[1][strlen(argv[1])-3]=='.' && argv[1][strlen(argv[1])-2]=='m' && argv[1][strlen(argv[1])-1]=='c'))		//	file type is .mc
	{
		error(14,line_count,NULL);
		return 1;
	}
	else
	{
		argv[1][strlen(argv[1])-2]=0;
		strcat(argv[1],"asm");											//	output file type .asm
	}
	fw=fopen(argv[1],"w");
	if(!fw)
	{
		error(3,line_count,"output.asm");
		return 1;
	}
	line_count=0;									//	start scan from 1st line
//	statement[0]=0;
	while(fgets(input_line,sizeof(input_line),fr))					//	scan till end of file
	{
		line_count++;								//	increment line_count to point to current scanned line
		check_comment(input_line);						//	remove comments from the input_line

/*		if(check_comment(input_line))//,statement))
		{
			strcat(statement,input_line);
			continue;
		}
		else
			statement[0]=0;	*/
		
		mylex(input_line,tokens,&j);						//	tokanize the input_line, j=number of tokens
		for(i=0;i<=j;i++)							//	scan through the tokens
		{
			if(!strcmp(tokens[i],"{"))					//	if {, push onto the stack S
				S[++t]=tokens[i][0];
			if(!strcmp(tokens[i],"(") || !strcmp(tokens[i],"["))		//	if (,[
			{
				if(i>0 && identifier(tokens[i-1]))			//	and if previous token is identifier
				{
					S[++t]=tokens[i][0];				//	then push [ or ( onto the stack
					continue;
				}
				else
				{
					error(8,line_count,NULL);
					return 1;
				}
			}
			if(!strcmp(tokens[i],")") || !strcmp(tokens[i],"]") || !strcmp(tokens[i],"}"))		//	if } or ] or )
			{
				if(t>=0 && match(S[t],tokens[i][0]))						//	if } matches { on stack top and so on
				{
					t--;									//	if matches, then pop the stack top element
					continue;
				}
				else
				{
					error(9,line_count,NULL);
					return 1;
				}
			}
			if(!strcmp(tokens[i],"main"))								//	if token is "main"
			{
				if(!strcmp(tokens[i+1],"("))							//	it should be followed by "("
					continue;
				else
				{
					error(4,line_count,NULL);
					return 1;
				}
			}
			if(var_check(tokens[i]))							//	if it is a variable declaration
			{
				flag++;									//	set flag (indicates atleast 1 variable declaration)
				if(!strcmp(tokens[i+1],";"))						//	if "int;"
					error(10,line_count,NULL);
				if(strcmp(tokens[j],";"))						//	line should be terminated by ";"
				{
					error(13,line_count,NULL);
					return 1;
				}
				if(flag==1)								//	if first variable declaration
				{
					strcpy(output_line,"data segment\n");				//	write "data segement" onto the .asm file
					fputs(output_line,fw);
				}
				for(i++;strcmp(tokens[i],";");i++)					//	scan through all the variables
				{
					if(!strcmp(tokens[i],",") && (!strcmp(tokens[i-1],",") || !strcmp(tokens[i+1],",")))	//	if 2 continuous ","
					{
						error(12,line_count,NULL);
						return 1;
					}
					if(identifier(tokens[i]))					//	chack if it is valid identifier
					{
						if(root==NULL)						//	for 1st variable
							root=maketree(tokens[i],0);
						else
						{
							p=q=root;
							while(strcmp(tokens[i],p->var) && q!=NULL)	//	scan through the tree till position found
							{
								p=q;
								if(strcmp(tokens[i],p->var)<0)
									q=p->left;
								else
									q=p->right;
							}
							if(!strcmp(tokens[i],p->var))			//	redeclaration of variable
							{
								error(6,line_count,tokens[i]);
								return 1;
							}
							else if(strcmp(tokens[i],p->var)<0)		//	insert variable into the tree
								setleft(p,tokens[i],0);
							else
								setright(p,tokens[i],0);
						}
						strcpy(output_line,"\t");				//	write corresponding variable onto .asm file
						strcat(output_line,tokens[i]);
						strcat(output_line," db ? \n");
						fputs(output_line,fw);
					}
				}
			}
		}
	}
	if(t!=-1)
	{
		error(15,line_count,NULL);
		return 1;
	}	
	strcpy(output_line,"");
	if(flag)
		strcat(output_line,"data ends\n");
	strcat(output_line,"code segment\nassume cs:code");					//	write remaining stuff onto .asm file
	if(flag)
		strcat(output_line,", ds:data");
	strcat(output_line,"\nstart : \n");
	if(flag)
		strcat(output_line,"\tmov ax,data\n\tmov ds,ax\n");
	strcat(output_line,"\tmov ah,4ch\n\tint 21h\ncode ends\nend start");
	fputs(output_line,fw);
	fclose(fr);
	fclose(fw);
	return 0;
}

/*	for displaying errors
	n : error number
	num : line number
	str : string related to error	*/
void error(int n,int num,char *str)
{
	switch(n)
	{
		case 1:	printf("\nInvalid parameters! usage : %s filename.mc\n",str);
			break;
		case 2:	printf("\nError in line %d : %s file not found or you do not have the required permissions to open the file!\n",num,str);
			break;
		case 3:	printf("\nError in line %d : %s file could not be created!\n",num,str);
			break;
		case 4:	printf("\nError in line %d : Syntax error\n",num);
			break;
		case 5:	printf("\nError in line %d : Semicolon expected\n",num);
			break;
		case 6:	printf("\nError in line %d : Redeclaration of variable %s\n",num,str);
			break;
		case 7:	printf("\nError in line %d : Syntax error in variable declaration\n",num);
			break;
		case 8: printf("\nError in line %d : There should be an identifier before (, [ or {\n",num);
			break;
		case 9: printf("\nError in line %d : Unexpected symbol ), ] or }\n",num);
			break;
		case 10: printf("\nWarning in line %d : useless type name in empty declaration\n",num);
			break;
		case 11: printf("\nError in line %d : Heap full!\n",num);
			break;
		case 12: printf("\nError in line %d : Syntax error in variable declaration! There should be a variable between 2 commas\n",num);
			break;
		case 13: printf("\nError in line %d : Semicolon missing at the end of the declaration statement!\n",num);
			break;
		case 14: printf("\nInvalid file type. The file should be with extension .mc\n");
			break;
		case 15: printf("\nMissing closing bracket ) or } or ]\n");
			break;
		default : printf("\nError in line %d : Unexpected error occurred!\n",num);
	}
}

/*	creates a new node
	str : variable name
	x : variable value	*/
NODEPTR maketree(char *str,int x)
{
	NODEPTR p;
	p=getnode();
	if(p==NULL)
		error(11,00,NULL);
	strcpy(p->var,str);
	p->val=x;
	p->left=NULL;
	p->right=NULL;
	return p;
}

/*	insert the node onto the left of node pointed by "p"
	str : variable name
	x : variable value	*/
void setleft(NODEPTR p,char *str,int x)
{
	if(p==NULL)
		printf("void insert\n");
	else
	if(p->left!=NULL)
		printf("invalid insertion\n");
	else
		p->left=maketree(str,x);
}

/*	insert the node onto the right of node pointed by "p"
	str : variable name
	x : variable value	*/
void setright(NODEPTR p,char *str,int x)
{
	if(p==NULL)
	printf("void insert\n");
	else if(p->right!=NULL)
		printf("invalid insertion\n");
	else
		p->right=maketree(str,x);
}

//	create node by allocating dynamic memory
NODEPTR getnode()
{
	NODEPTR p;
	p=(NODEPTR)malloc (sizeof(struct node));
	return p;
}

//	removes comment from the input_line
int check_comment(char *input_line)
{
	int flag=0,i,j=0;
	char str[200];
	for(i=0,j=0;input_line[i]!=0;i++)
	{
		if(input_line[i]=='/' && input_line[i+1]=='*')
		{
			flag=1;	i++;
			continue;
		}
		if(input_line[i]=='*' && input_line[i+1]=='/')
		{
			flag=0;	i++;
			continue;
		}
		if(input_line[i]=='/' && input_line[i+1]=='/')
			break;
		if(!flag)
			str[j++]=input_line[i];
	}
	str[j]=0;
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]=0;
	strcpy(input_line,str);
}

/*	for tokanizing the input_line
	input_line : string to be tokanized
	tokens : tokens stored in this array
	num : in the end will hold the number of tokens		*/
void mylex(char *input_line,char tokens[100][200],int *num)
{
	int i=0,j=0,k=0;
	char ch=input_line[i++];
	while(ch!=0)
	{
		if(ch=='\n' || ch=='\r' || ch==' ' || ch==',' || ch==';' || ch=='(' || ch==')' || ch=='{' || ch=='}')		//	delimitors
		{
			if(j!=0)
			{
				tokens[k][j]='\0';
				j=0;	k++;
				//printf("%s\n",tokens[k-1]);
			}
			if(!(ch=='\n' || ch=='\r' || ch==' '))
			{
				tokens[k][0]=ch;
				tokens[k][1]=0;
				k++;
				//printf("%s\n",tokens[k-1]);
			}
		}
		else
			tokens[k][j++]=ch;
		ch=input_line[i++];
	}
	tokens[k][j]=0;
	*num=k-1;
}

/*	check for ideintifier
	return 1 if str is identifier, else return 0	*/
int identifier(char *str)
{
	int i;
	if(!strcmp(str,"int") || !strcmp(str,"float") || !strcmp(str,"long") || !strcmp(str,"double"))		//	if keyword
		return 0;
	else if(!(isalpha(str[0]) || str[0]=='_'))								//	if starts with alphabet or _
		return 0;
	else
	{
		for(i=1;i<strlen(str);i++)
			if(!(isalnum(str[i]) || str[i]=='_'))							//	if alphanum or _
				return 0;
		return 1;
	}
}

//	returns 1 if the open brackets in A match the closing brakets in B, else returns 0
int match(char A,char B)
{
	if(A=='(' && B==')')
		return 1;
	else if(A=='[' && B==']')
		return 1;
	else if(A=='{' && B=='}')
		return 1;
	else 
		return 0;
}

//	return 1 if str contains valid data type, else return 0
int var_check(char *str)
{
	if(!strcmp(str,"int"))
		return 1;
	else
		return 0;
}
