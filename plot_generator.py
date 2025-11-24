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
    
    path_m_tspb = [131, 121, 1, 24, 156, 198, 117, 54, 31, 193, 190, 80, 45, 175, 78, 5, 177, 36, 61, 91, 141, 21, 104, 8, 82, 77, 81, 153, 187, 163, 103, 89, 127, 137, 114, 113, 180, 176, 194, 166, 86, 95, 130, 99, 185, 179, 66, 94, 47, 148, 60, 20, 28, 149, 4, 199, 140, 183, 152, 170, 34, 55, 18, 62, 128, 124, 106, 159, 143, 111, 35, 109, 0, 29, 160, 33, 138, 182, 11, 139, 168, 195, 145, 15, 3, 70, 169, 188, 6, 147, 51, 191, 90, 10, 133, 107, 40, 63, 135, 122]

    path_m_tspa = [43, 42, 5, 96, 115, 118, 59, 72, 151, 109, 51, 66, 137, 176, 80, 133, 79, 122, 63, 94, 152, 97, 1, 101, 26, 100, 121, 180, 154, 158, 53, 86, 75, 2, 120, 44, 25, 129, 92, 57, 179, 145, 78, 16, 171, 175, 113, 31, 196, 81, 90, 165, 119, 40, 185, 55, 52, 106, 178, 49, 14, 144, 62, 9, 148, 15, 186, 23, 89, 183, 143, 117, 0, 46, 139, 68, 93, 140, 108, 69, 18, 22, 193, 41, 181, 34, 160, 54, 177, 184, 112, 127, 70, 135, 162, 123, 149, 131, 65, 116]


    visualize_solution(nodesa, path_m_tspa, costsa, title="Steepest-Descent MSLS TSPA", save_path="M_tspa.png")
    visualize_solution(nodesb, path_m_tspb, costsb, title="Steepest-Descent MSLS TSPB", save_path="M_tspb.png")