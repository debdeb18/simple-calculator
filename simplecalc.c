#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define filename "inputcalc.txt"

//limit of instructions that can be accepted
#define MAXINSTR 32
#define MAX 50

//global variable
int R[10]; //providing 10 registers
char mem_instr[MAXINSTR][MAX], c;
char(*pc)[MAX], (*mar)[MAX], mdr[MAX], cir[MAX]; //represent main registers inside the CPU
char instr_opnd1[MAX], instr_opnd2[MAX];

//function to load input to memory
void input_to_memory(FILE* file, int mem) {
	int i = 0;
	char temp[20] = { NULL };

	while (1) {
		c = fgetc(file);
		if (c == '\n' || c == EOF) {
			strcpy(mem_instr[mem], temp);
			return;
		}
		temp[i] = c;
		i++;
	}

	return;
}

//function to fetch instruction from memory
void fetch_instr(char* addr) {
	mar = addr;
	strcpy(mdr, mar);
	strcpy(cir, mdr);
	pc++;
	printf("FET: %s\n", cir);
	return;
}

/*
decode_instr() function will specify which one is the opcode, operand1, and operand2.
also what instruction to execute, and specifies if the operand is register or value
*/
int* decode_instr() {
	const char s[4] = " ";
	char* token;
	char instr_opcode[MAX];

	//instruction format [opcode, type, rd, rs, rt]
	int i = 0, type = 0, op = 0, dest = 0, rs = 0, rt = 0;

	//get the 1st token
	token = strtok(cir, s);

	//continue for the rest
	char* temp[4];
	temp[0] = &instr_opcode;
	temp[1] = &instr_opnd1;
	temp[2] = &instr_opnd2;

	//temp[0] = opcode, temp[1] = operand1, temp[2] = operand2
	while (token != NULL) {
		strcpy(temp[i], token);
		i++;
		token = strtok(NULL, s);
	}

	//define opcode for instruction details	
	opcode(instr_opcode, &type, &op);

	switch (op)
	{
	case 1:							//if instruction is J or B
		rs = operand(instr_opnd1);				//define operand1
		rt = 0;
		dest = 0;						//operand2 and destination register is ignored
		break;
	case 2:							//if instruction is M or C or arithmethic calc(+,-,*,/)
		rs = operand(instr_opnd1);				//define operand1
		rt = operand(instr_opnd2);				//define operand2
		dest = 0;						//dest register is not needed (ignored)
		break;
	case 3:							//if instruction is M or C or arithmethic calc(+,-,*,/)
		rs = operand(instr_opnd1);				//define operand1
		rt = operand(instr_opnd2);				//define operand2
		dest = 1;						//destination register is needed
		break;
	default:						//if (op == 0) - instruction is H or error in input
		break;							//do nothing
	}

	//	printf("DEC: %d.%d.%d.%d.%d\n", op, type, dest, rs, rt);

		//make as one decoded instruction
	static int dec_instr[6];
	dec_instr[0] = op;
	dec_instr[1] = type;
	dec_instr[2] = dest;
	dec_instr[3] = rs;
	dec_instr[4] = rt;

	return dec_instr;
}

//extract opcode from instruction
int opcode(char arr[MAX], int* type, int* op) {
	char temp = arr[0];
	switch (temp)
	{
	case 'H':
		//kill program
		*type = 0;
		*op = 0;
		break;
	case 'J':
		*type = 0;
		*op = 1;
		break;
	case 'B':
		*type = 1;
		*op = 1;
		break;
	case 'M':
		*type = 0;
		*op = 2;
		break;
	case 'C':
		*type = 1;
		*op = 2;
		break;
	case '+':
		*type = 0;
		*op = 3;
		break;
	case '-':
		*type = 1;
		*op = 3;
		break;
	case '*':
		*type = 2;
		*op = 3;
		break;
	case '/':
		*type = 3;
		*op = 3;
		break;
	default:
		//error and kill program
		*type = 4;
		*op = 4;
		break;
	}
	return;
}

//extract operand from instruction
int operand(char arr[MAX]) {
	char temp = arr[0];
	char temp1 = arr[1];

	if (temp == 'R') {
		//define a register
		char* ps = arr + 1;
		strcpy(arr, ps);
		return 1;
	}
	else if (temp == '0' && temp1 == 'x') {
		//define a number or value
		return 2;
	}
	else {
		//error, value not recognizable
		return 0;
	}
}

void execute_instr(int* instruction, int end) {
	int op = instruction[0];
	int type = instruction[1];
	int dest = instruction[2];
	int rs = instruction[3];  //operand1
	int rt = instruction[4];  //operand2
	int a, b, ra = 0, rb = 0; //a and b is for operand value, rb and ra is used as limit if operand is register
	int* result = &R[0];	  //all results will be saved in R0
	int cond = R[0];		  //used for comparing purposes

	if (rs == 2) {
		char* ptr;
		a = strtol(instr_opnd1, &ptr, 0);
	}
	else if (rs == 1) {
		int x = atoi(instr_opnd1);
		int* ptr = &R[x];
		a = *ptr;
		ra = atoi(instr_opnd1);
	}

	if (rt == 2) {
		char* ptr1;
		b = strtol(instr_opnd2, &ptr1, 0);
	}
	else if (rt == 1) {
		int x = atoi(instr_opnd2);
		int* ptr1 = &R[x];
		b = *ptr1;
		rb = atoi(instr_opnd2);
	}


	if (op == 0 && type == 0) {
		pc = mem_instr[end];
		return printf("End of Program.\n");
	}
	else if (op == 1) {
		switch (type)
		{
		case 1:
			//branch function
			if (cond == 1) {
				pc = mem_instr[a]; //if true, jump to address
				printf("EXE: PC --> %p", pc);
			}
			break;
		case 0:
			//jump function
			pc = mem_instr[a];
			printf("EXE: PC --> %p", pc);
			break;
		default:
			printf("error\n");
			pc = mem_instr[end];
			break;
		}
	}
	else if (ra < 10 && rb < 10) {
		switch (op)
		{
		case 3:
			switch (type)
			{
			case 3:
				//divide
				*result = a / b;
				printf("EXE: R0: %d --> %d / %d\n", R[0], a, b);
				break;
			case 2:
				//multiply
				*result = a * b;
				printf("EXE: R0: %d --> %d * %d\n", R[0], a, b);
				break;
			case 1:
				//substraction
				*result = a - b;
				printf("EXE: R0: %d --> %d - %d\n", R[0], a, b);
				break;
			case 0:
				//addition
				*result = a + b;
				printf("EXE: R0: %d --> %d + %d\n", R[0], a, b);
				break;
			default:
				printf("EXE: error\n");
				pc = mem_instr[end];
				break;
			}
			break;
		case 2:
			switch (type)
			{
			case 1:
				//compare function
				if (a >= b) {
					*result = 0;
				}
				else {
					*result = 1;
				}
				printf("EXE: R0: %d --> %d >= %d ? 0 : 1\n", R[0], a, b);
				break;
			case 0:
				//move function
				R[atoi(instr_opnd1)] = b;
				printf("EXE: R%d: %d\n", atoi(instr_opnd1), b);
				break;
			default:
				printf("EXE: error\n");
				pc = mem_instr[end];
				break;
			}
			break;
		default:
			printf("error\n");
			pc = mem_instr[end];
			break;
		}
		printf("\n");
	}
	else {
		printf("error. program terminated\n");
		pc = mem_instr[end];
	}
}





int main() {
	FILE* file;
	int s_addr = 0;

	//initializing pc
	pc = mem_instr[0];
	int* dec_instr;

	//open file
	file = fopen(filename, "r");
	if (file == NULL) {
		printf("filecannot be open\n");
		return 0;
	}

	//put input from text file to memory
	do {
		input_to_memory(file, s_addr);
		s_addr++;
	} while (c != EOF);

	//start fetch-decode-execude cycle
	while (pc != mem_instr[s_addr]) {
		fetch_instr(pc);
		dec_instr = decode_instr(); //Control Unit tells the system what to do
		execute_instr(dec_instr, s_addr); //executing according to given signals
	}

	//close file
	fclose(file);
	return 0;

}
