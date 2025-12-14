# Assignment 9 - Hybrid Evolutionary Algorithm (HEA)

### Prepared by
- Marianna Myszkowska 156041
- Jakub Liszyński 156060

---

## 1. Problem Description

The problem consists of selecting exactly 50% of the available nodes from a given dataset (with coordinates and costs) and finding a Hamiltonian cycle through them. The objective is to minimize the sum of the total path length (Euclidean distance) and the costs of the selected nodes.

---

## 2. Methods

We implemented a **Hybrid Evolutionary Algorithm (HEA)**, also known as a Memetic Algorithm, which combines evolutionary population management with local search improvement.

### Algorithm Structure

* **Population**: A small "elite" population of **20 solutions**.
* **Type**: Steady-State Evolutionary Algorithm. In each iteration, two parents are selected to generate one child. If the child is better than the worst individual in the population and is unique, it replaces the worst individual.
* **Diversity Maintenance**: To prevent premature convergence, we enforce a strict **uniqueness constraint**. A new child is only added to the population if its objective value does not match any existing individual in the population.
* **Parent Selection**: Parents are selected with uniform probability from the current population.

### Recombination Operators

We implemented and compared two distinct recombination (crossover) operators:

#### Operator 1: Common Components (Structure Exploitation)
This operator is designed to exploit the "Global Convexity" of the problem (as identified in Assignment 8). It assumes that common features between high-quality parents are likely part of the global optimum.

**Logic:**
1.  Identify all **nodes** present in both parents.
2.  Identify all **edges** present in both parents.
3.  Construct sub-paths using these common edges.
4.  If the number of nodes is less than the target (50%), fill the remaining spots with random unselected nodes.
5.  Randomly connect the resulting sub-paths and random nodes to form a valid cycle.
6.  **Local Search**: Always applied to the offspring.

**Pseudocode:**
```cpp
Child = Empty
CommonNodes = Intersection(Parent1.Nodes, Parent2.Nodes)
CommonEdges = Intersection(Parent1.Edges, Parent2.Edges)

// Build fragments from common edges
Fragments = BuildSubpaths(CommonEdges)

// Fill remaining nodes randomly
While Child.Size < TargetSize:
    Add RandomNode NOT in Fragments

// Connect all fragments and loose nodes randomly
Child = ConnectRandomly(Fragments + RandomNodes)

ApplyLocalSearch(Child)
```

#### Operator 2: Filter & Repair (Heuristic Based)
This operator tries to preserve the structure of one parent while using the other parent as a "filter" to guide node selection, relying on the Regret Heuristic to fix the solution.

``` cpp
Child = Copy(Parent1)
// Filter step
For each node in Child:
    If node NOT in Parent2:
        Remove node from Child

// Repair step
While Child.Size < TargetSize:
    Insert node with MaxRegret(Child)

// Optional LS
If Variant == WithLS:
    ApplyLocalSearch(Child)
```

We tested three variants of the algorithm on instances TSPA and TSPB. Each experiment consisted of 20 runs.

    HEA_Op1 (Common): Operator 1 + Local Search.

    HEA_Op2 (Filter+LS): Operator 2 + Local Search.

    HEA_Op2_NoLS (Filter): Operator 2 + No Local Search (Repaired only).

| Instance | Method | Min | Average | Max |
| --- | --- | --- | --- | --- |
|TSPA |	HEA_Op1 (Common)	|69,066 |	69,150.2 |	69,331|
|TSPA	|HEA_Op2 (Filter+LS)|	70,346	|70,952.6 |	71,438|
|TSPA	|HEA_Op2_NoLS (Filter)|	71,047|	72,135.9|	72,610|
|TSPB	|HEA_Op1 (Common)|	43,404|	43,488.0	|43,579|
|TSPB	|HEA_Op2 (Filter+LS)	|44,695|	45,548.2|	46,099|
|TSPB	|HEA_Op2_NoLS (Filter)|	45,072	|46,290.8	|47,032|

### Superiority of Operator 1 (Common Components)

The results clearly demonstrate that Operator 1 is significantly superior to Operator 2 on both instances.

On TSPA, Operator 1 achieved an average of 69,150, compared to ~70,950 for Operator 2.

On TSPB, Operator 1 achieved an average of 43,488, compared to ~45,500 for Operator 2.

Why did Operator 1 work best? The answer lies in the Global Convexity Analysis performed in the previous assignment. As shown in the Global Convexity charts (Figure 1), there is a strong negative correlation between similarity and cost. This "Big Valley" structure means that the global optimum shares many nodes and edges with other high-quality local optima.

Operator 1 explicitly exploits this by fixing the "common backbone" (intersection of edges/nodes) and only searching the space of non-common elements. It centers the search in the middle of the "Big Valley," effectively recombining the "good building blocks" from both parents.

Operator 2 is more destructive; by filtering nodes and re-repairing with a heuristic, it likely breaks the beneficial edge structures that Operator 1 successfully preserves.

Global Convexity Analysis for TSPA and TSPB showing strong correlation between common edges/nodes and solution quality.

Comparing HEA_Op2 (Filter+LS) and HEA_Op2_NoLS, we see that adding Local Search yields better results (an improvement of ~1000 units on average). However, even with Local Search, Operator 2 cannot match the structural efficiency of Operator 1.

### Conclusions

The Hybrid Evolutionary Algorithm with Operator 1 (Common Components) is the most effective method tested so far. Its success validates the "Big Valley" hypothesis for this problem: the best way to find the optimum is to combine the shared structural features of high-quality parents rather than relying on heuristic repair of partial solutions.