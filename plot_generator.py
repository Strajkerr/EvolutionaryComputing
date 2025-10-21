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
      171,88,16,75,180,173,72,59,197,115,46,0,153,143,170,117,93,140,36,68,110,193,41,147,54,30,34,103,146,22,195,181,192,160,48,43,105,118,51,176,80,97,78,157,98,81,187,91,179,92,57,33,37,111,152,74,125,167,128,3,32,138,14,155,144,64,15,124,189,19,172,95,71,58,164,7,21,94,63,79,151,149,131,35,84,4,112,24,66,186,114,83,89,183,73,132,61,148,99,121,182,136,65,116,96,5,42,28,166,11,126,156,29,127,194,161,162,45,133,122,130,12,137,23,76,141,109,60,198,139,191,142,199,20,134,18,69,108,67,163,159,104,177,10,190,184,77,47,123,70,135,6,154,158,53,86,100,26,1,101,150,44,25,13,31,38,168,145,169,174,90,107,27,39,165,8,119,40,185,52,55,129,82,120,2,87,9,62,102,49,178
    ]
    path_m5_tspa.append(path_m5_tspa[0])

    # TSPB M5 best route (append start at end to close cycle)
    path_m5_tspb = [
      56,62,18,83,128,64,166,194,88,176,129,153,97,36,175,46,162,105,136,123,12,55,47,154,94,66,57,172,52,119,81,77,58,21,87,82,170,152,183,22,179,48,114,103,163,186,14,111,68,144,160,33,49,39,109,34,174,140,4,149,101,28,59,20,23,60,148,130,95,181,110,86,185,99,9,199,53,184,69,143,159,106,124,76,93,75,137,127,26,113,180,89,165,187,146,141,80,190,108,196,42,156,151,19,112,121,116,125,90,191,178,115,71,104,8,61,7,45,164,31,193,117,30,198,131,40,107,17,72,100,63,96,102,92,38,27,16,24,54,73,173,25,157,138,182,74,118,158,1,197,135,32,122,44,133,10,147,192,150,6,188,65,161,70,15,145,43,134,85,67,120,51,98,2,139,11,189,167,155,84,3,13,132,169,126,195,168,29,0,35,37,41,50,91,79,78,142,5,177,171
    ]
    path_m5_tspb.append(path_m5_tspb[0])

    visualize_solution(nodesa, path_m5_tspa, costsa, title="M5 Greedy First-Improvement (2-node swap) TSPA", save_path="M5_tspa.png")
    visualize_solution(nodesb, path_m5_tspb, costsb, title="M5 Greedy First-Improvement (2-node swap) TSPB", save_path="M5_tspb.png")

    # --- Added: M6 best routes (from M6 runs) ---
    # TSPA M6 best route (append start at end to close cycle)
    path_m6_tspa = [
      163,67,108,69,199,18,134,20,22,146,103,195,159,193,41,142,191,139,110,68,46,198,115,60,118,109,51,72,59,197,96,5,42,192,181,34,160,48,30,54,147,104,177,10,190,4,112,156,29,126,84,35,11,184,28,166,43,77,105,116,65,47,131,149,24,123,127,161,162,45,151,133,194,135,70,6,173,180,154,158,53,136,182,121,99,100,26,86,150,75,44,25,16,88,171,175,113,50,56,13,31,85,188,157,38,168,78,145,91,179,92,57,55,185,119,40,169,196,17,98,81,187,174,90,107,27,71,58,164,7,21,95,39,165,8,106,178,3,32,138,14,155,144,132,73,61,62,9,15,33,37,148,111,167,128,102,49,52,172,129,82,120,2,87,1,101,97,125,74,152,19,189,124,12,130,94,63,122,79,80,176,66,141,137,23,186,114,64,83,89,183,76,153,0,170,143,117,93,140,36
    ]
    path_m6_tspa.append(path_m6_tspa[0])

    # TSPB M6 best route (append start at end to close cycle)
    path_m6_tspb = [
      63,100,40,107,17,72,44,133,10,115,178,122,135,32,96,102,92,38,27,16,197,1,24,156,42,108,196,30,198,117,151,54,31,193,164,73,173,136,105,190,80,46,162,175,78,142,45,5,123,177,7,36,61,79,91,141,97,77,58,50,41,14,81,153,129,186,146,187,165,163,89,127,137,75,93,48,76,114,103,26,113,180,176,88,194,166,64,86,110,181,95,130,99,22,185,179,52,172,57,66,94,154,47,148,60,23,20,59,28,101,149,4,199,9,140,183,174,53,152,170,34,55,18,83,62,128,124,106,119,159,143,37,35,109,0,12,29,39,160,144,111,68,8,82,87,21,171,157,104,56,33,49,138,182,11,139,2,85,134,43,168,195,126,145,15,69,189,167,184,155,84,3,70,161,13,132,169,65,188,6,150,192,147,71,67,120,191,90,125,51,98,118,74,25,158,116,121,112,19,131
    ]
    path_m6_tspb.append(path_m6_tspb[0])

    visualize_solution(nodesa, path_m6_tspa, costsa, title="M6 Greedy First-Improvement (greedy start) TSPA", save_path="M6_tspa.png")
    visualize_solution(nodesb, path_m6_tspb, costsb, title="M6 Greedy First-Improvement (greedy start) TSPB", save_path="M6_tspb.png")