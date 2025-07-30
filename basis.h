#ifndef _BASIS_H_
#define _BASIS_H_

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <stdlib.h>
#include <stdint.h>

// Branch prediction hints for better performance
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

// Prefetch hints for cache optimization
#define PREFETCH_READ(addr)   __builtin_prefetch(addr, 0, 3)
#define PREFETCH_WRITE(addr)  __builtin_prefetch(addr, 1, 3)

//#include <stdio.h>
//#include <unistd.h>
//#include <string.h>
//#include <limits.h>
//#include <float.h>
//#include <getopt.h>
//#include <signal.h>
//#include <assert.h>

using namespace std;

// Performance optimization: Lookup tables for expensive math operations
#define MAX_LOOKUP_SIZE 100000
double sqrt_lookup[MAX_LOOKUP_SIZE];
double log_lookup[MAX_LOOKUP_SIZE];
double log2_lookup[MAX_LOOKUP_SIZE];
bool lookup_tables_initialized = false;

// Optimized math functions using lookup tables
inline double fast_sqrt(double x) {
    int idx = static_cast<int>(x);
    if (idx >= 0 && idx < MAX_LOOKUP_SIZE) {
        return sqrt_lookup[idx];
    }
    return sqrt(x);
}

inline double fast_log(double x) {
    int idx = static_cast<int>(x);
    if (idx >= 1 && idx < MAX_LOOKUP_SIZE) {
        return log_lookup[idx];
    }
    return log(x);
}

inline double fast_log2(double x) {
    int idx = static_cast<int>(x);
    if (idx >= 1 && idx < MAX_LOOKUP_SIZE) {
        return log2_lookup[idx];
    }
    return log2(x);
}

// Initialize lookup tables
inline void init_math_lookup_tables() {
    if (!lookup_tables_initialized) {
        for (int i = 0; i < MAX_LOOKUP_SIZE; i++) {
            sqrt_lookup[i] = sqrt(i);
            if (i > 0) {
                log_lookup[i] = log(i);
                log2_lookup[i] = log2(i);
            }
        }
        lookup_tables_initialized = true;
    }
}

// Fast random number generator using xorshift
static uint32_t xorshift_state = 123456789;

inline uint32_t fast_rand() {
    xorshift_state ^= xorshift_state << 13;
    xorshift_state ^= xorshift_state >> 17;
    xorshift_state ^= xorshift_state << 5;
    return xorshift_state;
}

// Fast random double between 0.0 and 1.0
inline double fast_rand_double() {
    return fast_rand() * (1.0 / 4294967296.0);
}

// Set seed for fast random generator
inline void fast_srand(uint32_t seed) {
    xorshift_state = seed ? seed : 123456789;
}

enum type{SAT3, SAT4 ,SAT5, SAT6, SAT7, strSAT} probtype;

/* limits on the size of the problem. */
#define MAX_VARS    4000010
#define MAX_CLAUSES 20000000

// Define a data structure for a literal in the SAT problem.
struct lit {
    int clause_num;		//clause num, begin with 0
    int var_num;		//variable num, begin with 1
    int sense;			//is 1 for true literals, 0 for false literals.
};

/*parameters of the instance*/
int     num_vars;		//var index from 1 to num_vars
int     num_clauses;	//clause index from 0 to num_clauses-1
int		max_clause_len;
int		min_clause_len;
int		formula_len=0;
double	avg_clause_len;
double 	ratio;

/* literal arrays */				
lit*	var_lit[MAX_VARS];				//var_lit[i][j] means the j'th literal of var i.
int		var_lit_count[MAX_VARS];        //amount of literals of each var
lit*	clause_lit[MAX_CLAUSES];		//clause_lit[i][j] means the j'th literal of clause i.
int		clause_lit_count[MAX_CLAUSES]; 	// amount of literals in each clause				
//new
//int*	var_poslit[MAX_VARS];				//var_lit[v][j]: the clause number of the clause that the j'th positive literal of v occurs in.
//int*	var_neglit[MAX_VARS];
//int		var_poslit_count[MAX_VARS];
//int		var_neglit_count[MAX_VARS];

/* Information about the selected clauses */ 

int*    time_stamp_clause;
int*    count_selectCaluse;//	amount of clause selected
int*    isCandidate_first;// Is it a candidate clause
int*    isCandidate_second;// Is it a candidate clause
int     count_candidate_falseClause_first;//Number of first candidate clauses
int     count_candidate_falseClause_second;//Number of first candidate clauses
int*     candidate_falseClause_first_stack;//candidate unsat clause
int*     candidate_falseClause_second_stack;//candidate unsat clause
int*     whereCandidate_first;//the place of candidate unsat clause
int*     whereCandidate_second;//the place of candidate unsat clause
int     count_SelectT_first;
int     count_SelectT_second;
int     Beta_second;
int     Beta_first;
int     count_FRW;
int     count_breaktingTie=0;
int     best_var;
int     least_unsatClauses;

double  ucbc=4;
double  p_first;
double  p_second;
double  cb;
double  eps;
double probsBreak[10000];
double		*var_reward;
double     *clause_reward;
int     gama;	
/** contains the probabilities of the variables from one unsatisfied clause*/


lit*	org_clause_lit[MAX_CLAUSES];		//clause_lit[i][j] means the j'th literal of clause i.
int*	org_clause_lit_count; 	// org_clause_lit_count[MAX_CLAUSES]amount of literals in each clause 
int		simplify=0;
			
/* Information about the variables. */
int     breaks[MAX_VARS];
int     score[MAX_VARS];				
int		time_stamp[MAX_VARS];
int		conf_change[MAX_VARS];
int*	var_neighbor[MAX_VARS];
int 	var_neighbor_count[MAX_VARS];//var_neighbor_count[MAX_VARS]
int*     vSelect;

int     best_atom[MAX_VARS];
//int		pscore[MAX_VARS];
int		fix[MAX_VARS];


/* Information about the clauses */			
int*     clause_weight;//clause_weight[MAX_CLAUSES]		
int*     sat_count;//sat_count[MAX_CLAUSES]			
int*		sat_var;//sat_var[MAX_CLAUSES]
//int		sat_var2[MAX_CLAUSES];

//unsat clauses stack
int*	unsat_stack;		//unsat_stack[MAX_CLAUSES]store the unsat clause number
int		unsat_stack_fill_pointer;
int*	index_in_unsat_stack;//index_in_unsat_stack[MAX_CLAUSES]which position is a clause in the unsat_stack

//variables in unsat clauses
int		unsatvar_stack[MAX_VARS];		
int		unsatvar_stack_fill_pointer;
int		index_in_unsatvar_stack[MAX_VARS];
int		unsat_app_count[MAX_VARS];		//a varible appears in how many unsat clauses

//configuration changed decreasing variables (score>0 and confchange=1)
int		goodvar_stack[MAX_VARS];		
int		goodvar_stack_fill_pointer;
int		already_in_goodvar_stack[MAX_VARS];

//unit clauses preprocess
lit		unitclause_queue[MAX_VARS];		
int		unitclause_queue_beg_pointer=0;
int     unitclause_queue_end_pointer=0;
int     clause_delete[MAX_CLAUSES];

/* Information about solution */
int             cur_soln[MAX_VARS];	//the current solution, with 1's for True variables, and 0's for False variables

//cutoff
int		max_tries = 10000000;
int		tries;
int		max_flips = 2000000000;
int		step;

int fct = 0; //function to use 0= poly 1=exp 
void (*initLookUpTable)() = NULL;

void setup_datastructure();
void free_memory();
int build_instance(char *filename);
void build_neighbor_relation();

void free_memory()
{
	int i;
	for (i = 0; i < num_clauses; i++) 
	{
		delete[] clause_lit[i];
	}
	
	for(i=1; i<=num_vars; ++i)
	{
		delete[] var_lit[i];
		delete[] var_neighbor[i];
//		delete[] var_poslit[i];
//		delete[] var_neglit[i];
	}
	
	// Free aligned allocated memory
	free(var_reward);
	free(vSelect);
	free(clause_weight);
	free(clause_reward);
	free(sat_count);
	free(sat_var);
	free(unsat_stack);
	free(index_in_unsat_stack);
	free(org_clause_lit_count);
	free(time_stamp_clause);
	free(count_selectCaluse);
	free(isCandidate_first);
	free(isCandidate_second);
	free(candidate_falseClause_first_stack);
	free(candidate_falseClause_second_stack);
	free(whereCandidate_first);
	free(whereCandidate_second);
}
inline void allocateMemory() {
	// Initialize lookup tables for optimized math operations
	init_math_lookup_tables();
	
	// Allocating memory for the instance data (independent from the assignment).
	// Using aligned allocation for better cache performance
	
	// Allocating memory for the assignment dependent data.
	var_reward= (double*) aligned_alloc(64, sizeof(double) * (num_vars + 1));
	vSelect=(int*) aligned_alloc(64, sizeof(int) * (num_vars + 1));
	clause_weight= (int*) aligned_alloc(64, sizeof(int) * (num_clauses + 1));
	clause_reward= (double*) aligned_alloc(64, sizeof(double) * (num_clauses + 1));
	sat_count= (int*) aligned_alloc(64, sizeof(int) * (num_clauses + 1));
	sat_var= (int*) aligned_alloc(64, sizeof(int) * (num_clauses + 1));
	unsat_stack= (int*) aligned_alloc(64, sizeof(int) * (num_clauses + 1));
	index_in_unsat_stack= (int*) aligned_alloc(64, sizeof(int) * (num_clauses + 1));
	
    org_clause_lit_count= (int*) aligned_alloc(64, sizeof(int) * (num_clauses + 1));
    time_stamp_clause= (int*) aligned_alloc(64, sizeof(int) * (num_clauses + 1));
	count_selectCaluse =(int*) aligned_alloc(64, sizeof(int) * (num_clauses + 1));
	isCandidate_first=(int*) aligned_alloc(64, sizeof(int) * (num_clauses + 1));
	isCandidate_second=(int*) aligned_alloc(64, sizeof(int) * (num_clauses + 1));

	candidate_falseClause_first_stack=(int*) aligned_alloc(64, sizeof(int) * (num_clauses + 1));
	candidate_falseClause_second_stack=(int*) aligned_alloc(64, sizeof(int) * (num_clauses + 1));
	whereCandidate_first=(int*) aligned_alloc(64, sizeof(int) * (num_clauses + 1));
	whereCandidate_second=(int*) aligned_alloc(64, sizeof(int) * (num_clauses + 1));

	// Initialize arrays to zero for better cache behavior
	if (var_reward) memset(var_reward, 0, sizeof(double) * (num_vars + 1));
	if (vSelect) memset(vSelect, 0, sizeof(int) * (num_vars + 1));
	if (clause_weight) memset(clause_weight, 0, sizeof(int) * (num_clauses + 1));
	if (clause_reward) memset(clause_reward, 0, sizeof(double) * (num_clauses + 1));
}
/*
 * Read in the problem.
 */
int temp_lit[MAX_VARS]; //the max length of a clause can be MAX_VARS
int build_instance(char *filename)
{
	char    line[1000000];
	char    tempstr1[10];
	char    tempstr2[10];
	int     cur_lit;
	int     i,j;
	int		v,c;//var, clause
	
	ifstream infile(filename);
	//if(infile==NULL)
	//	return 0;

	/*** build problem data structures of the instance ***/
	infile.getline(line,1000000);
	while (line[0] != 'p')
		infile.getline(line,1000000);

	sscanf(line, "%s %s %d %d", tempstr1, tempstr2, &num_vars, &num_clauses);
	ratio = double(num_clauses)/num_vars;
	allocateMemory();//new
	
	if(num_vars>=MAX_VARS || num_clauses>=MAX_CLAUSES)
	{
		cout<<"the size of instance exceeds out limitation, please enlarge MAX_VARS and (or) MAX_CLAUSES."<<endl;
		exit(-1);
	}
    
	for (c = 0; c < num_clauses; c++) 
	{
		clause_lit_count[c] = 0;
		clause_delete[c] = 0;
	}
	for (v=1; v<=num_vars; ++v)
	{
		var_lit_count[v] = 0;
		fix[v] = 0;
	}
		
	max_clause_len = 0;
	min_clause_len = num_vars;
		
	//Now, read the clauses, one at a time.
	for (c = 0; c < num_clauses; c++) 
	{
		infile>>cur_lit;

		while (cur_lit != 0) { 
			temp_lit[clause_lit_count[c]] = cur_lit;
			clause_lit_count[c]++;
		
			infile>>cur_lit;
		}
		
		clause_lit[c] = new lit[clause_lit_count[c]+1];
		
		for(i=0; i<clause_lit_count[c]; ++i)
		{
			clause_lit[c][i].clause_num = c;
			clause_lit[c][i].var_num = abs(temp_lit[i]);
			if (temp_lit[i] > 0) {
				clause_lit[c][i].sense = 1;

			}
				else {

					clause_lit[c][i].sense = 0;
				}
			
			var_lit_count[clause_lit[c][i].var_num]++;
		}
		clause_lit[c][i].var_num=0; 
		clause_lit[c][i].clause_num = -1;
        
        //unit clause
        if(clause_lit_count[c]==1)
        {
            unitclause_queue[unitclause_queue_end_pointer++] = clause_lit[c][0];
            clause_delete[c]=1;
        }
		
		if(clause_lit_count[c] > max_clause_len)
			max_clause_len = clause_lit_count[c];
		else if(clause_lit_count[c] < min_clause_len)
			min_clause_len = clause_lit_count[c];
			
		formula_len += clause_lit_count[c];
	}
	infile.close();
//	delete[] temp_lit;
	
	avg_clause_len = (double)formula_len/num_clauses;
	
	if(unitclause_queue_end_pointer>0)
	{
		simplify = 1;
		for (c = 0; c < num_clauses; c++) 
		{
			org_clause_lit_count[c] = clause_lit_count[c];
			org_clause_lit[c] = new lit[clause_lit_count[c]+1];
			for(i=0; i<org_clause_lit_count[c]; ++i)
			{
				org_clause_lit[c][i] = clause_lit[c][i];
			}
			
		}
	}

	
	//creat var literal arrays
	for (v=1; v<=num_vars; ++v)
	{
		var_lit[v] = new lit[var_lit_count[v]+1];
		var_lit_count[v] = 0;	//reset to 0, for build up the array
	}
	//scan all clauses to build up var literal arrays
	for (c = 0; c < num_clauses; ++c) 
	{
		for(i=0; i<clause_lit_count[c]; ++i)
		{
			v = clause_lit[c][i].var_num;
			var_lit[v][var_lit_count[v]] = clause_lit[c][i];
			++var_lit_count[v];
		}
	}

	for (v=1; v<=num_vars; ++v) //set boundary num_vars
	{
		var_lit[v][var_lit_count[v]].clause_num=-1;
	
	}
		
		//problem type
	if(max_clause_len == min_clause_len)
	{
		if(max_clause_len<=3) probtype=SAT3;
		else if(max_clause_len<=4) probtype=SAT4;
		else if(max_clause_len<=5) probtype=SAT5;
		else if(max_clause_len<=6) probtype=SAT6;
		else probtype=SAT7;
	}
	else {
		probtype=strSAT;
	}
	
	return 1;
}
void initPoly() {
	int i;
	for (i = 0; i <10000; i++) {
		probsBreak[i] = pow((eps + i), -cb);
		//probsBreak2[i] = pow((1+i),-cb2);
	}
}

void initExp() {
	int i;
	for (i = 0; i < 10000; i++) {
		probsBreak[i] = pow(cb, -i);
	//	probsBreak2[i] = pow(cb2, -i);
	}
}

void build_neighbor_relation()
{
	int*	neighbor_flag = new int[num_vars+1];
	int		i,j,count;
	int 	v,c;

	for(v=1; v<=num_vars; ++v)
	{
		var_neighbor_count[v] = 0;
		
		if(fix[v]==1) continue;

		for(i=1; i<=num_vars; ++i)
			neighbor_flag[i] = 0;
		neighbor_flag[v] = 1;
		
		for(i=0; i<var_lit_count[v]; ++i)
		{
			c = var_lit[v][i].clause_num;
			if(clause_delete[c]==1) continue;
			
			for(j=0; j<clause_lit_count[c]; ++j)
			{
				if(neighbor_flag[clause_lit[c][j].var_num]==0)
				{
					var_neighbor_count[v]++;
					neighbor_flag[clause_lit[c][j].var_num] = 1;
				}
			}
		}

		neighbor_flag[v] = 0;
 
		var_neighbor[v] = new int[var_neighbor_count[v]+1];

		count = 0;
		for(i=1; i<=num_vars; ++i)
		{
			if(fix[i]==1) continue;
			
			if(neighbor_flag[i]==1)
			{
				var_neighbor[v][count] = i;
				count++;
			}
		}
		var_neighbor[v][count]=0;
	}
	
	delete[] neighbor_flag; neighbor_flag=NULL;
}


void print_solution()
{
     int    i;

     cout<<"v ";
     for (i=1; i<=num_vars; i++) {
         if(cur_soln[i]==0) cout<<"-";
         cout<<i;
         if(i%10==0) cout<<endl<<"v ";
         else	cout<<' ';
     }
     cout<<"0"<<endl;
}


int verify_sol()
{
	int c,j; 
	int flag;
	
	if(simplify==0)
	{
	
		for (c = 0; c<num_clauses; ++c) 
		{
			flag = 0;
			for(j=0; j<clause_lit_count[c]; ++j)
				if (cur_soln[clause_lit[c][j].var_num] == clause_lit[c][j].sense) {flag = 1; break;}

			if(flag ==0){//output the clause unsatisfied by the solution
				cout<<"c clause "<<c<<" is not satisfied"<<endl;
			
				cout<<"c ";
				for(j=0; j<clause_lit_count[c]; ++j)
				{
					if(clause_lit[c][j].sense==0)cout<<"-";
					cout<<clause_lit[c][j].var_num<<" ";
				}
				cout<<endl;
			
				for(j=0; j<clause_lit_count[c]; ++j)
					cout<<cur_soln[clause_lit[c][j].var_num]<<" ";
				cout<<endl;

				return 0;
			}
		}
	}
	
	if(simplify==1)
	{
		for (c = 0; c<num_clauses; ++c) 
		{
			flag = 0;
			for(j=0; j<org_clause_lit_count[c]; ++j)
				if (cur_soln[org_clause_lit[c][j].var_num] == org_clause_lit[c][j].sense) {flag = 1; break;}

			if(flag ==0){//output the clause unsatisfied by the solution
				cout<<"c clause "<<c<<" is not satisfied"<<endl;
				
				if(clause_delete[c]==1)cout<<"c this clause is deleted by UP."<<endl;
			
				cout<<"c ";
				for(j=0; j<org_clause_lit_count[c]; ++j)
				{
					if(org_clause_lit[c][j].sense==0)cout<<"-";
					cout<<org_clause_lit[c][j].var_num<<" ";
				}
				cout<<endl;
			
				for(j=0; j<org_clause_lit_count[c]; ++j)
					cout<<cur_soln[org_clause_lit[c][j].var_num]<<" ";
				cout<<endl;

				return 0;
			}
		}
		
	}
	
	return 1;
}

#endif
