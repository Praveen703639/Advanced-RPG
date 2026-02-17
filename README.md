# ⚔️ Advanced-RPG 
**A High-Performance, Systems-Oriented RPG Engine**

This isn't just a game; it's a technical demonstration of engine-level architecture in C++. Designed for scalability and cache-locality, aiming for AAA performance standards.

---

### 💻 Core Engineering Highlights

* **Custom Entity-Component System (ECS):** Architected for memory efficiency and cache-friendly iteration, bypassing standard OOP overhead.
* **Memory Management:** Implemented automated lifecycle management using custom Smart Pointers to ensure zero leaks and high safety.
* **Data-Driven Combat Engine:** Built a flexible system where abilities and stats are driven by external data, allowing for rapid balancing without recompilation.
* **High-Performance C++:** Focus on `O(1)` or `O(log n)` operations for critical path systems.

---

### 🛠 Tech Stack
`C++20` | `Unreal Engine 5 (Source)` | `Multithreading` | `Data-Oriented Design`

### 🏗 Architecture Preview
> [!TIP]
> Check the `/Source/Core` folder to see the ECS implementation and memory allocator logic.
