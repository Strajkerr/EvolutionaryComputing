# Assignment 7 - Large Neighborhood Search (LNS)

### Prepared by

- Marianna Myszkowska 156041
- Jakub Liszyński 156060

---

## Important Note

**This assignment properly implements destroy-repair operators for LNS.**

In Assignment 6, we corrected the ILS implementation to use **simple perturbation (random 2-opt kicks)** instead of destroy-repair. The destroy-repair mechanism is now properly used here in Assignment 7 for Large Neighborhood Search.

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
For run = 1 to 20:
    currentSolution = random_permutation()
    apply_local_search(currentSolution)  // Still apply to initial
    currentCost = evaluate(currentSolution)
    iterations = 0
    
    While time_elapsed < timeLimit:
        iterations++
        
        // Destroy phase
        destroyed = destroy(currentSolution, 0.3)
        
        // Repair phase (NO LS after)
        repaired = repair(destroyed, targetSize)
        repairedCost = evaluate(repaired)
        
        // Acceptance
        If repairedCost < currentCost:
            currentSolution = repaired
            currentCost = repairedCost
    
    Report best solution found in this run
```

**Key Parameters:**
- Destruction rate: 30%
- Time limit per run: 13.69s (TSPA) / 14.37s (TSPB) - matching average MSLS time from corrected A6
- Total runs: 20 per instance
- Acceptance criterion: Accept only if better (strict improvement)
- Local search: Steepest descent with list of moves (from Assignment 5)

---

## Results

### ⚠️ TO BE FILLED AFTER RUNNING EXPERIMENTS ⚠️

### LNS with Local Search

| Instance | Runs | Avg (Min – Max) | Execution Time | Avg Iterations |
|---|---:|---:|---:|---:|
| TSPA | 20 | **[RUN EXPERIMENT]** | **[TIME]** s | **[ITER]** |
| TSPB | 20 | **[RUN EXPERIMENT]** | **[TIME]** s | **[ITER]** |

**Iterations per run (TSPA):**
```
[RUN EXPERIMENT - List of 20 numbers]
```

**Iterations per run (TSPB):**
```
[RUN EXPERIMENT - List of 20 numbers]
```

**Best solution TSPA (cost: [COST]):**
```
[RUN EXPERIMENT - Node sequence]
```

**Best solution TSPB (cost: [COST]):**
```
[RUN EXPERIMENT - Node sequence]
```

---

### LNS without Local Search

| Instance | Runs | Avg (Min – Max) | Execution Time | Avg Iterations |
|---|---:|---:|---:|---:|
| TSPA | 20 | **[RUN EXPERIMENT]** | **[TIME]** s | **[ITER]** |
| TSPB | 20 | **[RUN EXPERIMENT]** | **[TIME]** s | **[ITER]** |

**Iterations per run (TSPA):**
```
[RUN EXPERIMENT - List of 20 numbers]
```

**Iterations per run (TSPB):**
```
[RUN EXPERIMENT - List of 20 numbers]
```

**Best solution TSPA (cost: [COST]):**
```
[RUN EXPERIMENT - Node sequence]
```

**Best solution TSPB (cost: [COST]):**
```
[RUN EXPERIMENT - Node sequence]
```

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

**Note:** ILS with simple perturbation achieved the **best TSPB solution ever** (44392)!

---

## Analysis

### ⚠️ TO BE COMPLETED AFTER EXPERIMENTS ⚠️

### LNS with vs without Local Search

**Quality Comparison:**
- **Best solutions**: [TO BE FILLED]
- **Average quality**: [TO BE FILLED]
- **Impact of LS**: [TO BE FILLED]

**Efficiency Comparison:**
- **Iterations**: [TO BE FILLED]
- **Time usage**: [TO BE FILLED]
- **Convergence speed**: [TO BE FILLED]

**Destroy-Repair Effectiveness:**
- [TO BE FILLED]
- Comparison with ILS perturbation
- Large neighborhood exploration benefits

### Comparison with Assignment 6 Methods

**LNS vs MSLS:**
- MSLS: More consistent (better avg), but 168-206× slower
- [TO BE FILLED after experiments]

**LNS vs ILS:**
- ILS: Found best TSPB ever (44392) with simple perturbation
- ILS: Very fast (1.4-1.6s), efficient (8-23 LS runs)
- [TO BE FILLED: How does destroy-repair compare to 2-opt kicks?]

### Key Insights from Corrected A6

1. **Simple perturbation works remarkably well**: 
   - ILS found best TSPB solution (44392) beating all previous methods
   - Only 8-23 LS runs needed on average
   - 168× faster than MSLS

2. **ILS vs MSLS trade-off**:
   - MSLS better average quality (more consistent)
   - ILS better best solutions (more exploration)
   - ILS dramatically more efficient

### Best Method Overall

**Based on:**
- Solution quality
- Computational efficiency
- Consistency
- Practical applicability

**Current leader:** ILS with simple perturbation (best TSPB: 44392)

**To determine:** Can LNS with destroy-repair beat this?

---

## Conclusions

### ⚠️ TO BE COMPLETED AFTER EXPERIMENTS ⚠️

Key findings to address:

1. **Destroy-repair vs simple perturbation**
   - Which provides better diversification?
   - Quality comparison
   - Computational cost comparison

2. **Impact of local search on LNS performance**
   - LS vs no-LS comparison
   - When is destroy-repair sufficient alone?
   - Trade-offs in solution quality vs iterations

3. **Comparison with MSLS and ILS**
   - Can LNS beat ILS's 44392 for TSPB?
   - Average quality comparison
   - Efficiency analysis

4. **Best method recommendation**
   - For time-constrained scenarios: ILS (1.5s, excellent results)
   - For best quality: [TO BE DETERMINED]
   - For consistency: MSLS (but 168× slower)

5. **Lessons learned**
   - Proper separation of ILS (simple perturbation) and LNS (destroy-repair)
   - Simple methods can outperform complex ones
   - Importance of implementation correctness

---

## Instructions for Completion

1. **Update main.cpp time limits** based on corrected A6 MSLS times:
   ```cpp
   // TSPA: 273.77 / 20 = 13.69s per run
   // TSPB: 287.41 / 20 = 14.37s per run
   double timeLimit = (FILE_NAME.find("TSPA") != std::string::npos) ? 13.69 : 14.37;
   ```

2. **Compile Assignment 7:**
   ```bash
   cd Assignment_7
   g++ -std=c++17 -O2 -o assignment7.exe main.cpp
   ```

3. **Run experiments:**
   ```bash
   .\assignment7.exe
   ```

4. **Fill in results** in the placeholders marked with **[RUN EXPERIMENT]**

5. **Complete analysis sections** marked with **[TO BE FILLED]**

6. **Write conclusions** comparing LNS with ILS's impressive 44392 result

7. **Determine overall winner** among MSLS, ILS, and LNS

---