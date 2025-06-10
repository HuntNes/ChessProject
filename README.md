# ChessProject ♟️ - Custom Chess Game in C++

A customizable chess game with fantasy pieces, portals, and JSON-based configuration.  
Developed in modern C++20 as part of the CSE211 Data Structures course.

##  Features

-  Traditional chess rules implemented
-  Portal system for teleporting pieces
-  Custom pieces (e.g., Teleporter, Archer)
-  Configurable via JSON (board size, pieces, portals)
-  Move validation with graph traversal (BFS/DFS)
-  Undo system using stack
-  Cooldown queue for portal usage

## 🛠️ Build & Run Instructions (macOS / Linux)

### 🔹 1. Repoyu klonla

```bash
git clone https://github.com/HuntNes/ChessProject.git
cd ChessProject

rm -rf build         # (Optional) Clean up previous builds
mkdir build
cd build
cmake ..
make

./CHESS3  # play
