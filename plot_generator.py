import csv
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.colors import LinearSegmentedColormap

def read_nodes_from_csv(filename):
    """
    Reads node data from a CSV file.
    Each row: x; y; cost
    Returns:
        nodes (list of (x, y)),
        costs (list of float)
    """
    nodes = []
    costs = []
    with open(filename, 'r', newline='') as csvfile:
        reader = csv.reader(csvfile, delimiter=';')
        for row in reader:
            if not row or len(row) < 3:
                continue
            x, y, cost = map(float, row[:3])
            nodes.append((x, y))
            costs.append(cost)
    return nodes, costs


def visualize_solution(nodes, path=None, costs=None,
                       title="Graph Visualization",
                       edges=None,
                       show=True,
                       save_path=None):
    """
    Visualize a 2D graph or TSP-like path.
    - nodes: list of (x, y)
    - path: optional ordered list of node indices
    - costs: list of node costs (optional, used for color)
    - edges: optional list of (i, j) pairs to draw connections
    """
    nodes = np.array(nodes)
    x, y = nodes[:, 0], nodes[:, 1]

    if costs is not None:
        costs = np.array(costs)
        normalized_costs = (costs - np.min(costs)) / (np.max(costs) - np.min(costs) + 1e-9)
    else:
        normalized_costs = np.zeros(len(nodes))

    plt.figure(figsize=(8, 6))

    if edges is None and path is not None:
        edges = [(path[i], path[i + 1]) for i in range(len(path) - 1)]
    elif edges is None:
        edges = []

    for (i, j) in edges:
        plt.plot([nodes[i, 0], nodes[j, 0]], [nodes[i, 1], nodes[j, 1]],
                 color='black', lw=1, alpha=0.8, zorder=4)


    pink_map = LinearSegmentedColormap.from_list("white_pink", ["#ffffff", "#ff69b4"])

    scatter = plt.scatter(
        x, y,
        c=normalized_costs,
        cmap=pink_map,
        s=80,
        edgecolor='black',
        alpha=0.95,
        zorder=3
    )

    plt.title(title)
    if costs is not None:
        plt.colorbar(scatter, label="Node Cost")

    plt.axis("equal")
    plt.xlabel("X coordinate")
    plt.ylabel("Y coordinate")
    plt.tight_layout()

    if save_path:
        plt.savefig(save_path, dpi=300)
        print(f"Saved figure {save_path}")
    if show:
        plt.show()
    plt.close()


if __name__ == "__main__":
    nodesa, costsa = read_nodes_from_csv(r".\TSPA.csv")
    nodesb, costsb = read_nodes_from_csv(r".\TSPB.csv")
    
    # --- Added: M5 best routes (from M5 runs) ---
    # TSPA M5 best route (append start at end to close cycle)
    path_m5_tspa = [
      46,118,51,89,183,143,0,117,93,140,108,18,159,193,41,34,160,54,177,4,112,84,146,22,195,181,43,116,65,59,94,129,92,179,145,78,31,56,25,82,57,55,52,178,106,185,165,39,27,90,81,2,75,101,86,100,26,97,1,152,167,49,102,62,9,148,63,79,80,176,137,23,186,15,144,14,138,40,196,113,175,171,16,44,120,53,70,135,154,180,133,151,162,123,149,184,42,5,115,139
    ]
    path_m5_tspa.append(path_m5_tspa[0])

    # TSPB M5 best route (append start at end to close cycle)
    path_m5_tspb = [
      180,176,106,124,62,183,140,149,28,20,148,47,94,66,179,166,194,163,153,77,8,121,131,107,40,63,102,135,122,90,51,147,6,188,169,132,15,145,13,126,195,168,109,35,0,29,139,11,144,111,103,26,114,137,127,89,165,187,146,97,141,91,61,36,177,5,78,175,45,80,190,193,117,31,54,25,104,86,185,130,95,18,55,34,170,152,184,155,3,70,161,134,118,74,182,138,33,160,143,113
    ]
    path_m5_tspb.append(path_m5_tspb[0])

    visualize_solution(nodesa, path_m5_tspa, costsa, title="M5 Greedy First-Improvement (2-node swap) TSPA", save_path="M5_tspa.png")
    visualize_solution(nodesb, path_m5_tspb, costsb, title="M5 Greedy First-Improvement (2-node swap) TSPB", save_path="M5_tspb.png")

    # --- Added: M6 best routes (from M6 runs) ---
    # TSPA M6 best route (append start at end to close cycle)
    path_m6_tspa = [
      0,46,65,47,72,162,149,131,166,28,184,35,123,127,29,156,112,4,84,77,43,96,41,193,181,192,160,48,104,177,190,10,54,30,34,103,146,195,159,22,18,108,140,68,139,115,5,42,105,116,59,118,51,176,151,133,79,80,122,63,94,26,100,121,53,180,135,70,154,158,86,97,152,1,101,75,2,120,44,25,16,171,175,78,145,185,40,165,106,178,14,49,62,148,137,23,89,183,143,117
    ]
    path_m6_tspa.append(path_m6_tspa[0])

    # TSPB M6 best route (append start at end to close cycle)
    path_m6_tspb = [
      147,192,150,6,188,65,169,132,161,70,3,15,145,13,126,195,168,49,33,56,144,160,29,0,109,35,34,18,62,124,106,86,176,113,153,81,77,141,91,36,61,21,87,82,111,8,104,138,182,11,139,43,134,85,74,118,98,51,120,67,71,191,90,122,131,121,116,112,19,151,24,1,197,135,63,38,27,16,42,156,198,117,193,31,54,164,73,173,136,190,80,46,162,175,78,142,45,5,177,25
    ]
    path_m6_tspb.append(path_m6_tspb[0])

    visualize_solution(nodesa, path_m6_tspa, costsa, title="M6 Greedy First-Improvement (greedy start) TSPA", save_path="M6_tspa.png")
    visualize_solution(nodesb, path_m6_tspb, costsb, title="M6 Greedy First-Improvement (greedy start) TSPB", save_path="M6_tspb.png")

    # --- Added: M7 best routes (from M7 runs) ---
    # TSPA M7 best route (append start at end to close cycle)
    path_m7_tspa = [
      62,144,14,49,3,178,106,185,40,119,165,90,81,196,31,56,113,175,171,16,78,145,179,52,55,57,92,129,25,44,120,2,152,1,75,86,101,97,26,100,53,158,180,154,135,70,127,123,35,84,112,4,190,10,177,54,184,160,34,181,146,22,41,193,18,108,140,68,46,0,117,143,183,89,186,23,137,176,51,118,59,115,139,42,43,116,65,131,149,162,151,133,79,80,122,63,94,124,148,9
    ]
    path_m7_tspa.append(path_m7_tspa[0])

    # TSPB M7 best route (append start at end to close cycle)
    path_m7_tspb = [
      113,26,103,114,137,127,165,89,163,153,77,141,91,79,61,36,177,5,78,175,142,45,162,80,190,136,73,164,54,31,193,117,198,1,135,63,40,107,122,131,121,51,90,191,147,188,169,132,70,3,15,145,13,195,168,139,11,138,33,160,104,21,82,8,111,144,29,0,109,35,143,159,106,124,62,18,55,34,152,183,140,4,149,28,20,60,148,47,94,66,179,22,99,95,185,86,166,194,176,180
    ]
    path_m7_tspb.append(path_m7_tspb[0])

    visualize_solution(nodesa, path_m7_tspa, costsa, title="M7 Greedy First-Improvement (2-opt, random start) TSPA", save_path="M7_tspa.png")
    visualize_solution(nodesb, path_m7_tspb, costsb, title="M7 Greedy First-Improvement (2-opt, random start) TSPB", save_path="M7_tspb.png")

    # --- Added: M8 best routes (from M8 runs) ---
    # TSPA M8 best route (append start at end to close cycle)
    path_m8_tspa = [
      198,115,96,5,42,160,48,54,30,177,10,190,4,112,84,35,184,28,43,105,116,65,47,131,149,24,123,162,151,72,59,118,51,176,80,79,133,194,127,70,135,154,158,53,180,63,94,124,152,1,97,26,100,86,101,75,2,120,44,16,78,145,40,185,55,52,106,178,49,14,144,62,9,148,137,23,89,183,143,170,0,117,93,140,36,108,69,18,22,146,34,181,195,159,193,41,139,110,68,46
    ]
    path_m8_tspa.append(path_m8_tspa[0])

    # TSPB M8 best route (append start at end to close cycle)
    path_m8_tspb = [
      141,61,36,175,5,177,21,87,82,8,104,182,138,33,49,11,139,168,195,169,188,70,3,145,29,109,35,0,160,144,56,111,41,81,119,159,143,106,124,62,18,55,34,170,152,53,140,4,149,101,28,59,20,23,60,154,94,66,47,148,9,199,183,174,83,181,95,130,99,22,179,57,172,52,185,86,110,128,64,166,194,88,176,180,113,114,137,127,165,89,103,26,163,129,186,153,187,146,97,77
    ]
    path_m8_tspb.append(path_m8_tspb[0])

    visualize_solution(nodesa, path_m8_tspa, costsa, title="M8 Greedy First-Improvement (2-opt, greedy start) TSPA", save_path="M8_tspa.png")
    visualize_solution(nodesb, path_m8_tspb, costsb, title="M8 Greedy First-Improvement (2-opt, greedy start) TSPB", save_path="M8_tspb.png")