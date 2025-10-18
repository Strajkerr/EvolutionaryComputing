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
    nodes, costs = read_nodes_from_csv(r".\TSPA.csv")
    path_random = [
    86, 6, 59, 8, 34, 156, 74, 91, 182, 103, 127, 163, 153, 168, 148, 44, 164, 185,
    18, 194, 70, 97, 176, 116, 120, 54, 10, 146, 111, 72, 154, 31, 132, 50, 184, 100,
    95, 93, 7, 82, 1, 90, 183, 191, 19, 126, 63, 197, 166, 198, 94, 79, 181, 22, 23,
    53, 139, 11, 137, 138, 57, 5, 15, 162, 41, 161, 151, 117, 51, 46, 48, 28, 55, 109,
    133, 21, 65, 42, 29, 47, 145, 125, 196, 130, 121, 40, 12, 0, 69, 78, 108, 123, 81,
    101, 129, 62, 66, 140, 99, 113
    ]
    path_neigbour_end = [
    154, 180, 53, 63, 176, 80, 151, 59, 65, 116, 42, 193, 41, 139, 115, 46, 0, 183,
    143, 117, 93, 18, 22, 34, 160, 184, 123, 135, 70, 127, 162, 133, 79, 94, 97, 101,
    1, 152, 120, 78, 145, 185, 40, 165, 90, 81, 113, 175, 171, 16, 31, 44, 92, 57,
    106, 49, 144, 62, 14, 178, 52, 55, 129, 2, 75, 86, 26, 100, 121, 148, 137, 23,
    186, 89, 114, 15, 9, 102, 138, 21, 164, 7, 95, 39, 27, 196, 179, 25, 82, 167,
    124, 51, 118, 43, 149, 131, 112, 4, 177, 54
    ]
    path_neighbour = [
        196, 81, 90, 165, 119, 40, 185, 106, 178, 14, 144, 62, 9, 148, 102, 49, 52, 55,
        57, 129, 92, 179, 145, 78, 31, 56, 113, 175, 171, 16, 25, 44, 120, 2, 75, 101,
        1, 152, 97, 26, 100, 86, 53, 154, 70, 135, 180, 94, 63, 79, 133, 127, 123, 162,
        151, 51, 80, 176, 137, 23, 186, 89, 183, 143, 0, 117, 93, 140, 68, 46, 139, 115,
        118, 59, 65, 116, 43, 184, 35, 84, 112, 4, 190, 10, 177, 54, 48, 160, 34, 181,
        42, 5, 41, 193, 159, 146, 22, 18, 69, 108
    ]
    path_greedy = [
        40, 119, 185, 52, 55, 57, 129, 92, 179, 145, 78, 31, 56, 113, 175, 171, 16, 25,
        44, 120, 2, 75, 86, 100, 26, 101, 1, 97, 152, 124, 94, 63, 53, 180, 154, 135,
        70, 127, 123, 112, 4, 84, 35, 184, 190, 10, 177, 54, 48, 160, 34, 181, 146, 22,
        18, 108, 69, 159, 193, 41, 139, 68, 46, 115, 5, 42, 43, 116, 65, 59, 118, 51,
        162, 151, 133, 79, 80, 176, 137, 0, 117, 143, 183, 89, 23, 186, 15, 148, 9, 62,
        102, 144, 14, 49, 178, 106, 165, 90, 81, 196
    ]
    print(len(nodes))
    visualize_solution(nodes, path_random, costs, title="Random Solution",save_path="random_solution.png")
    visualize_solution(nodes, path_neigbour_end, costs, title="Neighbour End Solution", save_path="neighbour_end.png") 
    visualize_solution(nodes, path_neighbour, costs, title="Nearest Neighbour Solution",save_path="neighbour_solution.png")
    visualize_solution(nodes, path_greedy, costs, title="Greedy Solution", save_path="greedy_solution.png") 