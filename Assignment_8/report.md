# Assignment 7 - Large Neighborhood Search (LNS)

### Prepared by

- Marianna Myszkowska 156041
- Jakub Liszyński 156060

---

## Important Note

In Assignment 6, we corrected the ILS implementation to use simple perturbation (random 2-opt kicks) instead of destroy-repair. The destroy-repair mechanism is now properly used here in Assignment 7 for Large Neighborhood Search.

### Assignment 6 Results (Corrected Implementation)

After fixing the ILS implementation to use simple perturbation:

**MSLS Results:**
- TSPA: 71483.2 avg (70876 – 71878), Time: 273.77s
- TSPB: 45807.9 avg (45011 – 46646), Time: 287.41s

**ILS Results (with simple 2-opt perturbation):**
- TSPA: 73626.9 avg (70998 – 75350), Time: 1.63s, Avg LS runs: 8.45
- TSPB: 47171.2 avg (44392 – 50455), Time: 1.39s, Avg LS runs: 23.1

**Key Observation:** ILS with simple perturbation found best TSPB solution (44392) across all methods!

---

## Problem Description

We are given three columns of integers with a row for each node. The first two columns contain x and y coordinates of the node positions in a plane. The third column contains node costs. The goal is to select exactly 50% of the nodes (if the number of nodes is odd we round the number of nodes to be selected up) and form a Hamiltonian cycle (closed path) through this set of nodes such that the sum of the total length of the path plus the total cost of the selected nodes is minimized.

The distances between nodes are calculated as Euclidean distances rounded mathematically to integer values.

---

## Methods

### Large Neighborhood Search (LNS)

#### Destroy Operator

**Description**: Random destruction with 30% removal rate
- Randomly removes 30% of nodes from the current solution
- Selection is completely random (uniform distribution)
- Minimum 2 nodes are always removed
- Creates a partial solution requiring reconstruction

**Pseudocode**:
```
numToRemove = max(2, floor(solutionSize * 0.3))
destroyed = copy(currentSolution)

For i from 1 to numToRemove:
    randomIndex = uniform_random(0, destroyed.size() - 1)
    Remove node at randomIndex from destroyed
    
Return destroyed (partial solution)
```

#### Repair Operator

**Description**: Greedy best-position insertion
- Uses the best construction heuristic approach
- For each missing node, evaluates all insertion positions
- Selects the node and position that minimizes total cost
- Iteratively builds complete solution

**Pseudocode**:
```
repaired = copy(partialSolution)
inSolution = boolean array marking which nodes are present

While repaired.size() < targetSize:
    bestNode = NULL
    bestPosition = -1
    bestCost = INFINITY
    
    For each node NOT in solution:
        For each position (0 to repaired.size()):
            Insert node at position
            cost = evaluateSolution(repaired)
            
            If cost < bestCost:
                bestCost = cost
                bestNode = node
                bestPosition = position
            
            Remove node from position
    
    Permanently insert bestNode at bestPosition
    Mark bestNode as in solution
    
Return repaired (complete solution)
```

#### LNS Algorithm

**Two Versions Implemented:**

**1. LNS with Local Search**
```pseudocode
For run = 1 to 20:
    currentSolution = random_permutation()
    apply_local_search(currentSolution)  // Always apply to initial
    currentCost = evaluate(currentSolution)
    iterations = 0
    
    While time_elapsed < timeLimit:
        iterations++
        
        // Destroy phase
        destroyed = destroy(currentSolution, 0.3)
        
        // Repair phase
        repaired = repair(destroyed, targetSize)
        
        // Local Search phase
        apply_local_search(repaired)
        repairedCost = evaluate(repaired)
        
        // Acceptance
        If repairedCost < currentCost:
            currentSolution = repaired
            currentCost = repairedCost
    
    Report best solution found in this run
```

**2. LNS without Local Search**
```pseudocode

```


---

## Results

### LNS with Local Search

| Instance | Runs | Avg (Min – Max) | Execution Time | Avg Iterations |
|---|---:|---:|---:|---:|
| TSPA | 20 | **[RUN EXPERIMENT]** | **[TIME]** s | **[ITER]** |
| TSPB | 20 | **[RUN EXPERIMENT]** | **[TIME]** s | **[ITER]** |


### LNS without Local Search

| Instance | Runs | Avg (Min – Max) | Execution Time | Avg Iterations |
|---|---:|---:|---:|---:|
| TSPA | 20 | **[RUN EXPERIMENT]** | **[TIME]** s | **[ITER]** |
| TSPB | 20 | **[RUN EXPERIMENT]** | **[TIME]** s | **[ITER]** |


---

## Comparison with Previous Methods

| Method | TSPA Avg (Min – Max) | TSPB Avg (Min – Max) |
|---|---:|---:|
| **Best Construction (NN Insertion)** | 71071.2 (69941 – 73650) | 44649.9 (43163 – 51497) |
| **Best Local Search (List of Moves)** | 74444.5 (70453 - 79976) | 49121.1 (45898 - 52188) |
| **MSLS (A6 corrected)** | 71483.2 (70876 – 71878) | 45807.9 (45011 – 46646) |
| **ILS (A6 corrected)** | 73626.9 (70998 – 75350) | 47171.2 (44392 – 50455) |
| **LNS with LS (A7)** | **[RUN EXPERIMENT]** | **[RUN EXPERIMENT]** |
| **LNS without LS (A7)** | **[RUN EXPERIMENT]** | **[RUN EXPERIMENT]** |

### Best Solutions Comparison

| Method | TSPA Best | TSPB Best |
|---|---:|---:|
| **Construction** | 69941 | 43163 |
| **Local Search** | 70453 | 45898 |
| **MSLS** | 70876 | 45011 |
| **ILS (corrected)** | 70998 | **44392** ⭐ |
| **LNS with LS** | **[RUN]** | **[RUN]** |
| **LNS without LS** | **[RUN]** | **[RUN]** |


---


## Conclusions

## Global Convexity Analysis

### Methodology

We analyzed the global structure of the solution space by examining 1000 random local optima for each instance. For each local optimum, we calculated its similarity using two measures:

**Similarity Measures:**
1. **Node-based (Jaccard)**: Number of common selected nodes divided by total unique nodes
   - Formula: `|A ∩ B| / |A ∪ B|`
2. **Edge-based**: Number of common edges divided by total unique edges
   - Formula: `|E_A ∩ E_B| / |E_A ∪ E_B|`

**Reference Solutions:**
1. **Average similarity**: Mean similarity to all other 999 local optima
2. **Best of 1000**: Similarity to the best solution found among the 1000 local optima (excluding itself)
3. **Reference solution**: Similarity to a high-quality solution generated by the best construction method

### Results

#### Correlation Coefficients

| Instance | Analysis Type | Node-based | Edge-based |
|---|---|---:|---:|
| **TSPA** | Avg to Others | [FILL] | [FILL] |
| **TSPA** | To Best of 1000 | [FILL] | [FILL] |
| **TSPA** | To Reference | [FILL] | [FILL] |
| **TSPB** | Avg to Others | [FILL] | [FILL] |
| **TSPB** | To Best of 1000 | [FILL] | [FILL] |
| **TSPB** | To Reference | [FILL] | [FILL] |

**Interpretation of Correlation:**
- **Negative correlation** (< 0): Better solutions (lower cost) have higher similarity → global convexity
- **Positive correlation** (> 0): Better solutions have lower similarity → multimodal landscape
- **Near zero** (≈ 0): No clear relationship → random structure

#### Charts

![Global Convexity Analysis](global_convexity_all_charts.png)

### Observations

[After running experiments, fill in:]

1. **Node-based vs Edge-based Similarity**:
   - Which measure shows stronger correlation?
   - Are the patterns consistent across both measures?

2. **Instance Comparison (TSPA vs TSPB)**:
   - Which instance shows stronger global convexity?
   - Are the fitness landscapes structurally different?

3. **Reference Solution Analysis**:
   - How do solutions cluster around high-quality solutions?
   - Is there a "funnel" structure toward optimal solutions?

4. **Implications for Search Algorithms**:
   - Strong negative correlation suggests gradient-based methods (like local search) will be effective
   - Weak or positive correlation suggests need for diversification strategies

### Conclusions

[Fill after analyzing results]

The correlation analysis reveals [positive/negative/weak] relationship between solution quality and similarity, suggesting the search space is [convex/multimodal/random]. This explains why [method X] performed [well/poorly] in previous experiments.

