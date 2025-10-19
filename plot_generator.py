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
    
    # TSPA solution (weighted)
    path_tspa = [89, 183, 143, 117, 0, 46, 68, 139, 193, 41, 115, 5, 42, 181, 159, 69, 108, 18, 22, 146, 34, 160, 48, 54, 177, 10, 190, 4, 112, 84, 184, 43, 116, 65, 59, 118, 51, 151, 133, 162, 123, 127, 70, 135, 154, 180, 53, 121, 100, 26, 86, 75, 44, 25, 16, 171, 175, 113, 56, 31, 78, 145, 179, 196, 81, 90, 40, 165, 185, 106, 178, 138, 14, 144, 62, 9, 148, 102, 49, 52, 55, 92, 57, 129, 82, 120, 2, 101, 1, 97, 152, 124, 94, 63, 79, 80, 176, 137, 23, 186, 89]
    
    # TSPB solution (weighted)
    path_tspb = [148, 47, 94, 60, 20, 59, 28, 149, 4, 140, 183, 152, 170, 34, 55, 18, 62, 128, 124, 106, 143, 35, 109, 0, 29, 160, 33, 11, 134, 74, 118, 121, 51, 90, 131, 54, 31, 193, 117, 1, 38, 135, 63, 122, 133, 10, 115, 147, 6, 188, 169, 132, 13, 70, 3, 15, 145, 195, 168, 139, 182, 138, 104, 8, 111, 82, 21, 177, 5, 45, 142, 78, 175, 36, 61, 91, 141, 77, 81, 153, 187, 163, 89, 127, 137, 114, 103, 26, 176, 113, 194, 166, 95, 86, 185, 179, 22, 99, 130, 9, 148]

    visualize_solution(nodesa, path_tspa, costsa, title="Greedy 2-regret with weights heuristics TSPA", save_path="2regret_weighted_a.png")
    visualize_solution(nodesb, path_tspb, costsb, title="Greedy 2-regret with weights heuristics TSPB", save_path="2regret_weighted_b.png")