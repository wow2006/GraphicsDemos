# QuadTree
is a space-partitioning data structure for organizing points in a k-dimensional space.
k-d trees are a useful data structure for several applications, such as searches involving a multidimensional search key (e.g. range searches and nearest neighbor searches).
k-d trees are a special case of binary space partitioning trees.

## Complexity
- Building  O(n log n)
- Inserting O(log n)
- Removing  O(log n)
- Querying  O(n ^ (1 − 1/k) + m) m=points, k=dimension
- Finding   O(log n)

## Resources
- [Wikipedia](https://en.wikipedia.org/wiki/K-d_tree)
- [Image](https://blog.krum.io/k-d-trees/)
- [Optimized KDtree](http://www.danielkoitzsch.de/blog/2015/10/08/kd-tree/)
- [PhotonMapper](https://github.com/ishaan13/PhotonMapper/blob/master/src/KDTree.cpp)
