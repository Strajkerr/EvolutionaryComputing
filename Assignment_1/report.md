# Assignment 1 - Greedy heuristic

### Problem descirption
We are given three columns of integers with a row for each node. The first two columns contain x
and y coordinates of the node positions in a plane. The third column contains node costs. The goal is
to select exactly 50% of the nodes (if the number of nodes is odd we round the number of nodes to
be selected up) and form a Hamiltonian cycle (closed path) through this set of nodes such that the
sum of the total length of the path plus the total cost of the selected nodes is minimized.
The distances between nodes are calculated as Euclidean distances rounded mathematically to
integer values. The distance matrix should be calculated just after reading an instance and then only
the distance matrix (no nodes coordinates) should be accessed by optimization methods to allow
instances defined only by distance matrices.

## Methods

### Random solution

#### Pseudocode
``` cpp
    
```

#### Results

| Best score | Worst score | Average score |
| --- | --- | --- |
| 231391 | 292542 | 263102 |

Best found solution:

```
86 6 59 8 34 156 74 91 182 103 127 163 153 168 148 44 164 185 18 194 70 97 176 116 120 54 10 146 111 72 154 31 132 50 184 100 95 93 7 82 1 90 183 191 19 126 63 197 166 198 94 79 181 22 23 53 139 11 137 138 57 5 15 162 41 161 151 117 51 46 48 28 55 109 133 21 65 42 29 47 145 125 196 130 121 40 12 0 69 78 108 123 81 101 129 62 66 140 99 113
```

### Nearest neighbour considering adding the node onlyt at the end of the current path

#### Pseudocode
``` cpp
    
```

#### Results

| Best score | Worst score | Average score |
| --- | --- | --- |
| 81598 | 88112 | 83234.5 |

Best found solution:

```
154 180 53 63 176 80 151 59 65 116 42 193 41 139 115 46 0 183 143 117 93 18 22 34 160 184 123 135 70 127 162 133 79 94 97 101 1 152 120 78 145 185 40 165 90 81 113 175 171 16 31 44 92 57 106 49 144 62 14 178 52 55 129 2 75 86 26 100 121 148 137 23 186 89 114 15 9 102 138 21 164 7 95 39 27 196 179 25 82 167 124 51 118 43 149 131 112 4 177 54
```

### Link to the source code (Github repository - directory Assignment 1)

https://github.com/Strajkerr/EvolutionaryComputing/tree/main/Assignment_1