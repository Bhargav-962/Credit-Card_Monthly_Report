//preprocessor directives
#include <stdio.h>
#include<stdlib.h>
#include<string.h>

#define RATE 0.02 //Defined constants
#define Line 17

int count=0,d,m,y; //Global Variables
int n; 
char name[50];
char phno[12];
char address[200];
char card_number[17];
char due_date[12];
char ch;
char chr[3];
float avb; //Balance credit
float prev_amount,credit_limit,cash_limit;

typedef struct data     //Structure to store data elements
{
	char date[12];
	char details[8];
	char category[20];
	float amount;
}table; 

//Function Definitions

void input();
void tabulate(table *);
float compute(table *);
void write(table *, float);
void print(float);

FILE *ptr,*ptr1; //File pointers to read and write from two files

int main(void)
{
	int i;
	char c;
	char temp[12];
	float net_amount;

	if ((ptr = fopen("Database.txt","r")) == NULL) //Error Handling
	{ 
		printf("Error in accessing data!!\n"); 
		exit(1);
	} 

	while ((c=fgetc(ptr)) != EOF) //read a line
	{
		if(c=='\n')
        		count++; //To count the number of lines in the file
	}
	n=count-Line;
	table arr[n]; // defining Variable length Array of structures

	input(); //Call of function input

	char *token;

	strcpy(temp,due_date);
	token=strtok(temp,"/");
	d=atoi(token);

	token=strtok(NULL,"/");
	m=atoi(token);

	token=strtok(NULL,"/");
	y=atoi(token);

	token=strtok(NULL,"/");

	for(i=0;i<n;i++)
		tabulate(&arr[i]); //Passing individual structure elements to function tabulate

	net_amount=compute(arr);

	write(arr,net_amount); //Call to function to write to a file

	print(net_amount); //Call to function print

	fclose(ptr);
	fclose(ptr1);

	return 0;
}

void input()
{
	int a=0;

	fseek(ptr,0,SEEK_SET);

	fscanf(ptr, "%*s %[^\n]", name);

	fscanf(ptr, "%*s %[^\n]", address);

	fscanf(ptr, "%*s %*s %s", phno);

	fscanf(ptr, "%*s %*s %*s %s", card_number);

	fscanf(ptr, "%*s %*s %*s %s", due_date);

	fscanf(ptr, "%*s %*s %*s %f %c", &prev_amount,&ch);

	fscanf(ptr, "%*s %*s %f", &credit_limit);

	fscanf(ptr, "%*s %*s %*s %f", &cash_limit);
 
	char c[15];

	//To direct seek to tabulated data for further reading
	fscanf(ptr,"%*s %*s %*s %s",c);

}

void tabulate(table *pt)
{
	
	fscanf(ptr,"%s %s %f %[^\n]", pt->date, pt->details, &pt->amount, pt->category);


}

float compute(table *pt)
{
	int dd,mm,yy,condition=0,start=0,count=0,c,diff;

	float net=0.0,interest=0.0,pay=0.0,temp=0.0;

	char tempc[12];

	if (ch== 'C')
			net=prev_amount;
		else
			net=prev_amount*(-1.0);

	for(c=0;c<n;c++)
	{
		char *token;
		strcpy(tempc,pt->date);

		token=strtok(tempc,"/");
		dd=atoi(token);

		if (c==0)
			start=dd;

		token=strtok(NULL,"/");
		mm=atoi(token);

		token=strtok(NULL,"/");
		yy=atoi(token);

		token=strtok(NULL,"/");


		if(mm == m && dd>d && count==0)
		{
			
			if (net<0)
			{
				condition=1;
				pay=net;
				temp=pay*(-1.0);

			}

			count=1;
		}

		if (condition == 1)
		{
			if (strcmp(pt->details,"credit")==0)
			{
				pay+=pt->amount;
			}
			
			if (pay>=0)
			{
				condition=0;
				diff=31-start+1+dd;
				interest=temp*diff*RATE;
			}
		}
			
					
		
		if (strcmp(pt->details,"credit")==0)
			net+=pt->amount;
		else
			net-=pt->amount;
		pt++;
	}

	if (pay<0)
	{
		diff=31-start+1+dd;
		interest=temp*diff*RATE;
	}
	
	net=net-interest;

	return net;
}

void write(table *pt, float amt)
{
	int i;
	if ((ptr1 = fopen("Monthly_Report.txt","w")) == NULL) //Error Handling
	{ 
		printf("Error in opening file!!\n"); 
		exit(1);
	}

	if (amt>= 0)
	{
		strcpy(chr,"Cr");
		avb=credit_limit+amt;
	}

	else
	{
		strcpy(chr,"Dr");
		amt=amt*(-1);
		avb=credit_limit-amt;
	}

	
	fprintf(ptr1, "%80s\n\n", "PLATINUM CREDIT CARD STATEMENT");

	fprintf(ptr1, "%s %120s %s\n", name, "Phone number:",phno);

	fprintf(ptr1,"%s\n\n\n\n\n\n", address);

	fprintf(ptr1,"%80s\n\n", "******PAYMENT SUMMARY******");

	fprintf(ptr1, "Total Payment Due =%.2f %s     Statement Period:- 20/01/2020 - 19/02/2020      Payment Due Date: %s      Statement Generation Date:- 19/02/2020\n\n",amt,chr,due_date);

	fprintf(ptr1, "Credit Card Number:%s        Credit Limit=%.2f        Available Credit Limit=%.2f        Available Cash Limit=%.2f\n\n",card_number,credit_limit,avb,cash_limit);

	fprintf(ptr1, "Previous Balance - Payments - Credits + Purchase + Other Debit&Charges = Total Payment Due\n\n\n");
 

	fprintf(ptr1,"%80s\n\n", "******ACCOUNT SUMMARY******");
	
	fprintf(ptr1,"Slno.   DATE\t\t MERCHANT CATEGORY\t\t\t TRANSACTION\t\t\t AMOUNT(Rs.) \n");

	fprintf(ptr1," \t\t\t\t CARD No:%s \t\t\t NAME: %s\n\n",card_number,name);

	for (i=0;i<n;i++)
	{
		char c[3];
		if (strcmp(pt[i].details,"credit") ==0)
		{
			strcpy(c,"cr");
		}
		
		else
			strcpy(c,"dr");

		fprintf(ptr1, "%d.     %s\t\t %s \t\t\t %s\t\t\t\t %.2f %s\n\n\n",i+1,pt[i].date,pt[i].category,pt[i].details,pt[i].amount,c);
	}
	
	fprintf(ptr1, "%100s\n\n", "******END OF STATEMENT******");
	
}
	
	

void print(float amount)
{
	if( amount<0)
		amount=amount*(-1);

	printf("Name of the card holder:%s\n\n", name);

	printf("Address:%s\n\n", address);

	printf("Phone number:%s\n\n", phno);

	printf("Credit Card number:%s\n\n",card_number);

	printf("Statement Period:- 20/01/2020 - 19/02/2020\n\n");

	printf("Total Payment Due =%.2f   %s        Payment Due Date: %s      Statement Generation Date: 19/02/2020\n\n",amount,chr,due_date);

	printf("Credit Limit=%.2f        Available Credit Limit=%.2f        Available Cash Limit=%.2f\n\n",credit_limit,avb,cash_limit);

	printf("Monthly Report generated Successfully!!\n");
	
}
	 
	
	