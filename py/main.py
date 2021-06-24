import numpy as np
# import scipy
import matplotlib.image as img
from matplotlib import pyplot as plt
from math import exp


def adjacent_nodes(node_idx, order, img_shape):
    node = order[node_idx]
    res = list()
    if node[0] != 0:
        res.append((node[0]-1, node[1]))
    if node[1] != 0:
        res.append((node[0], node[1]-1))
    if node[0] != img_shape[0]-1:
        res.append((node[0]+1, node[1]))
    if node[1] != img_shape[1]-1:
        res.append((node[0], node[1]+1))
    return res

def weight(img_, node1, node2):
    BETA = 2000
    return exp(-BETA*(img_[node1[0],node1[1]] - img_[node2[0],node2[1]])**2)

def b_entry(u_node, order, img_shape, seeds, cur_seed, img_):
    res = 0
    adj = adjacent_nodes(u_node, order, img_shape)
    for i in range(len(adj)):
        if adj[i] in seeds:
            if seeds[adj[i]] == cur_seed:
                res += weight(img_, order[u_node], adj[i])
    return res

def adjacent(node1, node2):
    if node1[0] == node2[0]:
        if (node1[1] == node2[1] + 1) or (node1[1] == node2[1] - 1):
            return True
    if node1[1] == node2[1]:
        if (node1[0] == node2[0] + 1) or (node1[0] == node2[0] - 1):
            return True
    return False


def get_L_u(order, seeds, img_):
    l = len(seeds)
    res = np.zeros((len(order)-l, len(order)-l))
    for i in range(l, len(order)):
        for j in range(l, len(order)):
            if i == j:
                adj = adjacent_nodes(i, order, img_.shape)
                for k in range(len(adj)):
                    res[i-l,j-l] += weight(img_, order[i], adj[k])
            elif adjacent(order[i], order[j]):
                res[i-l,j-l] = -weight(img_, order[i], order[j])
    return res

image = img.imread("../images/text2.png")
gray = image[:, :, 0]

# read the dict
seed_types = 0
order = list()
seeds = dict()
with open("../seeds.dat", 'r') as inp_file:
    seed_types = int(inp_file.readline())
    order = [ln.split(' ') for ln in inp_file.readlines()]

for i in range(len(order)):
    seeds[(int(order[i][0]), int(order[i][1]))] = int(order[i][2])
    order[i] = (int(order[i][0]), int(order[i][1]))

for i in range(gray.shape[0]):
    for j in range(gray.shape[1]):
        pair = (i,j)
        if pair not in seeds:
            order.append(pair)

solutions = list()
for cur_seed in range(seed_types):
    # L_u * x = b. Find x
    b = np.zeros((len(order)-len(seeds), 1))

    for i in range(b.shape[0]):
        b[i] = b_entry(i+len(seeds), order, gray.shape, seeds, cur_seed, gray)

    L_u = get_L_u(order, seeds, gray)
    x = np.linalg.solve(L_u, b)
    # x = np.linalg.inv(L_u).dot(b)

    cur_seed_sol = np.zeros(gray.shape)
    for seeded in seeds:
        if seeds[seeded] == cur_seed:
            cur_seed_sol[seeded[0], seeded[1]] = 1
        else:
            cur_seed_sol[seeded[0], seeded[1]] = 0
    for i in range(len(seeds), len(order)):
        cur_seed_sol[order[i][0], order[i][1]] = x[i-len(seeds)]

    solutions.append(cur_seed_sol)

    temp = np.zeros((cur_seed_sol.shape[0], cur_seed_sol.shape[1], 3))
    temp[:,:,0] = cur_seed_sol
    temp[:,:,1] = cur_seed_sol
    temp[:,:,2] = cur_seed_sol
    plt.imshow(temp)
    plt.show()

res_img = np.zeros((gray.shape[0], gray.shape[1],  3))
for i in range(len(order)):
    # set current image cell to the colour that is most probable
    curmax = 0
    curmax_idx = 0
    for seed_type in range(seed_types):
        if solutions[seed_type][order[i][0], order[i][1]] > curmax:
            curmax = solutions[seed_type][order[i][0], order[i][1]]
            curmax_idx = seed_type
    res_img[order[i][0], order[i][1], curmax_idx%3] = 1

plt.imshow(res_img)
plt.show()
