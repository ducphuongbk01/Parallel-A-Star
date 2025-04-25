import subprocess
import osmnx as ox

SOURCE_POINT = (10.768856, 106.725019)
DESTINATION_POINT = (10.839613, 106.845798)

def calculate_path_length(G, path):
    total_length = 0
    for u, v in zip(path[:-1], path[1:]):
        # OSMnx graphs can have multiple edges (MultiDiGraph), so we take the shortest if there are multiple
        edge_data = G.get_edge_data(u, v)
        if edge_data is not None:
            # If multiple edges (e.g., different lanes), choose the shortest
            min_length = min([d.get('length', 0) for d in edge_data.values()])
            total_length += min_length
        else:
            print(f"No edge between {u} and {v}")
    return total_length


if __name__ == "__main__":
    graph = ox.load_graphml(filepath="data/hcm.graphml")
    src_node = ox.nearest_nodes(graph, SOURCE_POINT[1], SOURCE_POINT[0])
    des_node = ox.nearest_nodes(graph, DESTINATION_POINT[1], DESTINATION_POINT[0])
    
    result = subprocess.run(["./astar", str(src_node), str(des_node)], capture_output=True) \
        .stdout.decode("utf-8").split("\n")[:-1]
    
    thread = result[0]
    time = result[-1]
    search_path = list(map(int, result[1:-2]))
    base_path = [3700940837, 6855606318, 6855606317, 6855606316, 6855606314, 6855606313, 4907667941, 2275303187, 4907667773, \
        4907667699, 11640279650, 6848079309, 6639800650, 11873573659, 714480604, 714480394, 366414360, 6785918021, \
        6785918023, 11896336823, 11896336783, 1834116615, 2275303191, 724232845, 5614661188, 5614661186, 1834117114, \
        6627017363, 5735458215, 6754398775, 6754398772, 366384168, 6754398776, 676036667, 6754371052, 2028465966, \
        5748445128, 6754344791, 5748445135, 6754343974, 6754343975, 676035822, 6760299234, 6754296825, 6754296829, \
        6754296832, 4066414553, 6754296831, 6754296833, 2217825343, 6754129803, 4519453270, 5778616014, 4934947979, \
        2217876796, 2217876792, 2217878484, 2217878488, 2024972808, 2217884564, 2217884568, 5755237684, 3301856095, \
        4615796000, 12084602277, 366419231, 8359554062, 5755237908, 366447236, 366408655, 5755237637, 2956123411, \
        2956123408, 366404347, 8359656486, 366388064, 2956123339, 5416650864, 2956123353, 2956123340, 366473118, \
        2956123387, 2956123399, 5753513879, 5755024168, 5753513890, 366474942, 8359595395, 5753514288, 2956123390, \
        366473718, 366468161, 2956123391, 366403936, 5755051121, 5755051122, 366420716, 366458976, 366456389, 366466432, \
        366411387, 366457253, 366436909, 366426738, 11024999130, 7803234930, 7758176956, 7940744879, 7758176955, \
        366372254, 7793356612, 11560783655, 7793371415, 7957622509, 7957622510, 7957534573, 10197486245, 10197486241, \
        10197486247, 10197486238, 11346641691]
    
    path = [search_path, base_path]
    ox.plot_graph_routes(graph, path, route_colors=['blue', 'red'], node_size=2)
    print("Thread ", thread, "first find the path")
    print("Path cost: ", calculate_path_length(graph, search_path))
    print("Base cost: ", calculate_path_length(graph, base_path))
    print("Search time:", time, "ms")
