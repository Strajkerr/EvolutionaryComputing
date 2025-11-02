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
    
    path_m_tspb = [139, 11, 29, 0, 109, 35, 34, 18, 62, 124, 106, 143, 81, 153, 187, 163, 89, 127, 137, 114, 103, 113, 180, 176, 88, 194, 166, 86, 95, 185, 179, 66, 94, 47, 148, 20, 28, 140, 183, 152, 184, 155, 3, 70, 161, 15, 145, 168, 195, 13, 132, 169, 188, 6, 147, 10, 133, 107, 40, 63, 102, 135, 122, 90, 51, 121, 131, 38, 27, 16, 1, 156, 198, 117, 193, 31, 54, 73, 136, 190, 80, 45, 175, 78, 5, 177, 36, 61, 141, 77, 82, 21, 104, 8, 111, 144, 160, 33, 138, 182, 139]

    path_m_tspa = [101, 75, 2, 120, 44, 25, 82, 129, 92, 57, 179, 145, 78, 16, 171, 175, 113, 56, 31, 196, 81, 90, 27, 164, 7, 95, 165, 40, 185, 55, 52, 106, 178, 3, 49, 14, 144, 62, 148, 12, 94, 152, 1, 97, 100, 63, 79, 133, 80, 176, 137, 23, 89, 183, 143, 0, 117, 108, 69, 18, 22, 146, 159, 193, 41, 139, 46, 115, 96, 42, 181, 34, 160, 48, 54, 177, 10, 190, 184, 84, 4, 112, 127, 123, 149, 131, 43, 116, 65, 59, 118, 51, 151, 162, 135, 70, 154, 180, 53, 86, 101]


    visualize_solution(nodesa, path_m_tspa, costsa, title="Steepest-Descent (Candidate List, 2-opt+Exchange random start) TSPA", save_path="M_tspa.png")
    visualize_solution(nodesb, path_m_tspb, costsb, title="Steepest-Descent (Candidate List, 2-opt+Exchange random start) TSPB", save_path="M_tspb.png")
