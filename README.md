# STCP Routing System

## Overview
The **STCP Routing System** is a public transportation routing system designed to compute optimal paths between bus stops in the STCP (Sociedade de Transportes Colectivos do Porto) network. The project employs graph algorithms to calculate different travel routes based on various criteria, including shortest distance, fewest zone changes, minimum bus transfers, and fewest stops.

## Features
### Dataset Parsing
- The program reads bus stop data from `stops.csv` and stores it in a vector called `stops`.
- It processes line information from `lines.csv` and invokes the `addEdges()` function to generate graph edges.
- The `addEdges()` function reads individual route files (`line_xxx_0x.csv`) for both directions, creating edges between stops and storing them as a list of `Nodes`.
- User input determines whether only daytime or nighttime lines are considered. The program also supports excluding specific stops or lines.
- Additional edges are created between stops within a walkable distance defined by the user.

### Graph Representation
The project utilizes a `Graph` class with the following structure:
- **Attributes**:
  - `n`: Graph size.
  - `hasDir`: Boolean indicating if the graph is directional.
  - `nodes`: Vector storing `Node` structs.
  - `forbiddenLines`: List of codes for lines to be excluded from the graph.
- **Node Struct**:
  - `visited`: Boolean indicating whether the node has been visited.
  - `dist`: Estimated distance from the starting point.
  - `pred`: Previous node in the path, if applicable.
  - `adj`: List of edges connected to the node.
- **Edge Struct**:
  - `dest`: Destination node.
  - `zoneChanges`: Indicator of zone changes.
  - `line`: Line name.
  - `distanceRealWorld`: Real-world distance between stops.
  - `onFoot`: Boolean indicating if the segment is walkable.

### Implemented Functionalities and Algorithms
The system provides routing based on the following criteria:
- **Cheapest Route** (Fewest zone changes): Time complexity: `O( |V|² * |E| * log|V| )`.
- **Fastest Route** (Shortest distance): Time complexity: `O( |V|² * |E| * log|V| )`.
- **Least Bus Transfers** (Minimized line changes): Time complexity: `O( |V|² * |E| * |V| * log|V| )`.
- **Fewest Stops**: Time complexity: `O( |V| + |E| )`.

### Bus Transfer Handling
- Transfers between buses can occur at the same stop.
- Walking between stops is allowed if within a user-defined maximum distance.

### Additional Features
- User can select **daytime or nighttime routes**.
- The system allows the user to avoid specific stops and lines.

---

## User Interface
- The program starts by displaying an introductory message where the user can select:
  - Travel mode: Daytime or nighttime.
  - Stops and lines to avoid.
  - Whether to calculate the **Minimum Spanning Tree (MST)** from a specific stop.
- The user then specifies the origin and destination using:
  - Stop code to stop code.
  - Coordinates to coordinates.
  - Stop code to coordinates.
  - Coordinates to stop code.
- If coordinates are used, the program searches for all reachable stops within walking distance and chooses the most suitable one. If no stops are within range, the closest stop is used.
- After selecting the locations, the user can choose one of the four routing options mentioned above.

---

## Noteworthy Feature
The project uses a **single graph structure** to store all information. This approach simplifies the implementation of various route-finding algorithms by simply adding new attributes to the graph's edges. Minor modifications to the Dijkstra algorithm allow it to consider different criteria for finding the optimal path. This structure provides efficient access to all required data without unnecessary repetition or complex interactions between multiple graphs.
