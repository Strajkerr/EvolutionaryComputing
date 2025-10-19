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
        print(f"Saved figure → {save_path}")
    if show:
        plt.show()
    plt.close()


if __name__ == "__main__":
    nodesa, costsa = read_nodes_from_csv(r".\TSPA.csv")
    nodesb, costsb = read_nodes_from_csv(r".\TSPB.csv")
    path_random = [49, 144, 62, 148, 15, 114, 186, 23, 89, 183, 153, 170, 117, 93, 140, 36, 
    67, 108, 69, 18, 20, 22, 146, 103, 34, 160, 192, 42, 5, 96, 115, 198, 46, 
    60, 141, 66, 176, 80, 79, 133, 151, 72, 118, 197, 116, 77, 166, 28, 184, 
    147, 177, 10, 190, 4, 112, 156, 123, 162, 194, 70, 6, 154, 158, 53, 182, 
    99, 26, 1, 152, 87, 2, 172, 55, 57, 92, 129, 82, 120, 44, 25, 78, 88, 175, 
    50, 31, 38, 157, 196, 81, 90, 27, 71, 58, 7, 95, 39, 165, 8, 178, 106, 49]
    path_neigbour_end = [117, 30, 42, 196, 108, 80, 162, 45, 5, 7, 177, 123, 25, 182, 139, 11, 49, 
    160, 144, 104, 8, 82, 21, 36, 141, 97, 81, 146, 187, 186, 129, 163, 165, 
    137, 75, 93, 76, 48, 166, 194, 88, 64, 86, 95, 130, 22, 52, 57, 66, 47, 
    148, 60, 20, 59, 28, 149, 199, 140, 183, 174, 83, 34, 170, 53, 184, 155, 
    84, 3, 15, 145, 13, 132, 169, 188, 6, 134, 2, 74, 118, 98, 51, 125, 191, 
    71, 147, 115, 10, 133, 44, 17, 40, 100, 63, 92, 38, 16, 1, 24, 31, 73, 117]

    visualize_solution(nodesa, path_random, costsa, title="Greedy 2-regret heuristics TSPA",save_path="2regret_a.png")
    visualize_solution(nodesb, path_neigbour_end, costsb, title="Greedy 2-regret heuristics TSPB", save_path="2regret_b.png") 