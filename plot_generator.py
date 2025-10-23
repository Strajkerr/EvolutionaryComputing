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
      56,175,88,44,2,87,1,97,26,121,53,180,63,182,189,19,152,92,78,25,86,116,96,41,191,110,68,33,9,62,132,14,138,165,40,174,91,179,52,106,49,128,111,94,79,151,59,115,163,134,67,93,117,170,183,89,114,23,76,0,46,60,109,72,24,126,112,43,77,131,149,70,135,167,55,187,81,38,12,176,35,11,190,10,177,30,160,181,146,195,159,64,73,7,164,58,71,27,90,31
    ]
    path_m5_tspa.append(path_m5_tspa[0])

    # TSPB M5 best route (append start at end to close cycle)
    path_m5_tspb = [
      72,100,40,107,17,133,115,147,192,6,169,70,84,145,132,126,139,49,39,0,35,111,68,14,58,87,21,171,25,158,31,164,190,80,73,112,121,38,1,156,173,61,79,77,97,146,165,103,119,160,157,5,142,78,46,196,198,117,116,118,74,43,167,34,83,62,124,159,106,128,183,199,95,86,176,180,113,26,137,75,114,88,22,47,148,23,28,149,140,170,138,92,102,32,122,191,71,67,98,90
    ]
    path_m5_tspb.append(path_m5_tspb[0])

    visualize_solution(nodesa, path_m5_tspa, costsa, title="M5 Greedy First-Improvement (2-node swap) TSPA", save_path="M5_tspa.png")
    visualize_solution(nodesb, path_m5_tspb, costsb, title="M5 Greedy First-Improvement (2-node swap) TSPB", save_path="M5_tspb.png")

    # --- Added: M6 best routes (from M6 runs) ---
    # TSPA M6 best route (append start at end to close cycle)
    path_m6_tspa = [
      123,24,149,131,65,116,105,47,77,166,11,35,84,126,29,156,112,4,184,28,43,42,5,96,197,59,72,109,51,151,45,162,161,133,79,182,136,180,173,194,135,70,6,154,158,53,100,26,86,75,150,101,1,2,87,125,74,152,97,121,99,189,19,124,12,130,94,63,122,80,176,66,141,118,115,46,198,139,191,110,142,41,193,159,22,146,103,195,181,192,160,34,48,104,177,190,10,30,54,147
    ]
    path_m6_tspa.append(path_m6_tspa[0])

    # TSPB M6 best route (append start at end to close cycle)
    path_m6_tspb = [
      66,94,154,47,148,60,23,20,59,28,101,149,4,199,140,53,152,170,34,55,18,62,124,143,159,119,106,128,110,64,176,180,129,153,81,14,41,37,111,68,144,160,12,0,35,109,29,39,49,11,168,195,126,43,139,182,138,33,56,104,157,171,8,21,87,82,58,50,77,97,146,187,186,163,89,165,127,137,114,103,26,113,88,194,166,86,185,52,172,57,179,22,99,130,95,181,83,174,183,9
    ]
    path_m6_tspb.append(path_m6_tspb[0])

    visualize_solution(nodesa, path_m6_tspa, costsa, title="M6 Greedy First-Improvement (greedy start) TSPA", save_path="M6_tspa.png")
    visualize_solution(nodesb, path_m6_tspb, costsb, title="M6 Greedy First-Improvement (greedy start) TSPB", save_path="M6_tspb.png")

    # --- Added: M7 best routes (from M7 runs) ---
    # TSPA M7 best route (append start at end to close cycle)
    path_m7_tspa = [
      139,191,41,193,159,142,68,93,140,67,108,134,22,181,192,34,48,104,10,190,11,29,135,154,158,53,136,133,151,162,161,24,149,65,105,77,166,184,42,5,96,115,60,118,109,51,137,80,79,122,63,99,100,86,1,152,74,2,129,57,55,52,91,169,196,145,78,25,16,88,171,113,50,13,38,98,81,90,7,165,8,185,106,32,14,155,49,102,62,9,37,111,12,148,186,23,89,183,153,46
    ]
    path_m7_tspa.append(path_m7_tspa[0])

    # TSPB M7 best route (append start at end to close cycle)
    path_m7_tspb = [
      61,79,82,56,33,49,39,29,0,109,35,111,41,50,77,97,81,14,119,153,165,127,89,103,26,176,114,76,48,172,185,86,106,62,83,18,55,170,174,183,140,9,66,94,154,47,148,60,23,20,59,4,152,155,167,189,69,145,168,126,65,134,139,138,104,171,177,5,173,19,112,121,116,98,120,67,191,147,115,10,133,44,17,107,63,96,122,135,38,24,156,42,30,198,117,193,164,136,80,175
    ]
    path_m7_tspb.append(path_m7_tspb[0])

    visualize_solution(nodesa, path_m7_tspa, costsa, title="M7 Greedy First-Improvement (2-opt, random start) TSPA", save_path="M7_tspa.png")
    visualize_solution(nodesb, path_m7_tspb, costsb, title="M7 Greedy First-Improvement (2-opt, random start) TSPB", save_path="M7_tspb.png")

    # --- Added: M8 best routes (from M8 runs) ---
    # TSPA M8 best route (append start at end to close cycle)
    path_m8_tspa = [
      126,112,4,84,35,11,166,28,184,190,10,177,104,147,54,30,48,34,160,192,181,195,103,146,22,159,193,41,142,110,191,139,198,46,115,118,109,141,66,176,80,122,63,79,133,173,180,136,182,94,130,12,124,19,189,99,121,100,26,97,152,74,125,87,2,1,101,150,75,86,53,158,154,6,70,135,194,161,162,45,151,51,72,59,197,96,5,42,43,77,47,105,116,65,131,149,24,123,29,156
    ]
    path_m8_tspa.append(path_m8_tspa[0])

    # TSPB M8 best route (append start at end to close cycle)
    path_m8_tspb = [
      111,68,144,160,39,12,0,35,109,29,49,11,168,195,126,43,139,182,138,33,56,104,157,171,8,21,87,82,58,77,97,146,187,153,186,129,163,26,103,89,165,127,137,114,113,180,176,88,194,166,64,86,110,128,181,95,130,185,99,22,179,52,172,57,66,94,154,47,148,60,23,20,59,28,101,149,4,199,9,140,183,174,53,152,170,34,55,18,83,62,124,106,143,159,119,81,14,50,41,37
    ]
    path_m8_tspb.append(path_m8_tspb[0])

    visualize_solution(nodesa, path_m8_tspa, costsa, title="M8 Greedy First-Improvement (2-opt, greedy start) TSPA", save_path="M8_tspa.png")
    visualize_solution(nodesb, path_m8_tspb, costsb, title="M8 Greedy First-Improvement (2-opt, greedy start) TSPB", save_path="M8_tspb.png")