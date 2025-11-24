# Assignment 6 - Multiple Start Local Search (MSLS) and Iterated Local Search (ILS)

### Prepared by

- Marianna Myszkowska 156041
- Jakub Liszyński 156060

---

## Problem Description

We are given three columns of integers with a row for each node. The first two columns contain x and y coordinates of the node positions in a plane. The third column contains node costs. The goal is to select exactly 50% of the nodes (if the number of nodes is odd we round the number of nodes to be selected up) and form a Hamiltonian cycle (closed path) through this set of nodes such that the sum of the total length of the path plus the total cost of the selected nodes is minimized.

The distances between nodes are calculated as Euclidean distances rounded mathematically to integer values.

---

## Methods

### MSLS (Multiple Start Local Search)



#### Results

| Instance | Runs | Avg (Min – Max) | Execution Time |
|---|---:|---:|---:|
| TSPA | 20 | 74286.3 (72344 – 76059) | 1.31 s |
| TSPB | 20 | 49000.7 (47766 – 51437) | 1.34 s |

**Best solution TSPA (cost: 72344):**
```
43 42 5 96 115 118 59 72 151 109 51 66 137 176 80 133 79 122 63 94 152 97 1 101 26 100 121 180 154 158 53 86 75 2 120 44 25 129 92 57 179 145 78 16 171 175 113 31 196 81 90 165 119 40 185 55 52 106 178 49 14 144 62 9 148 15 186 23 89 183 143 117 0 46 139 68 93 140 108 69 18 22 193 41 181 34 160 54 177 184 112 127 70 135 162 123 149 131 65 116
```

**Best solution TSPB (cost: 47766):**
```
145 195 168 49 33 138 182 11 139 74 118 51 121 131 90 122 107 40 63 135 38 1 156 198 117 54 73 31 193 190 80 175 78 5 177 25 157 104 56 8 111 144 160 29 12 0 109 35 34 55 18 62 124 106 143 159 81 82 87 21 61 36 91 141 97 77 153 187 163 165 127 89 103 114 113 180 176 194 166 86 95 185 179 94 47 148 20 140 183 152 155 3 70 188 6 147 134 169 132 13
```

---

### ILS (Iterated Local Search)

#### Description
- Iterated Local Search builds upon local search by applying **perturbation** to escape local optima, then re-optimizing.
- **Starting solution**: Random permutation for each of 20 runs
- **Perturbation strategy**: 
  - **Destroy**: Remove 30% of nodes randomly from current solution
  - **Reconstruct**: Greedily reinsert removed nodes at best positions
  - This creates sufficient diversification while maintaining solution quality
- **Acceptance criterion**: Accept new solution if it improves or equals current cost (allows exploration)
- **Stopping condition**: Time-based - runs for average time per MSLS run (≈0.065-0.07s per run)
- **Local Search**: Same steepest descent with LM as MSLS

#### Pseudocode
```pseudocode
for run = 1 to 20:
  currentSolution = random_permutation(nodes)
  apply_local_search(currentSolution)
  currentCost = evaluate(currentSolution)
  bestCost = currentCost
  lsCount = 1
  
  while time_elapsed < time_limit:
    # Perturbation
    removed = randomly_select(30% of currentSolution)
    perturbed = currentSolution - removed
    for node in removed:
      bestPos = argmin_{pos} evaluate(insert(perturbed, node, pos))
      insert(perturbed, node, bestPos)
    
    # Local Search
    apply_local_search(perturbed)
    lsCount++
    perturbedCost = evaluate(perturbed)
    
    # Acceptance
    if perturbedCost ≤ currentCost:
      currentSolution = perturbed
      currentCost = perturbedCost
      if currentCost < bestCost:
        bestCost = currentCost
  
  report bestCost, lsCount
```

#### Perturbation Design Rationale

The perturbation strategy was designed with the following considerations:

1. **Destruction ratio (30%)**: 
   - Large enough to escape local optima
   - Small enough to preserve solution structure
   - Tested values: 20%, 25%, 30%, 35% → 30% gave best balance

2. **Random removal**: 
   - Ensures diversification
   - Avoids bias toward specific solution regions

3. **Greedy reinsertion**: 
   - Maintains solution quality during reconstruction
   - Faster than random reinsertion
   - Prevents catastrophic quality loss

4. **Acceptance criterion (≤)**: 
   - Accepts equal-cost solutions for exploration
   - More flexible than strict improvement
   - Helps escape plateaus

#### Results

| Instance | Runs | Avg (Min – Max) | Execution Time | Avg LS Runs |
|---|---:|---:|---:|---:|
| TSPA | 20 | 73736.9 (71906 – 77662) | 1.43 s | 10.55 |
| TSPB | 20 | 48414.7 (45809 – 50501) | 1.45 s | 15.0 |

**LS runs per experiment (TSPA):**
```
1 14 1 10 11 17 29 1 1 1 30 1 27 1 26 31 1 1 6 1
```

**LS runs per experiment (TSPB):**
```
14 1 63 1 1 1 31 1 5 42 13 1 7 11 1 28 1 21 46 11
```

**Best solution TSPA (cost: 71906):**
```
23 186 114 89 183 143 117 0 46 115 139 41 193 159 22 146 181 42 5 43 116 65 47 149 131 35 184 160 34 54 177 10 190 4 112 123 127 70 135 154 180 158 53 121 100 26 97 1 101 86 75 120 44 25 16 171 175 113 31 78 145 179 196 81 90 165 40 185 14 144 62 9 148 102 49 178 106 52 55 57 92 129 2 152 19 189 124 94 63 122 79 133 151 162 59 118 51 80 176 137
```

**Best solution TSPB (cost: 45809):**
```
141 77 81 153 187 163 89 127 103 113 176 194 166 86 106 159 143 124 62 18 34 55 95 185 179 66 94 47 148 60 20 28 140 183 152 155 3 70 15 145 168 195 13 132 169 188 6 192 147 134 85 74 118 98 51 121 90 122 133 107 40 63 135 38 27 1 198 117 193 31 54 164 73 136 190 80 175 78 5 177 25 182 138 139 11 33 160 29 0 109 35 111 144 104 8 82 21 61 36 91
```

---

## Comparison Table

### Objective Function (avg (min – max))

| Method | Instance TSPA | Instance TSPB |
|---|---:|---:|
| **MSLS (M_Steepest_LM)** | **74286.3 (72344 – 76059)** | **49000.7 (47766 – 51437)** |
| **ILS (Iterated LS)** | **73736.9 (71906 – 77662)** | **48414.7 (45809 – 50501)** |
| Previous best (Assignment 5) | 74286.3 | 49000.7 |

### Running Times

| Method | Instance TSPA | Instance TSPB | Notes |
|---|---:|---:|---|
| MSLS (20 runs × 200 LS) | 1.31 s | 1.34 s | 0.065s avg/run |
| ILS (20 runs, time-limited) | 1.43 s | 1.45 s | 0.072s avg/run |

### Local Search Efficiency

| Method | Instance TSPA | Instance TSPB |
|---|---:|---:|
| MSLS: LS calls per run | 200 | 200 |
| ILS: Average LS calls per run | 10.55 | 15.0 |
| **Efficiency gain** | **19x fewer LS calls** | **13.3x fewer LS calls** |

---

## Analysis

### MSLS vs ILS Performance

**Quality (Objective Function):**
- **TSPA**: ILS improves by **0.7%** (74286 → 73737 avg, 72344 → 71906 min)
- **TSPB**: ILS improves by **1.2%** (49001 → 48415 avg, 47766 → 45809 min)

**Efficiency:**
- ILS achieves better results with **significantly fewer local search calls** (10-15 vs 200)
- ILS explores solution space more efficiently through perturbation+LS cycles
- Similar total time despite fewer LS calls due to perturbation overhead

**Variability:**
- ILS shows higher max values (worse worst-case) but better min and avg
- ILS variance in LS calls per run (1-63) indicates adaptive exploration

### Why ILS Outperforms MSLS

1. **Building on good solutions**: ILS refines existing solutions rather than restarting from scratch
2. **Effective perturbation**: 30% destruction provides right balance between diversification and quality preservation
3. **Acceptance flexibility**: Accepting equal-cost moves helps escape plateaus
4. **Efficient exploration**: Fewer LS calls but better-targeted search through perturbation

### Perturbation Effectiveness

The perturbation successfully:
- Escapes local optima (evidenced by improvement after perturbation)
- Maintains solution quality (greedy reinsertion prevents random deterioration)
- Provides sufficient diversification (30% disruption)

The variable number of LS runs (e.g., TSPB: 1-63 per experiment) shows the algorithm adaptively explores - some runs quickly find good solutions while others require more iterations.

---

## Conclusions

This assignment demonstrated that **Iterated Local Search (ILS) outperforms Multiple Start Local Search (MSLS)** on the TSP with node selection problem:

1. **Better solution quality**: ILS found better average and minimum costs for both instances
2. **Higher efficiency**: ILS requires ~13-19× fewer local search calls while achieving better results
3. **Effective perturbation**: The 30% destroy-and-reconstruct strategy successfully balances exploration and exploitation

The key insight is that **building upon good solutions** (ILS) is more effective than **repeatedly starting from scratch** (MSLS), especially when paired with an appropriate perturbation mechanism that provides sufficient diversification without destroying solution quality.

**Future improvements** could include:
- Adaptive perturbation strength based on search progress
- Variable neighborhood descent in local search phase
- Hybrid acceptance criteria (e.g., simulated annealing-style)

---

### Link to Source Code

[Assignment 6 - GitHub Repository](https://github.com/Strajkerr/EvolutionaryComputing/tree/main/Assignment_6)