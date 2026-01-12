# Computer Graphics Course Project - Fruit Ninja Game

## Project Overview
**Project Title**: Particle System-Based Fruit Ninja: Make Fruit Ninja Great Again  
**Course**: Computer Graphics (08306027)  
**Semester**: Fall 2025-2026  
**Institution**: School of Computer Engineering and Science  
**Majors**: Computer Science and Technology, Intelligent Science and Technology  

## Team Members
- Sun Chenhao (23122690)
- Zhang Siming (24122886)  
- Pan Qiangchao (23123012)
- Wang Letian (23120844)

## Abstract
This project designs and implements an interactive graphics rendering system based on Win32/GDI, innovatively integrating a "Fruit Ninja" game module. The system core implements precise rendering of basic primitives including lines (Bresenham's algorithm), circles/arcs (midpoint algorithm), Bezier curves, and B-spline curves, along with advanced graphic processing functions such as scanline filling and line segment clipping (Cohen-Sutherland algorithm). The distinctive feature lies in combining computer graphics algorithms with game development, utilizing a particle system to simulate fruit-cutting effects, calculating parabolic trajectories using Bezier curves, and achieving real-time collision detection based on geometric calculations. The system not only validates graphics theory but also provides a feasible path for applying algorithmic knowledge to complex interactive scenarios.

## Key Features
### 1. Basic Primitive Drawing
- **Lines**: Bresenham algorithm with variable thickness and dash patterns
- **Circles/Arcs**: Midpoint circle algorithm with arc/sector support
- **Bezier Curves**: De Casteljau algorithm for smooth curve generation
- **B-spline Curves**: Uniform and non-uniform B-splines with adjustable orders
- **Ellipses**: Midpoint ellipse algorithm for complete and partial ellipses

### 2. Advanced Graphics Processing
- **Polygon Filling**: Scanline algorithm with edge table (ET) and active edge table (AET)
- **Line Clipping**: Cohen-Sutherland algorithm for efficient segment clipping
- **Geometric Transformations**: Translation, rotation, scaling with homogeneous coordinates
- **Object Grouping**: Combine multiple objects and manipulate as a single entity

### 3. Fruit Ninja Game Module
- **Parabolic Motion**: Bezier curves simulate realistic fruit trajectories
- **Collision Detection**: Real-time line-circle intersection testing
- **Particle System**: Dynamic visual effects for fruit cutting with independent life cycles
- **Game Mechanics**: Score calculation, combo system, health management
- **Interactive Gameplay**: Mouse-based fruit slicing with responsive feedback

## System Architecture
### Core Classes
- **GraphicsEngine**: Manages object linked list and double-buffered rendering
- **Object**: Base class for all graphical objects with virtual draw/transform/select methods
- **Derived Classes**: 
  - `Line` (Bresenham algorithm with dash/width control)
  - `Circle` (Midpoint circle algorithm with arc support)
  - `Bezier` (De Casteljau algorithm for curve generation)
  - `PolygonShape` (Scanline filling with ET/AET management)
  - `FruitNinjaGame` (Complete game logic with physics and rendering)

### Module Interaction
- **Event Handling**: Multi-level event response with state machine management
- **Algorithm Module**: Encapsulated algorithms with standardized interfaces
- **Game Integration**: Tight coupling between graphics rendering and game physics
- **Communication**: Event-driven asynchronous communication with standardized data formats

## Technical Implementation

### Key Algorithms
1. **Bresenham Line Algorithm**: Integer-only operations avoiding floating-point calculations
2. **Midpoint Circle Algorithm**: Eight-way symmetry for efficient circle generation
3. **De Casteljau Algorithm**: Recursive linear interpolation for Bezier curves
4. **Scanline Filling**: ET/AET-based polygon filling with O(n log n) complexity
5. **Cohen-Sutherland Clipping**: Region coding for fast line segment rejection
6. **Bezier-based Physics**: Parabolic trajectories using quadratic Bezier curves
7. **Geometric Collision Detection**: Line-circle intersection via quadratic equation solving

### Performance Optimizations
- **Double Buffering**: Eliminates screen flickering
- **Memory Management**: Object pooling and smart pointers
- **Level of Detail (LOD)**: Adaptive rendering based on distance
- **Caching Mechanisms**: Reduced redundant calculations
- **Asynchronous Processing**: Physics and rendering thread separation

## User Interface & Controls

### Drawing Modes (Keyboard Shortcuts)
- **L**: Line drawing mode
- **C**: Circle drawing mode → **CA**: Arc mode
- **E**: Ellipse drawing mode → **EA**: Ellipse arc mode
- **B**: Bezier curve mode → **BS**: B-spline mode (2/3 for order selection)
- **P**: Polygon mode
- **F**: Fill mode (with RGB color input via Y)
- **X**: Clipping mode
- **N**: Fruit Ninja game mode

### Object Manipulation
- **Select Mode**: Right-click to select objects
- **Delete**: Remove selected objects
- **T/R/Z**: Translate/Rotate/Scale modes
- **M/W**: Merge/Separate object groups
- **Ctrl**: Toggle automatic center specification
- **V**: Join two polygons at selected edges

## Game Features
- **Fruit Types**: 4 distinct fruits with unique visual styles
- **Bomb Mechanism**: Black circles that end the game when sliced
- **Particle Effects**: Dynamic juice splatter when fruits are cut
- **Combo System**: Multiplier for consecutive successful cuts
- **Progressive Difficulty**: Speed and spawn rate increase over time
- **Health Management**: Three lives with visual feedback

## Performance Characteristics
- **Frame Rate**: Stable 60 FPS rendering
- **Input Latency**: <50ms response time
- **Algorithm Efficiency**: 
  - Bresenham: O(n) complexity, integer-only operations
  - Scanline filling: O(n log n) vs original O(n²)
  - Collision detection: Real-time geometric calculations
- **Stability**: Multi-layer exception handling with fallback mechanisms
- **Memory**: Efficient allocation with prevention of fragmentation

## Development Environment
- **Language**: C++
- **Platform**: Windows
- **Graphics API**: Win32/GDI
- **Compiler**: Visual Studio
- **Architecture**: Modular design with clear separation of concerns

## Academic Contributions
This project demonstrates practical application of computer graphics theories including:
- Rasterization algorithms for primitive generation
- Curve generation using parametric equations
- Geometric transformations and clipping
- Real-time collision detection
- Particle system implementation
- Double-buffered rendering techniques
- Interactive system design principles

## References
1. Francis S Hill, Jr., Stephen M Kelley. *Computer Graphics Using OpenGL (3rd Edition)*. Tsinghua University Press, 2009.
2. Bresenham, J. E. *Algorithm for computer control of a digital plotter*. IBM Systems Journal, 1965, 4(1): 25-30.
3. de Casteljau, P. *Outillage methodes calcul*. André Citroën, Paris, 1959.

## Acknowledgments
We extend our sincere gratitude to Professor Zhang Jingxin for her guidance throughout this project. We also thank our teaching assistants for their valuable suggestions, our team members for their collaboration, and our families for their unwavering support.

---

*This project represents a comprehensive integration of computer graphics theory with practical game development, demonstrating both algorithmic understanding and software engineering skills.*
