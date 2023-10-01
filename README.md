# algorithm_optimization_problem

# Problem statement
In our problem (VCCP), we have to minimize the cost of opening plants in some of potential sites and assigning suppliers to them with constraints given.

# Mathematical Formulation
Let S be a set of suppliers with positive supply volume SVi and P = {1,...,n} denote a set of 
potential sites for plants (facilities) with an associated minimum capacity requirement CRj and a 
fixed opening cost FPj and we have to open exactly p Plants. Cij denotes the cost of assigning 
supplier i ∈ S to the plant located at site j ∈ P. We define a binary variable Yj, which takes 1 if a 
plant is located at site j ∈ P and 0 otherwise. Binary variable Xij equals 1 if supplier i ∈ S is 
assigned to plant located at site j ∈ P and 0 otherwise.

![Screenshot from 2023-09-29 12-52-41](https://github.com/amitmakkad/algorithm_optimization_problem/assets/79632719/bdecc128-0672-496a-9d1e-8a8ce91f0993)

The objective function (1) is to minimize the total cost of assigning suppliers to plants and establishing such plants. 

The assignment constraint (2) guarantee that each supplier is assigned to at most one plant. 

The constraint (3) satisfies minimum supply requirement. 

The constraint (4) ensures that p plants are exactly located. 

# Solution
As the above problem is NP Hard, Heuristics are only practical approach to solve this problem. [This](https://www.sciencedirect.com/science/article/pii/S0360835210002123) paper proposes a Lagrangian relaxation approach with two phases of dual optimization, the subgradient deflection method in the first phase and the standard subgradient method in the second phase, by relaxing the assignment constraints. The resulting Lagrangian relaxed
problem can be decomposed into a set of independent knapsack problems, which can be solved to optimality efficiently. At each Lagrangian iteration, a feasible solution is constructed from the Lagrangian relaxed problem by applying a greedy algorithm. Finally, the best feasible solution found so far is
improved by a simple tabu search algorithm.

# Implementation 
langrange.cpp contains implemntation of above code in C++. It can be run in latest version C++ compiler using the below command.
```
g++ langrange.cpp -fpermissive -Wwrite-strings -o lng -lm -Wregister && ./lng
```
The above takes 1.0 seconds for r=0.4 with n=200, p=100, |S|=4000 in intel i5 processor.

# Contributer
Amit Kumar Makkad

This project is part of course CS357 Optimization Algorithms and Techniques Lab IIT Indore under guidance of prof Kapil Ahuja.
