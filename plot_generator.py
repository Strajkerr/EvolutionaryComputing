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
    nodesa, costsa = read_nodes_from_csv("TSPA.csv")
    nodesb, costsb = read_nodes_from_csv("TSPB.csv")
    
    path_m_tspb = [12, 29, 160, 33, 144, 111, 8, 104, 138, 182, 11, 139, 43, 168, 195, 145, 15, 3, 70, 13, 132, 169, 188, 6, 134, 147, 71, 120, 191, 90, 125, 51, 98, 118, 116, 121, 131, 122, 135, 63, 38, 1, 198, 117, 54, 31, 193, 190, 80, 175, 5, 177, 21, 82, 61, 36, 79, 91, 141, 97, 77, 81, 153, 159, 143, 106, 124, 128, 86, 176, 163, 165, 127, 89, 103, 113, 194, 166, 185, 95, 99, 179, 94, 47, 148, 20, 28, 149, 4, 140, 183, 152, 55, 83, 62, 18, 34, 35, 109, 0]

    path_m_tspa = [178, 3, 14, 144, 49, 102, 62, 9, 15, 148, 167, 124, 94, 63, 79, 133, 80, 176, 137, 23, 186, 114, 89, 183, 143, 0, 117, 68, 46, 115, 139, 69, 108, 18, 22, 159, 193, 41, 5, 42, 181, 34, 160, 54, 177, 10, 4, 112, 84, 184, 43, 116, 65, 59, 118, 51, 151, 162, 123, 127, 70, 135, 154, 180, 53, 100, 26, 86, 75, 101, 1, 97, 152, 2, 129, 120, 44, 25, 16, 171, 175, 113, 56, 31, 78, 145, 92, 57, 55, 52, 179, 196, 81, 90, 27, 165, 119, 40, 185, 106]


    visualize_solution(nodesa, path_m_tspa, costsa, title="Steepest-Descent (List of moves, 2-opt+Exchange random start) TSPA", save_path="M_tspa.png")
    visualize_solution(nodesb, path_m_tspb, costsb, title="Steepest-Descent (List of moves, 2-opt+Exchange random start) TSPB", save_path="M_tspb.png")
