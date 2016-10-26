## Belief Propagation on Tree Simple Implementation

### Introduction
This code is part of an assigment of CS5340 at School of Computing, NUS. 
The code implemented a simple *sum-product* (also called *Belief Propagation*) algorithm on tree-strucetured graph.
And in the main function, we constructed a small tree as below shows, then we run the algorithm on the tree and print
the marginals of each node to screen.

```
    1  
   /  \  
  2    3  
 / \    \  
4   5    6  
```

P(x,y) = [[1, 0.5], [0.5, 1]] for each edge.  

P(x) = [0.7, 0.3] for node 1,3,5 and P(x) = [0.1, 0.9] for node 2,4,7.

### Directory
```console
main.cpp
README.md
```

### Enviroment
Ubuntu 14.04 LTS

g++ 4.8.4

### Complie
```console
$ g++ -O2 main.cpp -o run.out
```
### Run
```console
$ ./run.out
```
