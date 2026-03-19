#include "basis.h"
#include "cca.h"
#include "cw.h"
#include "preprocessor.h"

#include <sys/times.h> //these two h files are for linux
#include <unistd.h>

//pick a var to be flip
int pick_var() {
	int         i,k,c,v,ci;
	int         bestVar=best_var;
	lit*		clause_c;
	double      randPosition_p,sumProb,randPosition,tempBest_var,tempV;
	int		    *truep,bbreakv,countBestV,bestV[goodvar_stack_fill_pointer];
	double      probs[max_clause_len];
	double      probCcd[goodvar_stack_fill_pointer],sumProbCcd;

	/**Greedy Mode**/
	/*CCD (configuration changed decreasing) mode, the level with configuation chekcing*/
	if(likely(goodvar_stack_fill_pointer>0))
	{
		best_var = goodvar_stack[0];
		
		for(i=1; i<goodvar_stack_fill_pointer; ++i)
		{
			v=goodvar_stack[i];
			if(score[v]>score[best_var]) best_var = v;
			else if(score[v]==score[best_var] && time_stamp[v]<time_stamp[best_var]) best_var = v;
		}
		
		return best_var;
	}

	/*SD (significant decreasing) mode, the level with aspiration*/
	best_var = 0;
	for(i=0; i<unsatvar_stack_fill_pointer; ++i)
	{
		if(score[unsatvar_stack[i]]>sigscore) 
		{
			best_var = unsatvar_stack[i];
			break;
		}
	}

	for(++i; i<unsatvar_stack_fill_pointer; ++i)
	{
		v=unsatvar_stack[i];
		if(score[v]>score[best_var]) best_var = v;
		else if(score[v]==score[best_var] && time_stamp[v]<time_stamp[best_var]) best_var = v;
	}
		
	if(best_var!=0) return best_var;
	/**Diversification Mode**/

	update_clause_weights();

	/*focused random walk*/
	count_FRW++;
	randPosition_p = fast_rand_double();
	
	/*three-level clause selection strategy*/
	if(randPosition_p<p_first) {
		if(count_candidate_falseClause_first>0) {
			c =candidate_falseClause_first_stack[fast_rand()%count_candidate_falseClause_first];
			count_SelectT_first++;
		} 
		else{
			c=unsat_stack[fast_rand()%unsat_stack_fill_pointer];
		}	
	} 
	else if((randPosition_p>=p_first)&&(randPosition_p<(p_first+p_second))) {
		if(count_candidate_falseClause_second>0) {
			c =candidate_falseClause_second_stack[fast_rand()%count_candidate_falseClause_second];
			count_SelectT_second++;
		} 
		else{
			c=unsat_stack[fast_rand()%unsat_stack_fill_pointer];
		}	
		
	} 
	/*MAB for selecting a clause*/
	else {
				double best_ucb,temp_ucb;
				int cl;
				c=unsat_stack[0];
		   		best_ucb=clause_reward[c]/(count_selectCaluse[c]+1)+fast_sqrt(ucbc*fast_log(count_FRW+1)/(count_selectCaluse[c]+1));				
				for(int i=1;i<unsat_stack_fill_pointer;i++){
					cl=unsat_stack[i];
					temp_ucb=clause_reward[cl]/(count_selectCaluse[cl]+1)+fast_sqrt(ucbc*fast_log(count_FRW+1)/(count_selectCaluse[cl]+1));	
					if(temp_ucb>best_ucb) {
						best_ucb=temp_ucb;
						c=cl;
					}
	
				}
				
			}

	count_selectCaluse[c]++;
	clause_reward[c]+=fast_log2(step+1-time_stamp_clause[c]);
	time_stamp_clause[c]=step;
	
	clause_c = clause_lit[c];
	/*probability-based variable selection*/	
	for(k=0; k<clause_lit_count[c]; ++k) {
		v=clause_c[k].var_num;

		probs[k] =probsBreak[breaks[v]];
	
		sumProb += probs[k];
	}
	randPosition = fast_rand_double() * sumProb;
	for (k = k - 1; k != 0; k--) {
		sumProb -= probs[k];
		if (sumProb <= randPosition)
			break;
	}
	best_var = clause_c[k].var_num;

	if(bestVar!=best_var) 
	{
		return best_var;	
	}
	/*tie-breaking employing MAB for selecting a variable*/	
	else {
		count_breaktingTie++;
		tempBest_var=var_reward[best_var]/(vSelect[best_var]+1)+fast_sqrt(ucbc*fast_log(step+1)/(vSelect[best_var]+1));	
		for(k=0; k<clause_lit_count[c]; ++k) {
			v=clause_c[k].var_num;
			if(score[v]>score[best_var]) best_var = v;
			else if(score[v]==score[best_var]) {
				tempV=var_reward[v]/(vSelect[v]+1)+fast_sqrt(ucbc*fast_log(step+1)/(vSelect[v]+1));
				if(tempV>tempBest_var){
						best_var = v;
						tempBest_var=tempV;
				}
			}
		}
		return best_var;
	}


}


//set functions in the algorithm
void settings() {
	set_clause_weighting();
//	set_FRW();
}

void local_search(int max_flips) {
	int i,flipvar;
			
	for (step = 0; step<max_flips; step++) { 
		if(unlikely(unsat_stack_fill_pointer==0)) return;
		flipvar = pick_var();
		vSelect[flipvar]++;
		var_reward[flipvar] += fast_log2(step+1-time_stamp[flipvar]);
		time_stamp[flipvar] = step;		
		flip(flipvar);

	}
}


int main(int argc, char* argv[]) {

	int     seed,i;
	int		satisfy_flag=0;
	struct 	tms start, stop;
	cout<<"c This is CCAnr [Version: 2013.4.18] [Author: Shaowei Cai]."<<endl;

	times(&start);

	if (build_instance(argv[1])==0) {
		cout<<"Invalid filename: "<< argv[1]<<endl;
		return -1;
	}
	set_FRW();
	initLookUpTable(); //Initialize the look up table
	double total_p;
	sscanf(argv[2],"%d",&seed);
	p_first=atof(argv[3]);
	Beta_first=atoi(argv[4]);
	total_p=atof(argv[5]);
	p_second=total_p-p_first;
	Beta_second=Beta_first/2;
	srand(seed);
	fast_srand(seed);

	if(unitclause_queue_end_pointer>0) {
		preprocess();
	}

	build_neighbor_relation();

	cout<<"c Instance: Number of variables = "<<num_vars<<endl;
	cout<<"c Instance: Number of clauses = "<<num_clauses<<endl;
	cout<<"c Instance: Ratio = "<<ratio<<endl;
	cout<<"c Instance: Formula length = "<<formula_len<<endl;
	cout<<"c Instance: Avg (Min,Max) clause length = "<<avg_clause_len<<" ("<<min_clause_len<<","<<max_clause_len<<")"<<endl;
	cout<<"c Algorithmic: Random seed = "<<seed<<endl;
	cout<<"c Algorithmic: Beta_first = "<<Beta_first<<endl;
	cout<<"c Algorithmic: Beta_second = "<<Beta_second<<endl;
	cout<<"c Algorithmic: p_first = "<<p_first<<endl;
	cout<<"c Algorithmic: p_second = "<<p_second<<endl;
	for (tries = 0; tries <=max_tries; tries++) { 

		settings();

		init();
		local_search(max_flips);
		if (unsat_stack_fill_pointer==0) {
			if(verify_sol()==1) {
				satisfy_flag = 1;
				break;
			} else cout<<"c Sorry, something is wrong."<<endl; 
		}
	}

	times(&stop);
	double comp_time = double(stop.tms_utime - start.tms_utime +stop.tms_stime - start.tms_stime) / sysconf(_SC_CLK_TCK);

	if(satisfy_flag==1) {
	cout<<"s SATISFIABLE"<<endl;

	} else  cout<<"s UNKNOWN"<<endl;

	cout<<"c solveSteps = "<<i<<" tries + "<<step<<" steps (each try has " << max_flips << " steps)."<<endl;
	cout<<"c solveTime = "<<comp_time<<endl;



	free_memory();

	return 0;
}
