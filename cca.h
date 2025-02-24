/************************************=== CCAnr ===***************************************          
 ** CCAnr is a local search solver for the Boolean Satisfiability (SAT) problem,
 ** which is especially designed for non-random instances.
 ** CCAnr is designed and implemented by Shaowei Cai (email: shaoweicai.cs@gmail.com),                                        
 *****************************************************************************************/

/*****************************=== Develpment history ===*************************************
** 2011.5
** SWCC (Smoothed Weighting and Configuration Checking) by Shaowei Cai
** New Idea: Configuration Checking (CC)
** A variable is configuration changed, if since its last flip, at least one of its 
** neighboring var has been flipped. 
** In the greedy mode, Swcc picks the best Configuration Changed Decreasing  var to flip.
** In the random mode, it updates weights, and flips the oldest var in a random unsat clause.
 
** 2011.9 
** SWCCA (Smoothed Weighting and Configuration Checking with Aspiration) by Shaowei Cai                 
** New Idea: CC with Aspiration (CCA)
** Modification: in greedy mode, it first prefers to flip the best CCD var. If there is 
** no CCD variable, then flip the best significant decreasing var, i.e., with a great 
** positive score (in Swcca, bigger than averaged clause weight), if there exsit such vars.                  
 
** 2013.4	
** CCAnr (CCA for non-random SAT)
** Modifications: Generalize the smoothig fomula as w(ci)=w(ci)*p+ave_w*q; pick the greediest
** variable in the diversification mode.
************************************************************************************************/


#ifndef _CCA_H_
#define _CCA_H_

#include "basis.h"

#define pop(stack) stack[--stack ## _fill_pointer]
#define push(item, stack) stack[stack ## _fill_pointer++] = item



inline void unsat(int clause)
{
	index_in_unsat_stack[clause] = unsat_stack_fill_pointer;
	push(clause,unsat_stack);
	
	//update appreance count of each var in unsat clause and update stack of vars in unsat clauses
	int v;
	for(lit* p=clause_lit[clause]; (v=p->var_num)!=0; p++)
	{	
		unsat_app_count[v]++;
		if(unsat_app_count[v]==1)
		{
			index_in_unsatvar_stack[v] = unsatvar_stack_fill_pointer;
			push(v,unsatvar_stack);	
		}
	}
}


inline void sat(int clause)
{
	int index,last_unsat_clause;

	//since the clause is satisfied, its position can be reused to store the last_unsat_clause
	last_unsat_clause = pop(unsat_stack);
	index = index_in_unsat_stack[clause];
	unsat_stack[index] = last_unsat_clause;
	index_in_unsat_stack[last_unsat_clause] = index;
	
	//update appreance count of each var in unsat clause and update stack of vars in unsat clauses
	int v,last_unsat_var;
	for(lit* p=clause_lit[clause]; (v=p->var_num)!=0; p++)
	{	
		unsat_app_count[v]--;
		if(unsat_app_count[v]==0)
		{
			last_unsat_var = pop(unsatvar_stack);
			index = index_in_unsatvar_stack[v];
			unsatvar_stack[index] = last_unsat_var;
			index_in_unsatvar_stack[last_unsat_var] = index;
		}
	}
}

//initiation of the algorithm
void init()
{
	int 		v,c;
	int			i,j;
	int			clause;
	count_FRW=0;
	
	least_unsatClauses=num_clauses;
	count_candidate_falseClause_first=0;
	count_candidate_falseClause_second=0;	
	count_SelectT_first=0;
	count_SelectT_second=0;
	//Initialize edge weights
//	for (c = 0; c<num_clauses; c++)
//		clause_weight[c] = 1;
// Initialize candidate clause

	for (c = 0; c<num_clauses; c++){
		clause_weight[c] = 1;//Initialize edge weights
		clause_reward[c]=1.0;
		whereCandidate_first[c]=0;
		whereCandidate_second[c]=0;
		candidate_falseClause_first_stack[c]=0;
		candidate_falseClause_second_stack[c]=0;
		count_selectCaluse[c]=0;
		isCandidate_first[c]=0;
		isCandidate_second[c]=0;
		time_stamp_clause[c]=0;
	}

	//init unsat_stack
	unsat_stack_fill_pointer = 0;
	unsatvar_stack_fill_pointer = 0;

	//init solution
	for (v = 1; v <= num_vars; v++) {
        vSelect[v]=0;
        var_reward[v]=1.0;
        if(fix[v]==0){
            if(rand()%2==1) cur_soln[v] = 1;
            else cur_soln[v] = 0;
			time_stamp[v] = 0;
			conf_change[v] = 1;
			unsat_app_count[v] = 0;
		
			//pscore[v] = 0;
		}
       best_atom[v]=cur_soln[v];
       breaks[v]=0;
	}

	/* figure out sat_count, and init unsat_stack */
	for (c=0; c<num_clauses; ++c) 
	{
		if(clause_delete[c]==1) continue;
		
		sat_count[c] = 0;
		
		for(j=0; j<clause_lit_count[c]; ++j)
		{
			if (cur_soln[clause_lit[c][j].var_num] == clause_lit[c][j].sense)
			{
				sat_count[c]++;
				sat_var[c] = clause_lit[c][j].var_num;	
			}
		}

		if (sat_count[c] == 0) 
			unsat(c);
		else if(sat_count[c] == 1){
			breaks[sat_var[c]]++;
		} 
	}

	/*figure out var score*/
	int lit_count;
	for (v=1; v<=num_vars; v++) 
	{
		if(fix[v]==1) 
		{
			score[v] = -100000;
			continue;
		}
		
		score[v] = 0;

		lit_count = var_lit_count[v];
		
		for(i=0; i<lit_count; ++i)
		{
			c = var_lit[v][i].clause_num;
			if (sat_count[c]==0) score[v]++;
			else if (sat_count[c]==1 && var_lit[v][i].sense==cur_soln[v]) score[v]--;
		}
	}
	
	/*
	int flag;
	//compute pscore and record sat_var and sat_var2 for 2sat clauses
	for (c=0; c<num_clauses; ++c) 
	{
		if(clause_delete[c]==1) continue;
		
		if (sat_count[c]==1)
		{
			for(j=0;j<clause_lit_count[c];++j)
			{
				v=clause_lit[c][j].var_num;
				if(v!=sat_var[c])pscore[v]++;
			}
		}
		else if(sat_count[c]==2)
		{
			flag=0;
			for(j=0;j<clause_lit_count[c];++j)
			{
				v=clause_lit[c][j].var_num;
				if(clause_lit[c][j].sense == cur_soln[v])
				{
					pscore[v]--;
					if(flag==0){sat_var[c] = v; flag=1;}
					else	{sat_var2[c] = v; break;}
				}
			}
		
		}
	}
	*/
	
		
	//init goodvars stack
	goodvar_stack_fill_pointer = 0;
	for (v=1; v<=num_vars; v++) 
	{
		if(fix[v]==1)  continue;
		if(score[v]>0)// && conf_change[v]==1)
		{
			already_in_goodvar_stack[v] = 1;
			push(v,goodvar_stack);
			
		}
		else already_in_goodvar_stack[v] = 0;
	}
	
	//setting for the virtual var 0
	time_stamp[0]=0;
	var_reward[0]=1.0;

	//pscore[0]=0;
}


void flip(int flipvar)
{
	cur_soln[flipvar] = 1 - cur_soln[flipvar];

	int i,j;
	int v,c;

	lit* clause_c;
	int org_flipvar_score = score[flipvar];
	//update related clauses and neighbor vars

	for(lit *q = var_lit[flipvar]; (c=q->clause_num)>=0; q++)
	{
		clause_c = clause_lit[c];

		if(cur_soln[flipvar] == q->sense)
		{
			++sat_count[c];
			
			if (sat_count[c] == 2) //sat_count from 1 to 2
				{
					score[sat_var[c]] += clause_weight[c];
					breaks[sat_var[c]]--;
				}
			else if (sat_count[c] == 1) // sat_count from 0 to 1
			{
				sat_var[c] = flipvar;//record the only true lit's var
				for(lit* p=clause_c; (v=p->var_num)!=0; p++) score[v] -= clause_weight[c];
                
				sat(c);
				breaks[flipvar]++;

		//		if(count_selectCaluse[c]>Beta_second){
			   	    if(isCandidate_first[c]==1){//new
				       isCandidate_first[c]=0;
				       count_candidate_falseClause_first--;
				       candidate_falseClause_first_stack[whereCandidate_first[c]]=candidate_falseClause_first_stack[count_candidate_falseClause_first];
				       whereCandidate_first[candidate_falseClause_first_stack[count_candidate_falseClause_first]]=whereCandidate_first[c];
				
				   }
				    if(isCandidate_second[c]==1){
				        isCandidate_second[c]=0;
				        count_candidate_falseClause_second--;
				        candidate_falseClause_second_stack[whereCandidate_second[c]]=candidate_falseClause_second_stack[count_candidate_falseClause_second];
				        whereCandidate_second[candidate_falseClause_second_stack[count_candidate_falseClause_second]]=whereCandidate_second[c];	
				     }
	    	 //  }
			}
		}
		else // cur_soln[flipvar] != cur_lit.sense
		{
			--sat_count[c];
			if (sat_count[c] == 1) //sat_count from 2 to 1
			{
				
				for(lit* p=clause_c; (v=p->var_num)!=0; p++) 
				{
					if(p->sense == cur_soln[v] )
					{
						score[v] -= clause_weight[c];
						sat_var[c] = v;
						breaks[v]++;
						break;
					}
				}
			}
			else if (sat_count[c] == 0) //sat_count from 1 to 0
			{

				for(lit* p=clause_c; (v=p->var_num)!=0; p++) score[v] += clause_weight[c];
				unsat(c);
				breaks[flipvar]--;
		
			if(count_selectCaluse[c]>Beta_first){
				if((isCandidate_second[c]==0)&&(isCandidate_first[c]==0)){
					isCandidate_first[c]=1;
					candidate_falseClause_first_stack[count_candidate_falseClause_first]=c;
					whereCandidate_first[c]=count_candidate_falseClause_first;
					count_candidate_falseClause_first++;				
				}
				else if((isCandidate_second[c]==1)&&(isCandidate_first[c]==0)){
					isCandidate_second[c]=0;
					count_candidate_falseClause_second--;
					candidate_falseClause_second_stack[whereCandidate_second[c]]=candidate_falseClause_second_stack[count_candidate_falseClause_second];
					whereCandidate_second[candidate_falseClause_second_stack[count_candidate_falseClause_second]]=whereCandidate_second[c];					
					isCandidate_first[c]=1;
					candidate_falseClause_first_stack[count_candidate_falseClause_first]=c;
					whereCandidate_first[c]=count_candidate_falseClause_first;
					count_candidate_falseClause_first++;				
				}
			}
			if((count_selectCaluse[c]>Beta_second)&&(count_selectCaluse[c]<Beta_first)&&(isCandidate_second[c]==0)){
					isCandidate_second[c]=1;
					candidate_falseClause_second_stack[count_candidate_falseClause_second]=c;
					whereCandidate_second[c]=count_candidate_falseClause_second;
					count_candidate_falseClause_second++;
				} 
//		      	if(count_FRW>0) printf("flip mediun 2\t\n");
		   	}//end else if
			
		}//end else
	}
//	if(count_FRW>0) printf("flipmedium\t\n");
	score[flipvar] = -org_flipvar_score;

	/*update CCD */
	int index;
	
	conf_change[flipvar] = 0;
	//remove the vars no longer goodvar in goodvar stack 
	for(index=goodvar_stack_fill_pointer-1; index>=0; index--)
	{
		v = goodvar_stack[index];
		if(score[v]<=0)
		{
			goodvar_stack[index] = pop(goodvar_stack);
			already_in_goodvar_stack[v] = 0;
		}	
	}

    int* p;
	for(p=var_neighbor[flipvar]; (v=*p)!=0; p++) 
	{
		conf_change[v] = 1;
		if(score[v]>0 && already_in_goodvar_stack[v] ==0)
		{                                                 
			push(v,goodvar_stack);
			already_in_goodvar_stack[v] = 1;
		}
	}

}

#endif
