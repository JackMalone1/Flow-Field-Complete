# Flow-Field
---
Path to goal is created by following the best node for each tile starting from the start tile to the end tile.
The best tile is generated from the vector field and a vector between the two is generated which can be used
to travel between the if going for smooth movement.
The npc travels along this path using the best goal tiels instead of the vectors as I was going for tiles based
movement rather than smooth movement but this should be able to be altered by using the vectors
and then checking if it has reached the next tile in the queue of tiles before updating its velocity.
