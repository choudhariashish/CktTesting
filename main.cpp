/*
 * main.cpp
 */
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <time.h>

using namespace std;

#define NAND 1
#define AND	2
#define NOT	3
#define NOR	4
#define OR	5
#define XOR	6
#define BUFF 7

#define RandomTestDelay	1000
char fileName[50];

bool getValueOfInputLine(int _lineNum);
bool calcGateOutput(int _gateType, bool value_one, bool value_two);

struct GATE
{
	int GateType;
	int No_Of_Inputs;
	vector<int> Input_Nodes;
	int Output_Node;
	int Output_Value;
	bool Output_Ready;
};

// store first 5 lines of input benchmark file
char Bench_Name[50];
int Bench_Inputs;
int Bench_Outputs;
int Bench_Inverter;
int Bench_TotalGates;

struct InputLine
{
	int lineNumber;
	bool Value;
	bool First_Input;
};
struct StuckAtLine
{
	int lineNumber;
	bool StuckAtValue;
};

vector<InputLine*> InputVector;
vector<int> OutputVector;
vector<StuckAtLine*> StuckAtVector;
vector<GATE*> GateCkt;

bool getValueOfInputLine(int _lineNum)
{
    for(vector<InputLine*>::iterator it=InputVector.begin();it!=InputVector.end();++it)
    {
    	if((*it)->lineNumber == _lineNum)
    		return (*it)->Value;
    }
    return 0;
}
bool isLineFirstInput(int _lineNum)
{
    for(vector<InputLine*>::iterator it=InputVector.begin();it!=InputVector.end();++it)
    {
    	if((*it)->lineNumber == _lineNum && (*it)->First_Input)
    		return true;
    }
    return false;
}
void setValueOfInputLine(int _lineNum, bool _value)
{
    for(vector<InputLine*>::iterator it=InputVector.begin();it!=InputVector.end();++it)
    {
    	if((*it)->lineNumber == _lineNum)
    		(*it)->Value=_value;
    }
}
bool isLineStuckAt(int _lineNum)
{
    for(vector<StuckAtLine*>::iterator it=StuckAtVector.begin();it!=StuckAtVector.end();++it)
    {
    	if((*it)->lineNumber == _lineNum)
    		return true;
    }
    return false;
}
bool getLineStuckAtValue(int _lineNum)
{
	bool errorFlag=0;
    for(vector<StuckAtLine*>::iterator it=StuckAtVector.begin();it!=StuckAtVector.end();++it)
    {
    	if((*it)->lineNumber == _lineNum)
    	{
    		errorFlag=1;
    		return (*it)->StuckAtValue;
    	}
    }
    if(!errorFlag)
    {
    	cerr<<"\n\nError/bug in Stuck-At Logic\n";
    }
	return errorFlag;
}
void generateRandomInputVector()
{
	srand ( time(NULL) );
    for(vector<InputLine*>::iterator it=InputVector.begin();it!=InputVector.end();++it)
    {
    	if((*it)->First_Input)
    	{
    	    int ranValue = rand() % 10 + 1;
    		(*it)->Value = ranValue & 0x0001;
    	}
    }
    usleep( RandomTestDelay * 1000 );
}

void processGatesWithInput(int option)
{
	for(vector<GATE*>::iterator it=GateCkt.begin();it!=GateCkt.end();++it)
	{
		if(option==6)
			cout<<"\n* Intermediate:\t\tGate: "<<(*it)->Output_Node;
		int tempLine=(*(*it)->Input_Nodes.begin());
		int tempVal=getValueOfInputLine(tempLine);

		if((*it)->No_Of_Inputs==1)
		{
			if(option==6)
			{
				cout<<" #Inputs:"<<(*it)->No_Of_Inputs<<" in:"<<tempLine;
				cout<<" GateType: "<<(*it)->GateType;
				cout<<" outVal:"<<calcGateOutput( (*it)->GateType, getValueOfInputLine(tempLine),getValueOfInputLine(tempLine) );
			}
			if(isLineStuckAt((*it)->Output_Node))
			{
				setValueOfInputLine((*it)->Output_Node, getLineStuckAtValue((*it)->Output_Node));
			}
			else
				setValueOfInputLine((*it)->Output_Node, calcGateOutput((*it)->GateType, getValueOfInputLine(tempLine),getValueOfInputLine(tempLine)));
		}
		else if((*it)->No_Of_Inputs>1)
		{
			if(option==6)
				cout<<" #Inputs:"<<(*it)->No_Of_Inputs<<" GateType:"<<(*it)->GateType;
			for(vector<int>::iterator inpt=++((*it)->Input_Nodes.begin()); inpt!=(*it)->Input_Nodes.end();inpt++)
			{
				tempVal = calcGateOutput( (*it)->GateType, tempVal, getValueOfInputLine(*inpt) );
				if(isLineStuckAt((*it)->Output_Node))
				{
					setValueOfInputLine((*it)->Output_Node, getLineStuckAtValue((*it)->Output_Node));
				}
				else
					setValueOfInputLine( (*it)->Output_Node, tempVal);
				if((*it)->GateType==NAND || (*it)->GateType==NOR)
					tempVal=(tempVal+1)%2;
				tempLine = (*inpt);

				if(option==6)
					cout<<" outVal:"<<getValueOfInputLine((*it)->Output_Node);
			}
		}
	}
}
void printOutputVector()
{
	cout<<"\nOUTPUT: ";
	for(vector<InputLine*>::iterator it=InputVector.begin();it!=InputVector.end();++it)
	{
		for(vector<int>::iterator i=OutputVector.begin();i!=OutputVector.end();++i)
		{
			if((*i)==(*it)->lineNumber)
				cout<<(*it)->Value;
		}
	}
	cout<<"\n";
}
void printInputVector()
{
	cout<<"\nINPUT:  ";
	for(vector<InputLine*>::iterator it=InputVector.begin();it!=InputVector.end();++it)
	{
		if((*it)->First_Input)
			cout<<(*it)->Value;
	}
}

void readBenchMarkFile(int fileReadFlag)
{
    char line[100];
    char *ptr;
	int outLine;
	int i=0,k=10;
	bool flag=false;
    FILE* benchFile;

    if(!fileReadFlag)
    {
    	cout<<"\nAvailable benchmark files:\n \
    			c17.bench, c432.bench, c499.bench, c880.bench, c1355.bench,\n \
    			c1980.bench, c2670.bench, c3540.bench, c5315.bench, c6288.bench, c7552.bench";
    	cout<<"\n\nEnter filename>> ";
    	cin>>fileName;
    	cout<<"\n\nf: "<<fileName;
    }
    benchFile = fopen(fileName,"r");

    cout<<"\n\ndone here";

    if (benchFile == NULL)
    {
         cerr<<"No such benchmark file\n\n";
    }


    InputVector.clear();
    OutputVector.clear();
    StuckAtVector.clear();
    GateCkt.clear();

    // read input benchmark file
    while(fgets(line, sizeof line, benchFile)!=NULL)
    {
		static int count=0;
		static int countState=0;
		/*
		 * count:                                   ("   0,    1,            2,              3         ")
		 * countState: 4 types of data in input file("#..., INPUTS(*)..., OUTPUTS(*)..., Connections...")
		 */
    	if(countState==3)
    	{
    		if(count)
    		{
    			GATE *gate = new GATE();
    			// find Inputs
    			while(line[i]!='\n')
    			{
    				if(line[i]=='(')
    				{
    					char *temp = new char[5];
    					while(line[i]!=',')
    					{
    						if(line[i]==')')
    						{
    							flag=true;
    							break;
    						}
    						temp[k++]=line[i];
    						i++;
    					}
    					temp[0]='0';
    					gate->Input_Nodes.push_back(atoi(temp));
    				}
    				else if(line[i]==' ' && line[i-1]==',')
    				{
    					char *temp = new char[5];
    					while(line[i]!=',')
    					{
    						if(line[i]==')')
    						{
    							flag=true;
    							break;
    						}
    						temp[k++]=line[i];
    						i++;
    					}
    					temp[0]='0';
    					gate->Input_Nodes.push_back(atoi(temp));
    				}
    				k=0;
    				i++;
    				if(flag==true)
    				{
    					break;
    				}
    			}
    			flag=false;
    			i=0;
    			// example: 10 = NAND(1, 3)
     			// find output line
    			ptr = strtok(line, "=");
    			outLine=atoi(ptr);
    			gate->Output_Node=outLine;
    			InputLine *inputLine = new InputLine();
    			inputLine->Value=false;
    			inputLine->First_Input=false;
    			inputLine->lineNumber=outLine;
    			InputVector.push_back(inputLine);
    			// find GATE
    			ptr = strtok(NULL, " \t\n\r()=,");
    			if(!strcmp(ptr,"NAND"))
    			{
    				gate->GateType=NAND;
    			}
    			else if(!strcmp(ptr, "AND"))
    			{
    				gate->GateType=AND;
    			}
    			else if(!strcmp(ptr, "NOT"))
    			{
    				gate->GateType=NOT;
    			}
    			else if(!strcmp(ptr, "NOR"))
    			{
    				gate->GateType=NOR;
    			}
    			else if(!strcmp(ptr, "OR"))
    			{
    				gate->GateType=OR;
    			}
    			else if(!strcmp(ptr, "XOR"))
    			{
    				gate->GateType=XOR;
    			}
    			else if(!strcmp(ptr, "BUFF"))
    			{
    				gate->GateType=BUFF;
    			}
    			gate->Output_Value=false;
    			gate->Output_Ready=false;
    			gate->No_Of_Inputs=gate->Input_Nodes.size();
    			GateCkt.push_back(gate);
    		}
    		count++;
    	}
    	/*
    	 * scan Outputs
    	 */
    	else if(countState==2)
    	{
    		if(count)
    		{
    			ptr = strtok(line, "OUTPUT()");
    			OutputVector.push_back(atoi(ptr));
   			}
   			count++;
   			if(count==Bench_Outputs+1)
   			{
   				count=0;
    			countState=3;
    		}
    	}
    	/*
    	 * scan Inputs
    	 */
    	else if(countState==1)
    	{
    		if(count)
    		{
    			ptr = strtok(line, "INPUT()");
    			InputLine *inputLine = new InputLine();
    			inputLine->Value=false;
    			inputLine->First_Input=true;
    			inputLine->lineNumber=atoi(ptr);

    			InputVector.push_back(inputLine);
    		}
    		count++;
   			if(count==Bench_Inputs+1)
   			{
   				count=0;
   				countState=2;
   			}
    	}
    	else if(countState==0)
		{
			if(line[0]=='#')
			{
				ptr = strtok(line, "#");
				if(count==0)
					strcpy(Bench_Name, ptr);
				if(count==1)
					Bench_Inputs=atoi(ptr);
				if(count==2)
					Bench_Outputs=atoi(ptr);
				if(count==3)
					Bench_Inverter=atoi(ptr);
				if(count==4)
					Bench_TotalGates=atoi(ptr);
				count++;
				if(count==5)
				{
					cout<<"\n*************\n";
					cout<<"\nBenchmark Name:"<<Bench_Name<<"\n#_Inputs:"<<Bench_Inputs<<"\n#_Outputs:" \
							<<Bench_Outputs<<"\n#_Inverter:"<<Bench_Inverter<<"\n#_Gates:"<<Bench_TotalGates<<endl;
					cout<<"\n*************\n";
					count=0;
					countState=1;
				}
			}
    	}
    }
    // done reading input benchmark file

    cout<<"\n\nINPUTS:";
    for(vector<InputLine*>::iterator it=InputVector.begin();it!=InputVector.end();++it)
    {
    	cout<<" "<<(*it)->lineNumber;
    }
    cout<<"\n\nOUTPUTS:";
    for(vector<int>::iterator it=OutputVector.begin();it!=OutputVector.end();++it)
    	cout<<" "<<*it;
    fclose(benchFile);
}

int main(int argc, char** argv)
{
	if(argc<2)
		readBenchMarkFile(0);
	else
	{
		strcpy(fileName, argv[1]);
		cout<<"\n "<<fileName;
		readBenchMarkFile(1);
	}
	int option=0;
    while(1)
    {
    		cout<<"\n\n*******************************************************************";
    		cout<<"\n*                                                                 *";
    		cout<<"\n* Available options:                                              *"
    				"\n*     1) Provide Input Vector                                     *"
    				"\n*     2) Set Input Vector                                         *"
    				"\n*     3) Reset Input Vector                                       *"
    				"\n*     4) Print Input Vector                                       *"
    				"\n*     5) Process Gates With Input Vector                          *"
    				"\n*     6) Process Gates With Input Vector And Print Line Values    *"
    				"\n*     7) Print Output Vector                                      *"
    				"\n*     8) Print Number of Gates/Nodes                              *"
    				"\n*     9) Set Stuck-At-Lines                                       *"
    				"\n*    10) Print Stuck-At-Lines                                     *"
    				"\n*    11) Clear Stuck-At-Lines (fault free circuit)                *"
    				"\n*    12) Random Input Vector, Generate-> Process-> Print          *"
    				"\n*    13) Provide New Benchmark File                               *"
    				"\n*    14) Quit                                                     *";
    		cout<<"\n*                                                                 *";
    		cout<<"\n*******************************************************************";

    		cout<<"\n\nEnter option>> ";
    		cin>>option;
     	if(option==1 || option==2 || option==3 || option==4)
    	{
    		/*
    		 * Get input vector from user
    		 */
    		for(vector<InputLine*>::iterator it=InputVector.begin();it!=InputVector.end();++it)
    		{
    			if((*it)->First_Input)
    			{
    				if(option==1)
    				{
    					cout<<"\nLine:"<<(*it)->lineNumber<<" :";
    					cin>>(*it)->Value;
    				}
    				else if(option==3)
    					(*it)->Value=0;
    				else if(option==2)
    					(*it)->Value=1;
    				else if(option==4)
    					cout<<"\nLine: "<<(*it)->lineNumber<<" Value:"<<(*it)->Value;
    			}
    		}
    	}

    	else if(option==5 || option==6)
    	{
    		processGatesWithInput(option);
    	}

    	else if(option==7)
    	{
    		cout<<"\n\nProcessed Output Lines:";
    		cout<<"\n";
    		for(vector<InputLine*>::iterator it=InputVector.begin();it!=InputVector.end();++it)
    		{
    			for(vector<int>::iterator i=OutputVector.begin();i!=OutputVector.end();++i)
    			{
    				if((*i)==(*it)->lineNumber)
    					//    			cout<<"\n LineNo: "<<(*it)->lineNumber<<" Value:"<<(*it)->Value;
    					cout<<(*it)->Value;
    			}
    		}
    	}
    	else if(option==8)
    	{
    		cout<<"\n\nNumber of Nodes: "<<GateCkt.size();
    		cout<<"\n\n*************\n";
    	}
    	else if(option==9)
    	{
    		static int inptLine=0;
    		cout<<"\nEnter Stuck-At-Line numbers and their Values\nEnter Line number as '0' to escape";
    		while(1)
    		{
    			cout<<"\nLine Number>> ";
    			StuckAtLine* temp=new StuckAtLine();
    			cin>>inptLine;
    			if(!inptLine)
    			{
    				inptLine=0;
    				break;
    			}
    			else
    				temp->lineNumber=inptLine;
    			cout<<"      Value>> ";
    			cin>>temp->StuckAtValue;
    			StuckAtVector.push_back(temp);
    			if(isLineFirstInput(inptLine))
    			{
    				cout<<", changing Value of first input line";
    				setValueOfInputLine(inptLine, temp->StuckAtValue);
    			}
    		}
			cout<<"\n\nTotal Stuck-At-Lines: "<<StuckAtVector.size();
    	}
    	else if(option==10)
    	{
    		cout<<"\nStuck-At-Lines: ";
    	    for(vector<StuckAtLine*>::iterator it=StuckAtVector.begin();it!=StuckAtVector.end();++it)
    	    {
    	    	cout<<" "<<(*it)->lineNumber;
    	    }
    	}
    	else if(option==11)
    	{
    		StuckAtVector.clear();
    		cout<<"\n\nClearing StuckAtVector, ";
			cout<<"Total Stuck-At-Lines: "<<StuckAtVector.size();
    	}
    	else if(option==12)
    	{
    		cout<<"\n\nGenerating Random Input Vector";
    		for(int cnt=0; cnt<20; cnt++)
    		{
    			generateRandomInputVector();
    			printInputVector();
    			processGatesWithInput(5);
    			printOutputVector();
    		}
    	}
    	else if(option==13)
    	{
    		readBenchMarkFile(0);
    	}
    	else if(option==14)
    	{
    		cout<<"\n\nBye..!\n\n";
    		break;
    	}
    	else
    		cout<<"\nUnknown option..! Try again";
    }
    return 0;
}

bool calcGateOutput(int _gateType, bool value_one, bool value_two)
{
	bool temp=0;

	if(_gateType == NAND)
	{
		temp = value_one*value_two;
		if(temp==0)
			temp=1;
		else
			temp=0;
	}
	else if(_gateType == AND)
	{
		temp = value_one*value_two;
	}
	else if(_gateType == NOT)
	{
		if(value_two==false)
			temp = true;
		else
			temp = false;
	}
	else if(_gateType == NOR)
	{
		temp = value_one+value_two;
		if(temp==0)
			temp=1;
		else if(temp>=1)
			temp=0;
	}
	else if(_gateType == OR)
	{
		temp = value_one+value_two;
		if(temp==1 || temp==2)
			temp=1;
		else
			temp=0;
	}
	else if(_gateType == XOR)
	{
		temp = value_one+value_two;
		if(temp==2)
			temp=0;
		else if(temp==1)
			temp=1;
		else if(temp==0)
			temp=0;
	}
	else if(_gateType == BUFF)
	{
		temp = value_two;
	}
	else
		cerr<<"halt";

	return temp;
}
