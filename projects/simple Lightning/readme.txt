Waypoints.txt is a file used to determine the waypoints which the main branch will attempt to follow.
The format of the file is as follows:
X   Y   Z
X   Y   Z
...

With each trio of coordinates defining a point in space. The algorithm will then attempt to construct a main branch which goes from 1 to 2 and from 2 to 3 etc.

Branchpoints.txt is a file used to determine the more intricate branches of a user-defined lightning.
Its format is as follows:
A   B   C
X   Y   Z
A   B   C
X   Y   Z
...

A - determines the distance from the root at which the branch will be formed, following a trajectory from the root to the final waypoint and attaching to the closest node.
B - determines the length of the branch.
C - complexity
X Y and Z are the coordinates defining a direction vector which the branch will follow.