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
    lns_A = [23, 137, 176, 80, 51, 151, 162, 133, 79, 63, 94, 124, 148, 9, 62, 102, 49, 14, 144, 21, 7, 164, 27, 90, 81, 196, 40, 119, 165, 185, 106, 178, 52, 55, 57, 129, 92, 179, 145, 78, 31, 56, 113, 175, 171, 16, 25, 44, 120, 2, 75, 86, 101, 1, 152, 97, 26, 100, 121, 53, 180, 154, 135, 70, 127, 123, 112, 4, 190, 10, 177, 54, 184, 160, 34, 181, 42, 43, 116, 65, 59, 115, 46, 139, 41, 193, 159, 146, 22, 18, 108, 140, 93, 68, 117, 0, 143, 183, 89, 186]

    lns_no_ls_A = [118, 59, 65, 116, 43, 184, 42, 181, 193, 41, 139, 108, 18, 159, 22, 146, 34, 160, 54, 177, 4, 112, 131, 149, 151, 51, 176, 80, 122, 63, 79, 133, 162, 123, 127, 70, 135, 154, 180, 158, 53, 121, 100, 26, 86, 75, 101, 1, 97, 94, 124, 152, 2, 129, 92, 57, 179, 145, 78, 120, 44, 25, 16, 171, 175, 113, 31, 81, 90, 27, 165, 40, 185, 55, 52, 106, 178, 138, 164, 7, 21, 144, 14, 49, 102, 62, 9, 37, 148, 15, 186, 137, 89, 183, 143, 0, 117, 68, 46, 115]

    lns_B = [94, 47, 148, 60, 20, 28, 149, 4, 140, 183, 152, 34, 55, 18, 62, 124, 106, 143, 35, 109, 0, 29, 160, 33, 11, 139, 43, 168, 195, 13, 145, 15, 3, 70, 132, 169, 188, 6, 134, 147, 191, 90, 51, 121, 131, 122, 133, 10, 107, 40, 63, 102, 135, 38, 27, 1, 156, 198, 117, 193, 31, 54, 73, 136, 190, 80, 45, 175, 78, 5, 177, 25, 182, 138, 104, 8, 21, 61, 36, 141, 77, 81, 153, 187, 163, 103, 89, 165, 127, 137, 114, 113, 176, 194, 166, 86, 185, 99, 179, 66]

    lns_no_ls_B = [83, 18, 55, 34, 170, 152, 4, 149, 28, 199, 140, 183, 95, 185, 99, 148, 20, 60, 47, 94, 179, 86, 166, 194, 176, 113, 114, 137, 127, 89, 103, 163, 187, 153, 97, 141, 36, 61, 21, 87, 82, 77, 81, 14, 41, 111, 8, 177, 5, 78, 175, 142, 45, 80, 190, 193, 117, 31, 54, 121, 131, 135, 63, 122, 133, 90, 191, 51, 147, 6, 188, 169, 132, 70, 3, 15, 145, 13, 195, 168, 43, 139, 11, 182, 138, 104, 144, 33, 49, 160, 39, 29, 0, 109, 35, 143, 106, 124, 128, 62]

    visualize_solution(nodesa, lns_A, costsa, title="LNS TSPA", save_path="LNS_A.png")
    visualize_solution(nodesb, lns_B, costsb, title="LNS TSPB", save_path="LNS_B.png")
    visualize_solution(nodesa, lns_no_ls_A, costsa, title="LNS without LS TSPA", save_path="LNS_NO_LS_A.png")
    visualize_solution(nodesb, lns_no_ls_B, costsb, title="LNS without LS TSPB", save_path="LNS_NO_LS_B.png")