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
    

    # M1 (Steepest descent, 2-node exchange, random start) best cycles from out.txt
    path_m1_tspa = [
      135,126,4,123,122,172,57,185,40,119,39,138,169,196,17,145,106,178,3,155,15,186,23,141,117,114,132,21,7,164,27,174,85,50,113,31,168,78,44,101,97,189,19,152,125,26,53,158,136,182,63,133,161,184,160,192,41,139,198,68,93,140,36,163,108,69,159,146,103,54,10,190,35,11,166,149,151,80,16,171,13,129,128,167,72,116,105,42,5,96,197,118,176,9,62,111,124,180,154,6
    ]
    path_m1_tspa.append(path_m1_tspa[0])

    path_m1_tspb = [
      177,123,190,80,46,175,21,119,95,99,185,176,180,26,137,127,89,163,187,77,58,50,55,174,140,59,101,149,4,53,155,84,161,168,188,115,133,72,40,63,102,38,16,135,125,90,191,71,67,6,43,11,49,0,124,128,110,179,66,94,148,23,20,69,160,33,138,182,2,74,151,54,31,173,157,56,8,159,143,189,167,34,86,166,48,194,81,61,7,142,193,117,198,30,196,42,1,116,118,171
    ]
    path_m1_tspb.append(path_m1_tspb[0])

    visualize_solution(nodesa, path_m1_tspa, costsa, title="M1 Steepest Descent (2-node swap) TSPA", save_path="M1_tspa.png")
    visualize_solution(nodesb, path_m1_tspb, costsb, title="M1 Steepest Descent (2-node swap) TSPB", save_path="M1_tspb.png")

    # M2 (Steepest descent, 2-node exchange, greedy start)
    path_m2_tspa = [
      190,10,177,104,147,54,30,48,160,192,181,195,103,146,22,159,193,41,142,110,191,139,115,198,46,60,118,141,66,176,80,122,94,130,12,124,19,189,99,121,97,152,74,125,87,2,1,101,150,75,86,26,100,53,158,154,6,70,135,194,173,180,136,182,63,79,133,161,162,45,151,51,109,72,59,197,96,5,42,43,28,184,4,112,156,29,126,84,35,11,166,77,47,105,116,65,131,149,24,123
    ]
    path_m2_tspa.append(path_m2_tspa[0])

    path_m2_tspb = [
      9,183,174,83,181,95,130,99,22,179,57,172,52,185,86,166,194,88,113,26,103,114,137,127,165,89,163,186,187,146,97,77,50,58,82,87,21,8,171,157,104,56,33,138,182,139,43,126,195,168,11,49,39,29,109,35,0,12,160,144,68,111,37,41,14,81,153,129,180,176,64,110,128,106,119,159,143,124,62,18,55,34,170,152,53,140,199,4,149,101,28,59,20,23,60,148,47,154,94,66
    ]
    path_m2_tspb.append(path_m2_tspb[0])

    visualize_solution(nodesa, path_m2_tspa, costsa, title="M2 Steepest Descent (2-node swap, greedy start) TSPA", save_path="M2_tspa.png")
    visualize_solution(nodesb, path_m2_tspb, costsb, title="M2 Steepest Descent (2-node swap, greedy start) TSPB", save_path="M2_tspb.png")

    # M3 (Steepest descent, 2-edge / 2-opt, random start)
    path_m3_tspa = [
      13,85,157,38,168,78,92,57,179,91,196,40,39,95,7,165,185,52,106,178,3,32,138,14,49,102,155,132,73,9,33,128,111,12,94,122,63,133,80,176,186,15,64,114,83,89,183,153,0,117,140,108,69,191,139,193,159,103,34,192,160,48,30,54,147,10,24,131,65,105,28,42,96,115,118,72,45,162,6,180,158,53,121,99,19,152,97,26,86,101,150,75,120,82,16,88,171,175,113,56
    ]
    path_m3_tspa.append(path_m3_tspa[0])

    path_m3_tspb = [
      197,1,24,42,30,117,151,173,164,193,190,162,45,78,5,177,91,141,97,77,81,186,163,89,103,75,76,180,176,88,194,166,22,172,57,94,154,60,23,59,149,4,140,183,152,170,55,18,62,128,106,129,119,14,50,87,21,8,68,144,160,33,139,29,12,37,35,109,69,189,184,3,70,145,13,168,188,6,150,192,147,134,85,74,25,121,131,125,90,178,10,133,72,17,107,40,100,122,102,27
    ]
    path_m3_tspb.append(path_m3_tspb[0])

    visualize_solution(nodesa, path_m3_tspa, costsa, title="M3 Steepest Descent (2-opt) TSPA", save_path="M3_tspa.png")
    visualize_solution(nodesb, path_m3_tspb, costsb, title="M3 Steepest Descent (2-opt) TSPB", save_path="M3_tspb.png")

    # M4 (Steepest descent, 2-edge / 2-opt, greedy start)
    path_m4_tspa = [
      48,30,54,147,104,177,10,190,84,4,112,126,156,29,123,24,149,131,65,116,105,47,77,166,35,11,184,28,43,42,5,96,197,59,72,51,151,45,162,161,133,79,63,182,136,180,173,194,135,70,6,154,158,53,86,75,150,101,1,2,87,125,74,152,97,26,100,121,99,189,19,124,12,130,94,122,80,176,66,141,109,118,115,46,198,139,191,110,142,41,193,159,22,146,103,195,181,192,160,34
    ]
    path_m4_tspa.append(path_m4_tspa[0])

    path_m4_tspb = [
      130,95,181,83,174,53,152,170,34,55,18,62,124,143,159,119,106,128,110,64,176,180,129,153,81,14,41,37,111,68,144,160,39,12,0,35,109,29,49,11,168,195,126,43,139,182,138,33,56,104,157,171,8,21,87,82,58,50,77,97,146,187,186,163,89,165,127,137,114,103,26,113,88,194,166,86,185,99,22,179,52,172,57,66,94,154,47,148,60,23,20,59,28,101,149,4,140,183,199,9
    ]
    path_m4_tspb.append(path_m4_tspb[0])

    visualize_solution(nodesa, path_m4_tspa, costsa, title="M4 Steepest Descent (2-opt, greedy start) TSPA", save_path="M4_tspa.png")
    visualize_solution(nodesb, path_m4_tspb, costsb, title="M4 Steepest Descent (2-opt, greedy start) TSPB", save_path="M4_tspb.png")


    path_m5_tspa = [
      46,118,51,89,183,143,0,117,93,140,108,18,159,193,41,34,160,54,177,4,112,84,146,22,195,181,43,116,65,59,94,129,92,179,145,78,31,56,25,82,57,55,52,178,106,185,165,39,27,90,81,2,75,101,86,100,26,97,1,152,167,49,102,62,9,148,63,79,80,176,137,23,186,15,144,14,138,40,196,113,175,171,16,44,120,53,70,135,154,180,133,151,162,123,149,184,42,5,115,139
    ]
    path_m5_tspa.append(path_m5_tspa[0])

    path_m5_tspb = [
      180,176,106,124,62,183,140,149,28,20,148,47,94,66,179,166,194,163,153,77,8,121,131,107,40,63,102,135,122,90,51,147,6,188,169,132,15,145,13,126,195,168,109,35,0,29,139,11,144,111,103,26,114,137,127,89,165,187,146,97,141,91,61,36,177,5,78,175,45,80,190,193,117,31,54,25,104,86,185,130,95,18,55,34,170,152,184,155,3,70,161,134,118,74,182,138,33,160,143,113
    ]
    path_m5_tspb.append(path_m5_tspb[0])

    visualize_solution(nodesa, path_m5_tspa, costsa, title="M5 Greedy First-Improvement (2-node swap) TSPA", save_path="M5_tspa.png")
    visualize_solution(nodesb, path_m5_tspb, costsb, title="M5 Greedy First-Improvement (2-node swap) TSPB", save_path="M5_tspb.png")


    path_m6_tspa = [
      0,46,65,47,72,162,149,131,166,28,184,35,123,127,29,156,112,4,84,77,43,96,41,193,181,192,160,48,104,177,190,10,54,30,34,103,146,195,159,22,18,108,140,68,139,115,5,42,105,116,59,118,51,176,151,133,79,80,122,63,94,26,100,121,53,180,135,70,154,158,86,97,152,1,101,75,2,120,44,25,16,171,175,78,145,185,40,165,106,178,14,49,62,148,137,23,89,183,143,117
    ]
    path_m6_tspa.append(path_m6_tspa[0])


    path_m6_tspb = [
      147,192,150,6,188,65,169,132,161,70,3,15,145,13,126,195,168,49,33,56,144,160,29,0,109,35,34,18,62,124,106,86,176,113,153,81,77,141,91,36,61,21,87,82,111,8,104,138,182,11,139,43,134,85,74,118,98,51,120,67,71,191,90,122,131,121,116,112,19,151,24,1,197,135,63,38,27,16,42,156,198,117,193,31,54,164,73,173,136,190,80,46,162,175,78,142,45,5,177,25
    ]
    path_m6_tspb.append(path_m6_tspb[0])

    visualize_solution(nodesa, path_m6_tspa, costsa, title="M6 Greedy First-Improvement (greedy start) TSPA", save_path="M6_tspa.png")
    visualize_solution(nodesb, path_m6_tspb, costsb, title="M6 Greedy First-Improvement (greedy start) TSPB", save_path="M6_tspb.png")

    path_m7_tspa = [
      62,144,14,49,3,178,106,185,40,119,165,90,81,196,31,56,113,175,171,16,78,145,179,52,55,57,92,129,25,44,120,2,152,1,75,86,101,97,26,100,53,158,180,154,135,70,127,123,35,84,112,4,190,10,177,54,184,160,34,181,146,22,41,193,18,108,140,68,46,0,117,143,183,89,186,23,137,176,51,118,59,115,139,42,43,116,65,131,149,162,151,133,79,80,122,63,94,124,148,9
    ]
    path_m7_tspa.append(path_m7_tspa[0])


    path_m7_tspb = [
      113,26,103,114,137,127,165,89,163,153,77,141,91,79,61,36,177,5,78,175,142,45,162,80,190,136,73,164,54,31,193,117,198,1,135,63,40,107,122,131,121,51,90,191,147,188,169,132,70,3,15,145,13,195,168,139,11,138,33,160,104,21,82,8,111,144,29,0,109,35,143,159,106,124,62,18,55,34,152,183,140,4,149,28,20,60,148,47,94,66,179,22,99,95,185,86,166,194,176,180
    ]
    path_m7_tspb.append(path_m7_tspb[0])

    visualize_solution(nodesa, path_m7_tspa, costsa, title="M7 Greedy First-Improvement (2-opt, random start) TSPA", save_path="M7_tspa.png")
    visualize_solution(nodesb, path_m7_tspb, costsb, title="M7 Greedy First-Improvement (2-opt, random start) TSPB", save_path="M7_tspb.png")


    path_m8_tspa = [
      198,115,96,5,42,160,48,54,30,177,10,190,4,112,84,35,184,28,43,105,116,65,47,131,149,24,123,162,151,72,59,118,51,176,80,79,133,194,127,70,135,154,158,53,180,63,94,124,152,1,97,26,100,86,101,75,2,120,44,16,78,145,40,185,55,52,106,178,49,14,144,62,9,148,137,23,89,183,143,170,0,117,93,140,36,108,69,18,22,146,34,181,195,159,193,41,139,110,68,46
    ]
    path_m8_tspa.append(path_m8_tspa[0])


    path_m8_tspb = [
      141,61,36,175,5,177,21,87,82,8,104,182,138,33,49,11,139,168,195,169,188,70,3,145,29,109,35,0,160,144,56,111,41,81,119,159,143,106,124,62,18,55,34,170,152,53,140,4,149,101,28,59,20,23,60,154,94,66,47,148,9,199,183,174,83,181,95,130,99,22,179,57,172,52,185,86,110,128,64,166,194,88,176,180,113,114,137,127,165,89,103,26,163,129,186,153,187,146,97,77
    ]
    path_m8_tspb.append(path_m8_tspb[0])

    visualize_solution(nodesa, path_m8_tspa, costsa, title="M8 Greedy First-Improvement (2-opt, greedy start) TSPA", save_path="M8_tspa.png")
    visualize_solution(nodesb, path_m8_tspb, costsb, title="M8 Greedy First-Improvement (2-opt, greedy start) TSPB", save_path="M8_tspb.png")