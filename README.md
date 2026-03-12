Usage
The solver accepts several command-line parameters that control the behavior of the search process.
./BandSCCAnr instance.cnf seed p1 β1 d
Parameters
Parameter	Description
instance.cnf	Input SAT instance in DIMACS CNF format
seed	Random seed used to initialize the stochastic local search process
p1	Probability parameter p1​ controlling the probability-based variable selection
β1	Threshold parameter β1​ used in the clause selection mechanism
d	Exploration parameter ddd used in the UCB-based selection strategy
Parameter Relations
Some parameters are derived internally by the solver:
p2=total_p-p1;
p2=0.72;​p2=0.0.26; total_p=0.98
β=β1/2 = β2=1000
These relations follow the parameter settings described in the paper.
Default Parameter Setting (Used in the Paper)
The experiments in the paper use the following parameter configuration:
seed = 0
p1 = 0.72
β1 = 1000
d = 0.98
Example
To run the solver with the parameter settings used in the paper:
./BandSCCAnr instance.cnf 0 0.72 1000 0.98
This command runs the solver on instance.cnf using the same parameter configuration reported in the experimental section of the paper.
